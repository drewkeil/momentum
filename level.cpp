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
	is>>spawn.x>>spawn.y>>p>>s;
	platforms.resize(p);
	for(int i=0;i<p;++i){
		aabb& pl=platforms[i];
		is>>pl.topLeft.x>>pl.topLeft.y>>pl.size.x>>pl.size.y;
	}
	spikes.resize(s);
	for(int i=0;i<s;++i){
		aabb& sp=spikes[i];
		is>>sp.topLeft.x>>sp.topLeft.y>>sp.size.x>>sp.size.y;
	}
}

void level::collide_player(playerObject& p){
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
}

void level::ready_player(playerObject& p){
	p.spawnPoint.x=spawn.x;
	p.spawnPoint.y=spawn.y;
	p.respawn();
}
