#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>

#include "gameObjects.h"

struct build{
	std::vector<aabb> spikes;
	std::vector<aabb> platforms;
	std::vector<aabb> goals;
	std::vector<std::string> names;
	std::vector<vector2> spawns;
	std::vector<std::string> spawnNames;
	vector2 defaultSpawn;
	std::string defaultOps;
	float deathHeight;
};

void print_build(std::ostream& os, build& b){
	os<<"#death height\n";
	os<<b.deathHeight<<'\n';
	os<<"#name to spawn map\n"<<b.spawns.size()<<'\n';
	for(size_t i=0;i<b.spawns.size();++i)
		os<<b.spawnNames[i]<<' '<<b.spawns[i].x<<' '<<b.spawns[i].y<<' ';
	os<<b.defaultOps<<' '<<b.defaultSpawn.x<<' '<<b.defaultSpawn.y<<'\n';
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
	b.spawnNames.resize(s);
	b.spawns.resize(s);
	for(int i=0;i<s;++i){
		fin>>b.spawnNames[i];
		fin>>b.spawns[i].x>>b.spawns[i].y;
	}
	fin>>b.defaultOps>>b.defaultSpawn.x>>b.defaultSpawn.y;
	
	//section 3
	std::getline(fin, tmp);
	while(tmp[0]=='#'||tmp[0]=='\0')
		std::getline(fin, tmp);
	p=std::stoi(tmp);
	b.goals.resize(p);
	for(aabb& g:b.goals){
		fin>>g.topLeft.x>>g.topLeft.y>>g.size.x>>g.size.y;
		g.color=sf::Color::Green;
	}
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
	
	for(aabb& g:b.goals)
		drawn.push_back(g);
	for(size_t i=0;i<b.spawns.size();++i){
		aabb tmp(b.spawns[i].x, b.spawns[i].y, PLAYER_WIDTH, PLAYER_HEIGHT);
		tmp.color=sf::Color(200,200,255);
		drawn.push_back(tmp);
	}
	aabb player;
	player.color=sf::Color::Black;
	player.topLeft=b.defaultSpawn;
	player.size.x=PLAYER_WIDTH;
	player.size.y=PLAYER_HEIGHT;
	drawn.push_back(player);

	return drawn;
}

void static_add_drawn(build& b, std::vector<aabb>& drawn, float xOffset, float yOffset){
	for(aabb sp:b.spikes){
		sp.color= sf::Color(255, 200, 200);
		sp.topLeft.x+=xOffset;
		sp.topLeft.y+=yOffset;
		drawn.push_back(sp);
	}
	
	for(aabb pl:b.platforms){
		pl.color=sf::Color(200, 200, 200);
		pl.topLeft.x+=xOffset;
		pl.topLeft.y+=yOffset;
		drawn.push_back(pl);
	}
	
	for(aabb g:b.goals){
		g.topLeft.y+=yOffset;
		g.topLeft.x+=xOffset;
		g.color=sf::Color(200, 255, 200);
		drawn.push_back(g);
	}
}

enum Mode {none, box1, box2, spike, player, goal1, goal2, goal3, command, levelSpawn1, levelSpawn2};
std::string modes[11]={"none", "box1", "box2", "spike", "player", "goal1", "goal2", "goal3: ", "command: ","levelSpawn1", "levelSpawn2: "};

