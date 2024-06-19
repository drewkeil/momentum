#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>

#include "gameObjects.h"

struct dispObject{
	vector2 topLeft;
	vector2 size;
	sf::Color color;
};

struct build{
	std::vector<dispObject> spikes;
	std::vector<dispObject> platforms;
	std::vector<dispObject> goals;
	std::vector<std::string> names;
	std::vector<vector2> spawns;
	std::vector<std::string> spawnNames;
	std::vector<camData> cameras;
	std::vector<dispObject> camTriggers;
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
	for(dispObject& g:b.goals)
		os<<g.topLeft.x<<' '<<g.topLeft.y<<' '<<g.size.x<<' '<<g.size.y<<'\n';
	for(std::string& str:b.names)
		os<<str<<'\n';
	os<<"#platforms and spikes\n"<<b.platforms.size()<<'\n'<<b.spikes.size()<<'\n';
	for(dispObject& rect:b.platforms)
		os<<rect.topLeft.x<<' '<<rect.topLeft.y<<' '<<rect.size.x<<' '<<rect.size.y<<'\n';
	for(dispObject& rect:b.spikes)
		os<<rect.topLeft.x<<' '<<rect.topLeft.y<<'\n';
	os<<"#camera triggers and settings\n"<<b.cameras.size()<<'\n';
	for(size_t i=0;i<b.cameras.size();++i){
		dispObject& rect=b.camTriggers[i];
		os<<rect.topLeft.x<<' '<<rect.topLeft.y<<' '<<rect.size.x<<' '<<rect.size.y<<'\n';
		camData& cam=b.cameras[i];
		os<<cam.bounds.topLeft.x<<' '<<cam.bounds.topLeft.y<<' '<<cam.bounds.size.x<<' '<<cam.bounds.size.y<<'\n';
		os<<cam.size.x<<' '<<cam.size.y<<' '<<cam.offset.x<<' '<<cam.offset.y<<' '<<cam.moveAmount<<'\n';
	}
}

void load_build(std::string lvlname, build& b){
	std::ifstream fin;
	lvlname.insert(0,"levels/");
	fin.open(lvlname);
	if(!fin.is_open()){
		std::cerr<<"unable to open "<<lvlname<<std::endl;
		fin.close();
		return;
	}
	b.cameras.clear();
	b.camTriggers.clear();

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
	for(dispObject& g:b.goals){
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
		dispObject& pl=b.platforms[i];
		fin>>pl.topLeft.x>>pl.topLeft.y>>pl.size.x>>pl.size.y;
		pl.color=sf::Color::Black;
	}
	b.spikes.resize(s);
	for(int i=0;i<s;++i){
		dispObject& sp=b.spikes[i];
		fin>>sp.topLeft.x>>sp.topLeft.y;
		sp.size.x=5;
		sp.size.y=5;
		sp.color=sf::Color::Red;
	}

	// section 5
	std::getline(fin, tmp);
	while(tmp[0]=='#'||tmp[0]=='\0'){
		if(!std::getline(fin, tmp)){
			fin.close();
			return;
		}
	}
	p=std::stoi(tmp);
	b.camTriggers.resize(p);
	b.cameras.resize(p);
	for(int i=0;i<p;++i){
		dispObject& trg=b.camTriggers[i];
		fin>>trg.topLeft.x>>trg.topLeft.y>>trg.size.x>>trg.size.y;
		trg.color=sf::Color(210, 190, 100);
		camData& cam=b.cameras[i];
		fin>>cam.bounds.topLeft.x>>cam.bounds.topLeft.y>>cam.bounds.size.x>>cam.bounds.size.y;
		fin>>cam.size.x>>cam.size.y>>cam.offset.x>>cam.offset.y>>cam.moveAmount;
	}
	fin.close();
}

std::vector<dispObject> get_drawn(build& b){
	std::vector<dispObject> drawn;
	for(dispObject& sp:b.spikes)
		drawn.push_back(sp);
	
	for(dispObject& pl:b.platforms)
		drawn.push_back(pl);
	
	for(dispObject& g:b.goals)
		drawn.push_back(g);
	for(size_t i=0;i<b.spawns.size();++i){
		dispObject tmp{{b.spawns[i].x, b.spawns[i].y}, {PLAYER_WIDTH, PLAYER_HEIGHT}, sf::Color(200, 200, 255)};
		drawn.push_back(tmp);
	}
	for(size_t i=0;i<b.cameras.size();++i){
		drawn.push_back({b.cameras[i].bounds.topLeft, b.cameras[i].bounds.size, sf::Color(180, 160, 70)});
		drawn.push_back(b.camTriggers[i]);
	}
	dispObject player;
	player.color=sf::Color::Black;
	player.topLeft=b.defaultSpawn;
	player.size.x=PLAYER_WIDTH;
	player.size.y=PLAYER_HEIGHT;
	drawn.push_back(player);

	return drawn;
}

