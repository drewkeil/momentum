#include "level.h"
#include <iostream>
#include <string>
#include <fstream>

void level::get_drawn(std::vector<aabb>& drawn){
	for(aabb p:platforms)
		drawn.push_back(p);
	for(aabb s:spikes)
		drawn.push_back(s);
}

void level::load_level(std::string lvlname, std::string prevName){
	std::ifstream fin;
	name=lvlname;
	lvlname.insert(0,"levels/");
	fin.open(lvlname);
	if(!fin.is_open()){
		std::cerr<<"unable to open "<<lvlname<<std::endl;
		exit(1);
	}

	//section 1
	std::string tmp;
	std::getline(fin, tmp);
	while(tmp[0]=='#')
		std::getline(fin, tmp);
	deathHeight=std::stoi(tmp);
	
	//section 2
	std::getline(fin, tmp);
	while(tmp[0]=='#')
		std::getline(fin, tmp);
	int p, s;
	bool found=false;
	s=std::stoi(tmp);
	for(int i=0;i<s;++i){
		fin>>tmp;
		if(tmp==prevName){
			fin>>spawn.x>>spawn.y;
			found=true;
		}else
			fin>>p>>p;
	}
	if(!found)
		fin>>spawn.x>>spawn.y;
	else
		fin>>p>>p;
	
	//section 3
	std::getline(fin, tmp);
	while(tmp[0]=='#'||tmp[0]=='\0')
		std::getline(fin, tmp);
	p=std::stoi(tmp);
	goals.resize(p);
	for(aabb& g:goals)
		fin>>g.topLeft.x>>g.topLeft.y>>g.size.x>>g.size.y;
	next.resize(p);
	for(std::string& lvn:next)
		fin>>lvn;

	//section 4
	std::getline(fin, tmp);
	while(tmp[0]=='#'||tmp[0]=='\0')
		std::getline(fin, tmp);
	p=std::stoi(tmp);
	fin>>s;
	platforms.resize(p);
	for(int i=0;i<p;++i){
		aabb& pl=platforms[i];
		fin>>pl.topLeft.x>>pl.topLeft.y>>pl.size.x>>pl.size.y;
		pl.color=sf::Color::Black;
	}
	spikes.resize(s);
	for(int i=0;i<s;++i){
		aabb& sp=spikes[i];
		fin>>sp.topLeft.x>>sp.topLeft.y;
		sp.size.x=5;
		sp.size.y=5;
		sp.color=sf::Color::Red;
	}
	fin.close();
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
}

void level::ready_player(playerObject& p){
	p.spawnPoint.x=spawn.x;
	p.spawnPoint.y=spawn.y;
	p.respawn();
}

bool level::level_finished(playerObject& p){
	for(size_t i=0;i<goals.size(); ++i){
		if(goals[i].colliding(p))
			return true;
	}
	return false;
}
