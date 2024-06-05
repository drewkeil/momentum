#include "level.h"
#include <iostream>
#include <string>
#include <fstream>

void level::get_drawn(std::vector<visibleObject>& drawn){
	drawn.resize(platforms.size()+spikes.size());
	for(size_t i=0;i<platforms.size();++i){
		drawn[i].object=&platforms[i];
		drawn[i].color=sf::Color::Black;
	}
	size_t offset=platforms.size();
	for(size_t i=0;i<spikes.size();++i){
		drawn[i+offset].object=&spikes[i];
		drawn[i+offset].color=sf::Color::Red;
	}
}

// I should add versioning info to this so that I don't have to update all old level
// files whenever something new is added
void level::load_level(std::string lvlname, playerObject& pl){
	// clear stuff that might not be included in the file so it doesn't cause problems
	camTriggers.clear();
	camInfo.clear();

	std::ifstream fin;
	std::string tmp=lvlname;
	tmp.insert(0,"levels/");
	fin.open(tmp);
	if(!fin.is_open()){
		std::cerr<<"unable to open "<<tmp<<std::endl;
		exit(1);
	}

	//section 1
	std::getline(fin, tmp);
	while(tmp[0]=='#')
		std::getline(fin, tmp);
	deathHeight=std::stoi(tmp);
	
	//section 2
	std::getline(fin, tmp);
	while(tmp[0]=='#')
		std::getline(fin, tmp);
	int p, s;
	float temp;
	bool found=false;
	std::string ops;
	s=std::stoi(tmp);
	for(int i=0;i<s;++i){
		fin>>tmp;
		size_t opsLoc=tmp.find(':');
		if(opsLoc!=std::string::npos){
			ops=tmp.substr(opsLoc+1);
			tmp=tmp.substr(0,opsLoc);
		}
		if(tmp==name){
			fin>>pl.spawnPoint.x>>pl.spawnPoint.y;
			found=true;
		}else
			fin>>temp>>temp;
	}
	if(!found)
		fin>>ops>>pl.spawnPoint.x>>pl.spawnPoint.y;
	else
		fin>>tmp>>temp>>temp;
	vector2 plPos=pl.topLeft;
	pl.topLeft=pl.spawnPoint;
	for(char& ch:ops){
		switch(ch){
			case 'x':
				pl.topLeft.x=plPos.x;
				break;
			case 'y':
				pl.topLeft.y=plPos.y;
				break;
			case 's':
				pl.velocity.y=0;
				pl.velocity.x=0;
				break;
			default:
				break;
		}
	}			
	
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
	}
	spikes.resize(s);
	for(int i=0;i<s;++i){
		aabb& sp=spikes[i];
		fin>>sp.topLeft.x>>sp.topLeft.y;
		sp.size.x=5;
		sp.size.y=5;
	}

	//section 5
	std::getline(fin, tmp);
	while(tmp[0]=='#'||tmp[0]=='\0'){
		if(!std::getline(fin, tmp)){
			fin.close();
			name=lvlname;
			return;
		}
	}
	p=std::stoi(tmp);
	camTriggers.resize(p);
	camInfo.resize(p);
	for(int i=0;i<p;++i){
		aabb& trg=camTriggers[i];
		fin>>trg.topLeft.x>>trg.topLeft.y>>trg.size.x>>trg.size.y;
		camData& cam=camInfo[i];
		fin>>cam.bounds.topLeft.x>>cam.bounds.topLeft.y>>cam.bounds.size.x>>cam.bounds.size.y;
		fin>>cam.size.x>>cam.size.y>>cam.offset.x>>cam.offset.y>>cam.moveAmount;
	}
	fin.close();
	name=lvlname;
}

bool level::collide_player(playerObject& p){
	for(size_t i=0;i<goals.size();++i){
		if(goals[i].colliding(p)){
			load_level(next[i], p);
			return true;
		}
	}
	if(p.topLeft.y>deathHeight)
		p.respawn();
	for(aabb spike:spikes){
		if(spike.colliding(p)){
			p.respawn();
			return false;
		}
	}
	for(aabb platform:platforms){
		if(platform.colliding(p)){
			p.collide(platform);
		}
	}
	for(size_t i=0;i<camTriggers.size();++i){
		if(camTriggers[i].colliding(p))
			p.camera=camInfo[i];
	}
	return false;
}
