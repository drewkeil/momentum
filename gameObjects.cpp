#include <iostream>
#include <algorithm>
#include <cmath>

#include "gameObjects.h"

aabb::aabb(float x, float y, float width, float height)
	:topLeft({x,y}), size({width,height}) {}

aabb::aabb(){}

bool aabb::colliding(aabb& other){ // this probably works i think
	bool vert=!((topLeft.y>(other.topLeft.y+other.size.y))||((topLeft.y+size.y)<other.topLeft.y));
	bool horz=!((topLeft.x>(other.topLeft.x+other.size.x))||((topLeft.x+size.x)<other.topLeft.x));
	return horz&&vert;
}


void playerObject::collide(aabb& other){ // split this into 2 functions, collide_x and collide_y
	float minX,minY;
	//float prevx=abs(velocity.x);
	//float prevy=abs(velocity.y);
	minX=other.topLeft.x-(topLeft.x+size.x);
	if(abs(minX)>abs((other.topLeft.x+other.size.x)-topLeft.x))
		minX=(other.topLeft.x+other.size.x)-topLeft.x;
	minY=(other.topLeft.y+other.size.y)-topLeft.y;
	if(abs(minY)>abs(other.topLeft.y-(topLeft.y+size.y)))
		minY=other.topLeft.y-(topLeft.y+size.y);
	if(abs(minX)<abs(minY)){
		if(minX<0&&velocity.x>0){
			if(velocity.x>35){
				respawn();
				return;
			}
			velocity.x=0;
		}else if(velocity.x<0&&minX>0){
			if(velocity.x<-35){
				respawn();
				return;
			}
			velocity.x=0;
		}
		//topLeft.y-=(velocity.y/prevx)*abs(minX); // try this some other way i guess
		topLeft.x+=minX;
	}else{
		if(minY<0&&velocity.y>0){
			if(velocity.y>35){
				respawn();
				return;
			}
			velocity.y=0;
			grounded=true;
			coyote=5;
			shifts=std::max(shifts,1);
		}else if(velocity.y<0&&minY>0){
			if(velocity.y<-35){
				respawn();
				return;
			}
			velocity.y=0;
		}
		//topLeft.x-=(velocity.x/prevy)*abs(minY); // try this some other way
		topLeft.y+=minY;
	}
	//std::cout<<"uncolided "<<minY<<' '<<velocity.y<<std::endl;
}

void playerObject::process_input(uint8_t input){
	++velocity.y;
	if(input&4)
		velocity.x-=1;
	if(input&8)
		velocity.x+=1;
	if((input&32))
		jumpBuffer=5;
	if(jumpBuffer&&(grounded||coyote)){
		velocity.y-=15;
		jumpBuffer=0;
		coyote=0;
	}
	if(input&16)
		shiftBuffer=5;
	if(shifts&&shiftBuffer){
		--shifts;
		shiftBuffer=0;
		if(input&0x1){
			velocity.y=-(abs(velocity.x)+abs(velocity.y));
			velocity.x=0;
			if(input&8){
				velocity.y/=1.4142135623f;
				velocity.x=-velocity.y;
			}else if(input&4){
				velocity.y/=1.4142135623f;
				velocity.x=velocity.y;
			}
		}else if(input&0x2){
			velocity.y=(abs(velocity.x)+abs(velocity.y));
			velocity.x=0;
			if(input&8){
				velocity.y/=1.4142135623f;
				velocity.x=velocity.y;
			}else if(input&4){
				velocity.y/=1.4142135623f;
				velocity.x=-velocity.y;
			}
		}else if(input&0x4){
			velocity.x=-(abs(velocity.x)+abs(velocity.y));
			velocity.y=0;
		}else if(input&0x8){
			velocity.x=(abs(velocity.x)+abs(velocity.y));
			velocity.y=0;
		}
	}
	if(grounded){
		grounded=false;
		velocity.x*=0.99f;
		if(velocity.x>0&&!(input&8))
			velocity.x-=std::min(velocity.x, 3.f);
		else if(velocity.x<0&&!(input&4))
			velocity.x-=std::max(velocity.x, -3.f);
	}else
		coyote-=std::min(coyote, 1);	
	//std::cout<<velocity.y<<std::endl;
}

void playerObject::update(){ // split this into update_x and update_y
	topLeft.x+=velocity.x;
	topLeft.y+=velocity.y;
}

void playerObject::respawn(){
	velocity.x=0;
	velocity.y=0;
	jumpBuffer=0;
	shiftBuffer=0;
	grounded=false;
	coyote=0;
}

playerObject::playerObject(){
	size.x=20;
	size.y=50;
}
