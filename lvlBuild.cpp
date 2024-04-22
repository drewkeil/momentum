#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include <iostream>

#include "gameObjects.h"

struct build{
	std::vector<aabb> spikes;
	std::vector<aabb> platforms;
	std::vector<aabb> goals;
	std::vector<std::string> names;
	std::vector<vector2> spawns;
	std::string name;
	vector2 defaultSpawn;
	float deathHeight;
};

void print_build(std::ostream& os, build& b){
	os<<"#death height\n";
	os<<b.deathHeight<<'\n';
	os<<"#name to spawn map\n"<<b.spawns.size()<<'\n';
	for(size_t i=0;i<b.spawns.size();++i)
		os<<b.names[i]<<' '<<b.spawns[i].x<<' '<<b.spawns[i].y<<' ';
	os<<"n "<<b.defaultSpawn.x<<' '<<b.defaultSpawn.y<<'\n';
	os<<"#goals list\n"<<b.names.size()<<'\n';
	for(aabb& g:b.goals)
		os<<g.topLeft.x<<' '<<g.topLeft.y<<' '<<g.size.x<<' '<<g.size.y<<'\n';
	for(std::string& str:b.names)
		os<<str<<'\n';
	os<<"#platforms and spikes\n"<<b.platforms.size()<<'\n'<<b.spikes.size()<<'\n';
	for(aabb& rect:b.platforms)
		os<<rect.topLeft.x<<' '<<rect.topLeft.y<<' '<<rect.size.x<<' '<<rect.size.y<<'\n';
	for(aabb& rect:b.spikes)
		os<<rect.topLeft.x<<' '<<rect.topLeft.y<<'\n';
}

void load_build(std::string lvlname, build& b){
	std::ifstream fin;
	b.name=lvlname;
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
	b.deathHeight=std::stoi(tmp);
	
	//section 2
	std::getline(fin, tmp);
	while(tmp[0]=='#')
		std::getline(fin, tmp);
	int p, s;
	s=std::stoi(tmp);
	++s;
	b.names.resize(s);
	b.spawns.resize(s);
	for(int i=0;i<s;++i){
		fin>>b.names[i];
		fin>>b.spawns[i].x>>b.spawns[i].y;
	}
	
	//section 3
	std::getline(fin, tmp);
	while(tmp[0]=='#'||tmp[0]=='\0')
		std::getline(fin, tmp);
	p=std::stoi(tmp);
	b.goals.resize(p);
	for(aabb& g:b.goals)
		fin>>g.topLeft.x>>g.topLeft.y>>g.size.x>>g.size.y;
	b.names.resize(p);
	for(std::string& lvn:b.names)
		fin>>lvn;

	//section 4
	std::getline(fin, tmp);
	while(tmp[0]=='#'||tmp[0]=='\0')
		std::getline(fin, tmp);
	p=std::stoi(tmp);
	fin>>s;
	b.platforms.resize(p);
	for(int i=0;i<p;++i){
		aabb& pl=b.platforms[i];
		fin>>pl.topLeft.x>>pl.topLeft.y>>pl.size.x>>pl.size.y;
		pl.color=sf::Color::Black;
	}
	b.spikes.resize(s);
	for(int i=0;i<s;++i){
		aabb& sp=b.spikes[i];
		fin>>sp.topLeft.x>>sp.topLeft.y;
		sp.size.x=5;
		sp.size.y=5;
		sp.color=sf::Color::Red;
	}
	fin.close();
}

std::vector<aabb> get_drawn(build& b){
	std::vector<aabb> drawn;
	for(aabb& sp:b.spikes)
		drawn.push_back(sp);
	for(aabb& pl:b.platforms)
		drawn.push_back(pl);
	aabb player;
	player.color=sf::Color::Black;
	player.topLeft=b.defaultSpawn;
	player.size.x=PLAYER_WIDTH;
	player.size.y=PLAYER_HEIGHT;
	drawn.push_back(player);
	return drawn;
}

enum Mode {none, box1, box2, spike, player};

