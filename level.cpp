#include "level.h"
#include <iostream>

void level::get_drawn(std::vector<aabb>& drawn){
	for(aabb p:platforms)
		drawn.push_back(p);
	for(aabb s:spikes)
		drawn.push_back(s);
}

void level::load_level(std::istream& is){
	int p,s;
	is>>goal>>deathHeight;
	is>>spawn.x>>spawn.y>>p>>s;
	platforms.resize(p);
	for(int i=0;i<p;++i){
		aabb& pl=platforms[i];
		is>>pl.topLeft.x>>pl.topLeft.y>>pl.size.x>>pl.size.y;
		pl.color=sf::Color::Black;
	}
	spikes.resize(s);
	for(int i=0;i<s;++i){
		aabb& sp=spikes[i];
		is>>sp.topLeft.x>>sp.topLeft.y;
		sp.size.x=5;
		sp.size.y=5;
		sp.color=sf::Color::Red;
	}
}

void level::collide_player(playerObject& p){
	if(p.topLeft.y>deathHeight)
		p.respawn();
	for(aabb spike:spikes){
		if(spike.colliding(p)){
			p.respawn();
			return;
		}
	}
	for(aabb platform:platforms){
		if(platform.colliding(p)){
			//std::cout<<"hit platform"<<std::endl;
			p.collide(platform);
		}
	}
	complete=p.topLeft.x+p.size.x/2>goal;
}

void level::ready_player(playerObject& p){
	p.spawnPoint.x=spawn.x;
	p.spawnPoint.y=spawn.y;
	p.respawn();
}

bool level::level_finished(playerObject& p){
	return (p.topLeft.x+p.size.x/2)>=goal;
}

void level::print_level(std::ostream& os){
	os<<goal<<' '<<deathHeight<<'\n';
	os<<spawn.x<<' '<<spawn.y<<'\n';
	os<<platforms.size()<<' '<<spikes.size()<<'\n';
	for(aabb& rect:platforms)
		os<<rect.topLeft.x<<' '<<rect.topLeft.y<<' '<<rect.size.x<<' '<<rect.size.y<<'\n';
	for(aabb& rect:spikes)
		os<<rect.topLeft.x<<' '<<rect.topLeft.y<<'\n';
}
