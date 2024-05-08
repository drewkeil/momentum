#include <iostream>
#include <algorithm>
#include <cmath>

#include "gameObjects.h"

aabb::aabb(float x, float y, float width, float height)
	:topLeft({x,y}), size({width,height}) {}

aabb::aabb(){}

bool aabb::colliding(aabb& other){
	bool vert=((topLeft.y>(other.topLeft.y+other.size.y))||((topLeft.y+size.y)<other.topLeft.y));
	bool horz=((topLeft.x>(other.topLeft.x+other.size.x))||((topLeft.x+size.x)<other.topLeft.x));
	return !(horz||vert);
}


void playerObject::collide(aabb& other){ // split this into 2 functions, collide_x and collide_y
	float minX,minY;
	minX=other.topLeft.x-(topLeft.x+size.x);
	if(abs(minX)>abs((other.topLeft.x+other.size.x)-topLeft.x))
		minX=(other.topLeft.x+other.size.x)-topLeft.x;
	minY=(other.topLeft.y+other.size.y)-topLeft.y;
	if(abs(minY)>abs(other.topLeft.y-(topLeft.y+size.y)))
		minY=other.topLeft.y-(topLeft.y+size.y);
	if(abs(minX)<abs(minY)){
		if(minX<0&&velocity.x>0){
			//if(velocity.x>12){
			//	respawn();
			//	return;
			//}
			velocity.x=0;
		}else if(velocity.x<0&&minX>0){
			//if(velocity.x<-12){
			//	respawn();
			//	return;
			//}
			velocity.x=0;
		}
		topLeft.x+=minX;
	}else{
		if(minY<0&&velocity.y>0){
			//if(velocity.y>12){
			//	respawn();
			//	return;
			//}
			velocity.y=0;
			grounded=true;
			coyote=5;
			shifts=std::max(shifts,1);
		}else if(velocity.y<0&&minY>0){
			//if(velocity.y<-12){
			//	respawn();
			//	return;
			//}
			velocity.y=0;
		}
		topLeft.y+=minY;
	}
}

void playerObject::process_input(uint8_t input, bool jumpHeld){
	velocity.y+=(jumpHeld&&jumpTimer) ? 0.15:0.2;
	jumpBuffer-=std::min(jumpBuffer,1);
	shiftBuffer-=std::min(shiftBuffer,1);
	jumpTimer-=std::min(jumpTimer,1);
	if(input&4)
		velocity.x-=0.35/(1.5*abs(velocity.x)+2.5);
	if(input&8)
		velocity.x+=0.35/(1.5*abs(velocity.x)+2.5);
	if((input&32))
		jumpBuffer=5;
	if(jumpBuffer&&coyote){
		velocity.y-=3.6f;
		jumpBuffer=0;
		coyote=0;
		jumpTimer=24;
	}
	if(input&16)
		shiftBuffer=5;
	if(shifts&&shiftBuffer){
		--shifts;
		shiftBuffer=0;
		velocity.y-=std::max(0.05f*jumpTimer-0.64f,0.f);
		jumpTimer=0;
		if(input&0x1){
			velocity.y=-sqrt(velocity.x*velocity.x+velocity.y*velocity.y);
			velocity.x=0;
			if(input&8){
				velocity.y/=1.4142135623f;
				velocity.x=-velocity.y;
			}else if(input&4){
				velocity.y/=1.4142135623f;
				velocity.x=velocity.y;
			}
		}else if(input&0x2){
			velocity.y=sqrt(velocity.x*velocity.x+velocity.y*velocity.y);
			velocity.x=0;
			if(input&8){
				velocity.y/=1.4142135623f;
				velocity.x=velocity.y;
			}else if(input&4){
				velocity.y/=1.4142135623f;
				velocity.x=-velocity.y;
			}
		}else if(input&0x4){
			velocity.x=-sqrt(velocity.x*velocity.x+velocity.y*velocity.y);
			velocity.y=0;
		}else if(input&0x8){
			velocity.x=sqrt(velocity.x*velocity.x+velocity.y*velocity.y);
			velocity.y=0;
		}
	}
	if(grounded){
		grounded=false;
		velocity.x*=0.995f;
		if(velocity.x>0&&!(input&8))
			velocity.x-=std::min(velocity.x, 0.5f);
		else if(velocity.x<0&&!(input&4))
			velocity.x-=std::max(velocity.x, -0.5f);
	}else
		coyote-=std::min(coyote, 1);	
}

void playerObject::update(){ // split this into update_x and update_y
	topLeft.x+=velocity.x;
	topLeft.y+=velocity.y;
}

void playerObject::respawn(){
	topLeft.x=spawnPoint.x;
	topLeft.y=spawnPoint.y;
	velocity.x=0;
	velocity.y=0;
	jumpBuffer=0;
	shiftBuffer=0;
	grounded=false;
	coyote=0;
	jumpTimer=0;
}

playerObject::playerObject(){
	size.x=PLAYER_WIDTH;
	size.y=PLAYER_HEIGHT;
	color=sf::Color::Black;
	velocity.x=0;
	velocity.y=0;
	jumpBuffer=0;
	shiftBuffer=0;
	grounded=false;
	coyote=0;
	shifts=0;
	jumpTimer=0;
}

void playerObject::show_velocity(std::string& str){
	str=std::to_string(velocity.x)+", "+std::to_string(velocity.y);
}

movingPlatform::movingPlatform(std::istream& is)
	:delay(0), moveIdx(1), velocity(0.f){ // idx is 1 b/c it starts at 0 already
	//TODO: all of this -._(00-)_.-
}

void movingPlatform::move(){
	if(delay>0){
		--delay;
		return;
	}
	// figure out which direction we're going
	float xdir=path[moveIdx].position.x-topLeft.x;
	float ydir=path[moveIdx].position.y-topLeft.y;
	float mag=sqrt(xdir*xdir+ydir*ydir);
	xdir/=mag;
	ydir/=mag;
	// do moving
	velocity=std::min(velocity+acceleration, maxSpeed);
	if(velocity>=mag){
		topLeft=path[moveIdx].position;
		moveIdx=(moveIdx+1)%path.size();
		velocity=0.f;
		delay=path[moveIdx].frameDelay;
	}else{
		topLeft.x+=velocity*xdir;
		topLeft.y+=velocity*ydir;
	}
}
