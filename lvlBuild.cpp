#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include <iostream>

#include "gameObjects.h"

struct build{
	std::vector<aabb> spikes;
	std::vector<aabb> platforms;
	vector2 spawn;
	float deathHeight;
	float goal;
};

void print_build(std::ostream& os, build& l){
	os<<l.goal<<' '<<l.deathHeight<<'\n';
	os<<l.spawn.x<<' '<<l.spawn.y<<'\n';
	os<<l.platforms.size()<<' '<<l.spikes.size()<<'\n';
	for(aabb& rect:l.platforms)
		os<<rect.topLeft.x<<' '<<rect.topLeft.y<<' '<<rect.size.x<<' '<<rect.size.y<<'\n';
	for(aabb& rect:l.spikes)
		os<<rect.topLeft.x<<' '<<rect.topLeft.y<<'\n';
}

void load_build(std::istream& is, build& l){
	int p,s;
	is>>l.goal>>l.deathHeight;
	is>>l.spawn.x>>l.spawn.y>>p>>s;
	l.platforms.resize(p);
	for(int i=0;i<p;++i){
		aabb& pl=l.platforms[i];
		is>>pl.topLeft.x>>pl.topLeft.y>>pl.size.x>>pl.size.y;
		pl.color=sf::Color::Black;
	}
	l.spikes.resize(s);
	for(int i=0;i<s;++i){
		aabb& sp=l.spikes[i];
		is>>sp.topLeft.x>>sp.topLeft.y;
		sp.size.x=5;
		sp.size.y=5;
		sp.color=sf::Color::Red;
	}
}

std::vector<aabb> get_drawn(build& b){
	std::vector<aabb> drawn;
	for(aabb& sp:b.spikes)
		drawn.push_back(sp);
	for(aabb& pl:b.platforms)
		drawn.push_back(pl);
	aabb player;
	player.color=sf::Color::Black;
	player.topLeft=b.spawn;
	player.size.x=10;
	player.size.y=25;
	drawn.push_back(player);
	return drawn;
}

enum Mode {none, box1, box2, spike, player};

int main(int argc, char** argv){
	//  steps for test play:
	//		1. write level to predetermined file (levels/testlvl)
	//		2. use keypress in special momentum that re-reads level from correct file
	//		3. play epicly then leave open

	build b;
	b.goal=640;
	b.deathHeight=400;
	Mode state=none;
	sf::RenderWindow window(sf::VideoMode(640,360),"Level Builder");
	window.setKeyRepeatEnabled(false);
	aabb placingObject;
	std::vector<aabb> toDraw;
	std::ifstream fin;
	vector2 mousePos;
	if(argc>1){
		fin.open(argv[1]);
		load_build(fin, b);
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
					b.spawn=mousePos;
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
					placingObject.size.x=10;
					placingObject.size.y=25;
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