int main(int argc, char** argv){
	//  steps for test play:
	//		1. write level to predetermined file (levels/lvltest)
	//		2. use keypress in special momentum that re-reads level from correct file
	//		3. play epicly then leave open

	build b;
	b.deathHeight=400;
	b.defaultOps="n";
	Mode state=none;
	sf::RenderWindow window(sf::VideoMode(640,360),"Level Builder");
	window.setKeyRepeatEnabled(false);
	aabb placingObject;
	std::vector<aabb> toDraw;
	build staticNorth;
	build staticEast;
	build staticSouth;
	build staticWest;
	vector2 mousePos;
	std::string str;
	sf::Font arial;
	if(!arial.loadFromFile("fonts/arial.ttf")){
		std::cerr<<"unable to load font"<<std::endl;
		return 1;
	}
	sf::Text text;
	text.setFont(arial);
	text.setCharacterSize(15);
	text.setFillColor(sf::Color::Black);
	sf::View view=window.getDefaultView();
	if(argc>1){
		load_build(argv[1], b);
		toDraw=get_drawn(b);
	}
	// mousePressed, box, spike, player, delete spike, delete box, write, goal, command, enter, level spawn, delete level spawn
	bool pressed[12]={};
	bool boarder=true;
	bool labels=false;
	while(window.isOpen()){
		sf::Event event;
		for(int i=0;i<12;++i)
			pressed[i]=false;
		while(window.pollEvent(event)){
			switch(event.type){
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					pressed[1]=event.key.code==sf::Keyboard::Key::B;
					pressed[2]=event.key.code==sf::Keyboard::Key::S;
					pressed[3]=event.key.code==sf::Keyboard::Key::P;
					pressed[4]=event.key.code==sf::Keyboard::Key::D;
					pressed[5]=event.key.code==sf::Keyboard::Key::N;
					pressed[6]=event.key.code==sf::Keyboard::Key::W;
					pressed[7]=event.key.code==sf::Keyboard::Key::G;
					pressed[8]=event.key.code==sf::Keyboard::Key::Semicolon;
					pressed[9]=event.key.code==sf::Keyboard::Key::Enter;
					pressed[10]=event.key.code==sf::Keyboard::Key::L;
					pressed[11]=event.key.code==sf::Keyboard::Key::K;
					break;
				case sf::Event::MouseMoved:{
					sf::Vector2i mpos(event.mouseMove.x, event.mouseMove.y);
					mousePos.x=(int)window.mapPixelToCoords(mpos).x;
					mousePos.y=(int)window.mapPixelToCoords(mpos).y;
					break;
				}
				case sf::Event::MouseButtonPressed:
					pressed[0]=true;
					break;
				case sf::Event::TextEntered:
					if((state==command || state==goal3 || state==levelSpawn2) && !pressed[9]){
						if (event.text.unicode<128&&event.text.unicode!='\b')
							str+=(char)event.text.unicode;
						if(event.text.unicode=='\b'&&!str.empty())
							str.pop_back();
					}
					break;
				default:
					break;
			}

		}
		switch(state){
			case box1:
				if(pressed[0]){	
					placingObject.topLeft=mousePos;
					state=box2;
				}
				break;
			case box2:
				placingObject.size.x=mousePos.x-placingObject.topLeft.x;
				placingObject.size.y=mousePos.y-placingObject.topLeft.y;
				if(pressed[0]){
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
				}
				break;
			case goal1:
				if(pressed[0]){	
					placingObject.topLeft=mousePos;
					state=goal2;
				}
				break;
			case goal2:
				placingObject.size.x=mousePos.x-placingObject.topLeft.x;
				placingObject.size.y=mousePos.y-placingObject.topLeft.y;
				if(pressed[0]){
					state=goal3;
					if(placingObject.size.x<0){
						placingObject.topLeft.x+=placingObject.size.x;
						placingObject.size.x*=-1;
					}
					if(placingObject.size.y<0){
						placingObject.topLeft.y+=placingObject.size.y;
						placingObject.size.y*=-1;
					}
					b.goals.push_back(placingObject);
				}
				break;
			case goal3:
				if(pressed[9]){
					b.names.push_back(str);
					str.clear();
					state=none;
				}
				break;
			case spike:
				placingObject.topLeft=mousePos;
				if(pressed[0]){	
					placingObject.topLeft=mousePos;
					state=none;
					b.spikes.push_back(placingObject);
				}
				break;
			case player:
				placingObject.topLeft=mousePos;
				if(pressed[0]){	
					b.defaultSpawn=mousePos;
					state=none;
				}
				break;
			case levelSpawn1:
				placingObject.topLeft=mousePos;
				if(pressed[0]){
					b.spawns.push_back(mousePos);
					state=levelSpawn2;
				}
				break;
			case levelSpawn2:
				if(pressed[9]){
					b.spawnNames.push_back(str);
					str.clear();
					state=none;
				}
				break;
			case command:
				if(pressed[9]){
					std::string tmp;
					if(str.find(' ')==std::string::npos){
						state=none;
						str.clear();
						break;
					}
					tmp=str.substr(0,str.find(' '));
					str=str.substr(str.find(' ')+1);
					if(tmp=="zoom"){
						if(str=="default"){
							view=window.getDefaultView();
						}else{
							int amt=stoi(str);
							view.setSize(view.getSize().x+amt,view.getSize().y+amt);
						}
					}else if(tmp=="disp"){// do displaying
						if(str.find(' ')==std::string::npos){
							state=none;
							str.clear();
							break;
						}
						tmp=str.substr(0,str.find(' '));
						str=str.substr(str.find(' ')+1);
						if(tmp=="north")
							load_build(str,staticNorth);
						else if(tmp=="east")
							load_build(str,staticEast);
						else if(tmp=="south")
							load_build(str,staticSouth);
						else if(tmp=="west")
							load_build(str,staticWest);
					}else if(tmp=="boarder"){
						if(str=="off")
							boarder=false;
						else if(str=="on")
							boarder=true;
					}else if(tmp=="scroll"){
						if(str.find(' ')==std::string::npos){
							state=none;
							str.clear();
							break;
						}
						tmp=str.substr(0,str.find(' '));
						str=str.substr(str.find(' ')+1);
						if(tmp=="x")
							view.move(stoi(str), 0);
						else if(tmp=="y")
							view.move(0,stoi(str));
					}else if(tmp=="write"){
						std::ofstream fout;
						fout.open("levels/"+str);
						print_build(fout, b);
						fout.close();
					}else if(tmp=="defops"){
						b.defaultOps=str;
					}else if(tmp=="load"){
						load_build(str,b);
					}else if(tmp=="labels"){
						if(str=="on")
							labels=true;
						if(str=="off")
							labels=false;
					}
					str.clear();
					state=none;
				}
				break;
			case none:
				if(pressed[4]){
					if(!b.spikes.empty()){
						b.spikes.pop_back();
						toDraw=get_drawn(b);
					}
				}else if(pressed[5]){
					if(!b.platforms.empty()){
						b.platforms.pop_back();
						toDraw=get_drawn(b);
					}
				}else if(pressed[1]){
					placingObject.size.x=0;
					placingObject.size.y=0;
					placingObject.color=sf::Color::Black;
					state=box1;
				}else if(pressed[2]){
					placingObject.color=sf::Color::Red;
					placingObject.size.x=5;
					placingObject.size.y=5;
					state=spike;
				}else if(pressed[3]){
					placingObject.color=sf::Color::Black;
					placingObject.size.x=PLAYER_WIDTH;
					placingObject.size.y=PLAYER_HEIGHT;
					state=player;
				}else if(pressed[6]){
					std::ofstream fout;
					fout.open("levels/lvltest");
					print_build(fout, b);
					fout.close();
				}else if(pressed[7]){
					placingObject.size.x=0;
					placingObject.size.y=0;
					placingObject.color=sf::Color::Green;
					state=goal1;
				}else if(pressed[8]){
					state=command;
				}else if(pressed[11]&&!b.names.empty()){
					b.spawnNames.pop_back();
					b.spawns.pop_back();
				}else if(pressed[10]){
					placingObject.color=sf::Color(200, 200,255);
					placingObject.size.x=PLAYER_WIDTH;
					placingObject.size.y=PLAYER_HEIGHT;
					state=levelSpawn1;
				}
				break;
			default:
				state=none;
				break;
		}
		toDraw=get_drawn(b);
		static_add_drawn(staticNorth, toDraw, 0, -360);
		static_add_drawn(staticEast, toDraw, 640, 0);
		static_add_drawn(staticSouth, toDraw, 0, 360);
		static_add_drawn(staticWest, toDraw, -640, 0);
		window.clear(sf::Color::White);
		for(aabb& rect:toDraw){
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
		if(boarder){
			sf::Color color(180, 160, 70);
			sf::Vertex verticies[5]={
				sf::Vertex(sf::Vector2f(0, 0), color, sf::Vector2f(0,0)),
				sf::Vertex(sf::Vector2f(640, 0), color, sf::Vector2f(0,0)),
				sf::Vertex(sf::Vector2f(640, 360), color, sf::Vector2f(0,0)),
				sf::Vertex(sf::Vector2f(0, 360), color, sf::Vector2f(0,0)),
				sf::Vertex(sf::Vector2f(0, 0), color, sf::Vector2f(0,0))
			};
			window.draw(verticies, 5, sf::LineStrip);
		}
		if(labels){
			for(size_t i=0;i<b.names.size();++i){
				text.setString(b.names[i]);
				text.setPosition(b.goals[i].topLeft.x, b.goals[i].topLeft.y);
				window.draw(text);
			}
			for(size_t i=0;i<b.spawns.size();++i){
				text.setString(b.spawnNames[i]);
				text.setPosition(b.spawns[i].x, b.spawns[i].y);
				window.draw(text);
			}
		}
		window.setView(window.getDefaultView());
		text.setString(modes[state]+str);
		text.setPosition(5, 338);
		window.draw(text);

		window.display();
		window.setView(view);
	}
	return 0;
}