void static_add_drawn(build& b, std::vector<dispObject>& drawn, float xOffset, float yOffset){
	for(dispObject sp:b.spikes){
		sp.color= sf::Color(255, 200, 200);
		sp.topLeft.x+=xOffset;
		sp.topLeft.y+=yOffset;
		drawn.push_back(sp);
	}
	
	for(dispObject pl:b.platforms){
		pl.color=sf::Color(200, 200, 200);
		pl.topLeft.x+=xOffset;
		pl.topLeft.y+=yOffset;
		drawn.push_back(pl);
	}
	
	for(dispObject g:b.goals){
		g.topLeft.y+=yOffset;
		g.topLeft.x+=xOffset;
		g.color=sf::Color(200, 255, 200);
		drawn.push_back(g);
	}
}

enum Mode {none, box1, box2, spike, player, goal1, goal2, goal3, command, levelSpawn1, levelSpawn2, del, camtrig1, camtrig2,
	cambound1=14, cambound2=15, camwidth=16, camheight=17, camxoffset=18, camyoffset=19, camspeed=20};
std::string modes[]={"none", "box1", "box2", "spike", "player", "goal1", "goal2", "goal destination: ", "command: ", "levelSpawn1", "spawn entry level: ", "delete", "camtrig1", "camtrig2",
	"cambound1", "cambound2", "cam width: ", "cam height: ", "cam x offset: ", "cam y offset: ", "cam follow rate [0-1]:"};

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
	dispObject placingObject;
	std::vector<dispObject> toDraw;
	build staticNorth;
	build staticEast;
	build staticSouth;
	build staticWest;
	vector2 mousePos;
	std::string str;
	sf::Font font;
	if(!font.loadFromFile("fonts/OpenSans-Regular.ttf")){
		std::cerr<<"unable to load font"<<std::endl;
		return 1;
	}
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(15);
	text.setFillColor(sf::Color::Black);
	sf::View view=window.getDefaultView();
	if(argc>1){
		load_build(argv[1], b);
		toDraw=get_drawn(b);
	}
	// mousePressed, box, spike, player, delete, write, goal, command, enter, level spawn, escape, camera
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
					pressed[5]=event.key.code==sf::Keyboard::Key::W;
					pressed[6]=event.key.code==sf::Keyboard::Key::G;
					pressed[7]=event.key.code==sf::Keyboard::Key::Semicolon;
					pressed[8]=event.key.code==sf::Keyboard::Key::Enter;
					pressed[9]=event.key.code==sf::Keyboard::Key::L;
					pressed[10]=event.key.code==sf::Keyboard::Key::Escape;
					pressed[11]=event.key.code==sf::Keyboard::Key::C;
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
					if((state==command || state==goal3 || state==levelSpawn2 || (state>15 && state<21)) && !pressed[8]){
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
		if(pressed[10]){
			state=none;
			str.clear();
			if(state>13&&state<21){
				b.cameras.pop_back();
				b.camTriggers.pop_back();
			}
		}
		switch(state){
			case box1:
				placingObject.topLeft=mousePos;
				if(pressed[0])
					state=box2;
				break;
			case box2:
				placingObject.size.x=mousePos.x-placingObject.topLeft.x;
				placingObject.size.y=mousePos.y-placingObject.topLeft.y;
				if(pressed[0]){
					state=box1;
					if(placingObject.size.x<0){
						placingObject.topLeft.x+=placingObject.size.x;
						placingObject.size.x*=-1;
					}
					if(placingObject.size.y<0){
						placingObject.topLeft.y+=placingObject.size.y;
						placingObject.size.y*=-1;
					}
					b.platforms.push_back(placingObject);
					placingObject.size.x=15;
					placingObject.size.y=5;
				}
				break;
			case goal1:
				placingObject.topLeft=mousePos;
				if(pressed[0])
					state=goal2;
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
				}
				break;
			case goal3:
				if(pressed[8]){
					b.names.push_back(str);
					b.goals.push_back(placingObject);
					str.clear();
					state=goal1;
					placingObject.size={15, 5};
				}
				break;
			case spike:
				placingObject.topLeft=mousePos;
				if(pressed[0]){	
					placingObject.topLeft=mousePos;
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
					state=levelSpawn2;
				}
				break;
			case levelSpawn2:
				if(pressed[8]){
					b.spawnNames.push_back(str);
					b.spawns.push_back(placingObject.topLeft);
					str.clear();
					state=levelSpawn1;
				}
				break;
			case command:
				if(pressed[8]){
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
					}else if(tmp=="start"){
						std::ofstream fout;
						fout.open("levels/startpoints", std::ios_base::app);
						fout<<str<<'\n';
						fout.close();
					}
					str.clear();
					state=none;
				}
				break;
			case del:{
				std::vector<dispObject> *stuff[3]={&b.spikes, &b.platforms, &b.goals};
				uint32_t minDist=0xFFFFFFFF;
				size_t minType, minIdx;
				for(size_t i=0;i<3;++i){
					for(size_t j=0;j<stuff[i]->size();++j){
						const vector2& point={stuff[i]->at(j).topLeft.x+stuff[i]->at(j).size.x/2, stuff[i]->at(j).topLeft.y+stuff[i]->at(j).size.y/2};
						uint32_t dist=((point.y-mousePos.y)*(point.y-mousePos.y)+(point.x-mousePos.x)*(point.x-mousePos.x));
						if(dist<minDist){
							minDist=dist;
							minIdx=j;
							minType=i;
						}
					}
				}
				for(size_t i=0;i<b.spawns.size();++i){
					const vector2& point={b.spawns[i].x+PLAYER_WIDTH/2, b.spawns[i].y+PLAYER_HEIGHT/2};
					uint32_t dist=((point.y-mousePos.y)*(point.y-mousePos.y)+(point.x-mousePos.x)*(point.x-mousePos.x));
					if(dist<minDist){
						minDist=dist;
						minIdx=i;
						minType=0xFFFFFFFF;
					}
				}
				if(minDist!=0xFFFFFFFF){
					if(minType==0xFFFFFFFF){
						if(pressed[0]){
							b.spawns.erase(b.spawns.begin()+minIdx);
							b.names.erase(b.spawnNames.begin()+minIdx);
						}else
							placingObject={b.spawns[minIdx], {PLAYER_WIDTH, PLAYER_HEIGHT}, sf::Color(200,200,255)};
					}else{
						if(pressed[0]){
							stuff[minType]->erase(stuff[minType]->begin()+minIdx);
							if(minType==2)
								b.names.erase(b.names.begin()+minIdx);
						}else
							placingObject=stuff[minType]->at(minIdx);
					}
				}
				break;
			}
			case camtrig1:
				placingObject.topLeft=mousePos;
				if(pressed[0])
					state=camtrig2;
				break;
			case camtrig2:
				placingObject.size.x=mousePos.x-placingObject.topLeft.x;
				placingObject.size.y=mousePos.y-placingObject.topLeft.y;
				if(pressed[0]){
					state=cambound1;
					if(placingObject.size.x<0){
						placingObject.topLeft.x+=placingObject.size.x;
						placingObject.size.x*=-1;
					}
					if(placingObject.size.y<0){
						placingObject.topLeft.y+=placingObject.size.y;
						placingObject.size.y*=-1;
					}
					b.camTriggers.push_back(placingObject);
					b.cameras.push_back({{0, 0, 0, 0}, {0,0},{0,0},0}); // to make exiting early work
					placingObject.size={15, 5};
					placingObject.color=sf::Color(180, 160, 70);
				}
				break;
			case cambound1:
				placingObject.topLeft=mousePos;
				if(pressed[0])
					state=cambound2;
				break;
			case cambound2:
				placingObject.size.x=mousePos.x-placingObject.topLeft.x;
				placingObject.size.y=mousePos.y-placingObject.topLeft.y;
				if(pressed[0]){
					state=camwidth;
					if(placingObject.size.x<0){
						placingObject.topLeft.x+=placingObject.size.x;
						placingObject.size.x*=-1;
					}
					if(placingObject.size.y<0){
						placingObject.topLeft.y+=placingObject.size.y;
						placingObject.size.y*=-1;
					}
					b.cameras.back().bounds=aabb(placingObject.topLeft.x, placingObject.topLeft.y, placingObject.size.x, placingObject.size.y);
				}
				break;
			case camwidth:
				if(pressed[8]){
					b.cameras.back().size.x=stoi(str);
					str.clear();
					state=camheight;
				}
				break;
			case camheight:
				if(pressed[8]){
					b.cameras.back().size.y=stoi(str);
					str.clear();
					state=camxoffset;
				}
				break;
			case camxoffset:
				if(pressed[8]){
					b.cameras.back().offset.x=stoi(str);
					str.clear();
					state=camyoffset;
				}
				break;
			case camyoffset:
				if(pressed[8]){
					b.cameras.back().offset.y=stoi(str);
					str.clear();
					state=camspeed;
				}
				break;
			case camspeed:
				if(pressed[8]){
					b.cameras.back().moveAmount=std::stof(str);
					str.clear();
					state=camtrig1;
				}
				break;
			case none:
				placingObject={{0,0},{0,0},sf::Color::Black};
				if(pressed[1]){
					placingObject.size.x=15;
					placingObject.size.y=5;
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
				}else if(pressed[5]){
					std::ofstream fout;
					fout.open("levels/lvltest");
					print_build(fout, b);
					fout.close();
				}else if(pressed[6]){
					placingObject.size.x=15;
					placingObject.size.y=5;
					placingObject.color=sf::Color::Green;
					state=goal1;
				}else if(pressed[7]){
					state=command;
				}else if(pressed[9]){
					placingObject.color=sf::Color(200, 200, 255);
					placingObject.size.x=PLAYER_WIDTH;
					placingObject.size.y=PLAYER_HEIGHT;
					state=levelSpawn1;
				}else if(pressed[4]){
					state=del;
				}else if(pressed[11]){
					state=camtrig1;
					placingObject.size.x=15;
					placingObject.size.y=5;
					placingObject.color=sf::Color(210, 190, 100);
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
		for(size_t i=toDraw.size();i>0;--i){
			dispObject& rect=toDraw[i-1];
			sf::Vertex verticies[5]= {
				sf::Vertex(sf::Vector2f(rect.topLeft.x, rect.topLeft.y), rect.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(rect.topLeft.x+rect.size.x, rect.topLeft.y), rect.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(rect.topLeft.x+rect.size.x, rect.topLeft.y+rect.size.y), rect.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(rect.topLeft.x, rect.topLeft.y+rect.size.y), rect.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(rect.topLeft.x, rect.topLeft.y), rect.color, sf::Vector2f(0.f, 0.f))
			};
			window.draw(verticies, 5, sf::LineStrip);
		}
		if(state!=del){
			sf::Vertex verticies[5]= {
				sf::Vertex(sf::Vector2f(placingObject.topLeft.x, placingObject.topLeft.y), placingObject.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(placingObject.topLeft.x+placingObject.size.x, placingObject.topLeft.y), placingObject.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(placingObject.topLeft.x+placingObject.size.x, placingObject.topLeft.y+placingObject.size.y), placingObject.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(placingObject.topLeft.x, placingObject.topLeft.y+placingObject.size.y), placingObject.color, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(placingObject.topLeft.x, placingObject.topLeft.y), placingObject.color, sf::Vector2f(0.f, 0.f))
			};
			window.draw(verticies, 5, sf::LineStrip);
		}else{
			sf::Vertex verticies[2]={
				sf::Vertex(sf::Vector2f(placingObject.topLeft.x-5, placingObject.topLeft.y-5), sf::Color::Red, sf::Vector2f(0.f, 0.f)),
				sf::Vertex(sf::Vector2f(placingObject.topLeft.x+placingObject.size.x+5, placingObject.topLeft.y+placingObject.size.y+5), sf::Color::Red, sf::Vector2f(0.f, 0.f))
			};
			window.draw(verticies, 2, sf::LineStrip);
			verticies[0]=sf::Vertex(sf::Vector2f(placingObject.topLeft.x+placingObject.size.x+5, placingObject.topLeft.y-5), sf::Color::Red, sf::Vector2f(0.f, 0.f));
			verticies[1]=sf::Vertex(sf::Vector2f(placingObject.topLeft.x-5, placingObject.topLeft.y+placingObject.size.y+5), sf::Color::Red, sf::Vector2f(0.f, 0.f));
			window.draw(verticies, 2, sf::LineStrip);
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