int main(int argc, char** argv){
	//  steps for test play:
	//		1. write level to predetermined file (levels/lvltest)
	//		2. use keypress in special momentum that re-reads level from correct file
	//		3. play epicly then leave open

	build b;
	b.deathHeight=400;
	Mode state=none;
	sf::RenderWindow window(sf::VideoMode(640,360),"Level Builder");
	window.setKeyRepeatEnabled(false);
	aabb placingObject;
	std::vector<aabb> toDraw;
	vector2 mousePos;
	if(argc>1){
		load_build(argv[1], b);
		toDraw=get_drawn(b);
	}
	bool mousePressed=false;
	bool box=false;
	bool spikep=false;
	bool playerp=false;
	bool delS=false;
	bool delB=false;
	bool write=false;
	while(window.isOpen()){
		sf::Event event;
		mousePressed=false;
		box=false;
		spikep=false;
		playerp=false;
		delS=false;
		delB=false;
		write=false;
		
		while(window.pollEvent(event)){
			switch(event.type){
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					box=event.key.code==sf::Keyboard::Key::B;
					spikep=event.key.code==sf::Keyboard::Key::S;
					playerp=event.key.code==sf::Keyboard::Key::P;
					delS=event.key.code==sf::Keyboard::Key::D;
					delB=event.key.code==sf::Keyboard::Key::N;
					write=event.key.code==sf::Keyboard::Key::W;
					break;
				case sf::Event::MouseMoved:{
					sf::Vector2i mpos(event.mouseMove.x, event.mouseMove.y);
					mousePos.x=window.mapPixelToCoords(mpos).x;
					mousePos.y=window.mapPixelToCoords(mpos).y;
					break;
				}
				case sf::Event::MouseButtonPressed:
					mousePressed=true;
					break;
				default:
					break;
			}

		}
		switch(state){
			case box1:
				if(mousePressed){	
					placingObject.topLeft=mousePos;
					placingObject.size.x=0;
					placingObject.size.y=0;
					state=box2;
				}
				break;
			case box2:
				placingObject.size.x=mousePos.x-placingObject.topLeft.x;
				placingObject.size.y=mousePos.y-placingObject.topLeft.y;
				if(mousePressed){
					state=none;
					if(placingObject.size.x<0){
						placingObject.topLeft.x+=placingObject.size.x;
						placingObject.size.x*=-1;
					}
					if(placingObject.size.y<0){
						placingObject.topLeft.y+=placingObject.size.y;
						placingObject.size.y*=-1;
					}
					b.platforms.push_back(placingObject);
					toDraw=get_drawn(b);
				}
				break;
			case spike:
				placingObject.topLeft=mousePos;
				if(mousePressed){	
					placingObject.topLeft=mousePos;
					state=none;
					b.spikes.push_back(placingObject);
					toDraw=get_drawn(b);
				}
				break;
			case player:
				placingObject.topLeft=mousePos;
				if(mousePressed){	
					b.defaultSpawn=mousePos;
					state=none;
					toDraw=get_drawn(b);
				}
				break;
			case none:
				if(delS){
					if(!b.spikes.empty()){
						b.spikes.pop_back();
						toDraw=get_drawn(b);
					}
				}else if(delB){
					if(!b.platforms.empty()){
						b.platforms.pop_back();
						toDraw=get_drawn(b);
					}
				}else if(box){
					placingObject.color=sf::Color::Black;
					state=box1;
				}else if(spikep){
					placingObject.color=sf::Color::Red;
					placingObject.size.x=5; // i think this is wrong
					placingObject.size.y=5;
					state=spike;
				}else if(playerp){
					placingObject.color=sf::Color::Black;
					placingObject.size.x=PLAYER_WIDTH;
					placingObject.size.y=PLAYER_HEIGHT;
					state=player;
				}else if(write){
					std::ofstream fout;
					fout.open("levels/lvltest");
					print_build(fout, b);
				}
			default:
				break;
		}
		window.clear(sf::Color::White);
		for(aabb rect:toDraw){
			sf::Vertex verticies[5]= {
				sf::Vertex(sf::Vector2f(rect.topLeft.x, rect.topLeft.y), rect.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(rect.topLeft.x+rect.size.x, rect.topLeft.y), rect.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(rect.topLeft.x+rect.size.x, rect.topLeft.y+rect.size.y), rect.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(rect.topLeft.x, rect.topLeft.y+rect.size.y), rect.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(rect.topLeft.x, rect.topLeft.y), rect.color, sf::Vector2f(0.f, 0.f))
			};
			window.draw(verticies, 5, sf::LineStrip);
		}
		if(!(state==none || state==box1)){
			sf::Vertex verticies[5]= {
				sf::Vertex(sf::Vector2f(placingObject.topLeft.x, placingObject.topLeft.y), placingObject.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(placingObject.topLeft.x+placingObject.size.x, placingObject.topLeft.y), placingObject.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(placingObject.topLeft.x+placingObject.size.x, placingObject.topLeft.y+placingObject.size.y), placingObject.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(placingObject.topLeft.x, placingObject.topLeft.y+placingObject.size.y), placingObject.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(placingObject.topLeft.x, placingObject.topLeft.y), placingObject.color, sf::Vector2f(0.f, 0.f))
			};
			window.draw(verticies, 5, sf::LineStrip);
		}
		window.display();
	}
	return 0;
}
