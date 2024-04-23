#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

#include "gameObjects.h"
#include "level.h"

int main(int argc, char** argv){
	sf::RenderWindow window(sf::VideoMode(640,360),"Momentum");
	window.setKeyRepeatEnabled(false);
	sf::Clock clock;
	float timer=0.f;
	level cLevel;
	playerObject player;
	uint8_t input=0;
	bool building=false;
	sf::Keyboard::Key up=sf::Keyboard::Key::W;
	sf::Keyboard::Key down=sf::Keyboard::Key::S;
	sf::Keyboard::Key left=sf::Keyboard::Key::A;
	sf::Keyboard::Key right=sf::Keyboard::Key::D;
	sf::Keyboard::Key shift=sf::Keyboard::Key::LShift;
	sf::Keyboard::Key jump=sf::Keyboard::Key::Space;
	std::vector<aabb> toDraw;

	sf::Font arial;
	if(!arial.loadFromFile("fonts/arial.ttf")){
		std::cerr<<"unable to load font"<<std::endl;
		return 1;
	}
	sf::Text text;
	text.setFont(arial);
	text.setCharacterSize(15);
	text.setFillColor(sf::Color::Black);
	text.setPosition(5, 5);
	bool showSpeed=false;

	if(argc>1){
		if(!strcmp(argv[1], "--building"))
			building=true;
		else if(!strcmp(argv[1], "--speed")
			showSpeed=true;
		else{
			cLevel.load_level(argv[1], "none", player);
			cLevel.get_drawn(toDraw);
		}
	}else{
		cLevel.load_level("lv1", "none", player);
		cLevel.get_drawn(toDraw);
	}
	while(window.isOpen()){
		timer+=clock.restart().asSeconds();
		sf::Event event;
		while(window.pollEvent(event)){
			switch(event.type){
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					input|=event.key.code==up ? 1:0;
					input|=event.key.code==down ? 2:0;
					input|=event.key.code==left ? 4:0;
					input|=event.key.code==right ? 8:0;
					input|=event.key.code==shift ? 16:0;
					input|=event.key.code==jump ? 32:0;
					if(building&&event.key.code==sf::Keyboard::Key::L){
						cLevel.load_level("lvltest", "none", player);
						toDraw.clear();
						cLevel.get_drawn(toDraw);
					}
					break;
				case sf::Event::KeyReleased:
					input^=event.key.code==up ? 1:0;
					input^=event.key.code==down ? 2:0;
					input^=event.key.code==left ? 4:0;
					input^=event.key.code==right ? 8:0;
					break;
				default:
					break;
			}

		}
		if(timer>=0.015625f){
			timer=0;
			player.process_input(input);
			input&=207;
			player.update();
			if(cLevel.collide_player(player)){
				toDraw.clear();
				cLevel.get_drawn(toDraw);
			}
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
		sf::Vertex verticies[5]= {
			sf::Vertex(sf::Vector2f(player.topLeft.x, player.topLeft.y), player.color, sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(player.topLeft.x+player.size.x, player.topLeft.y), player.color, sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(player.topLeft.x+player.size.x, player.topLeft.y+player.size.y), player.color, sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(player.topLeft.x, player.topLeft.y+player.size.y), player.color, sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(player.topLeft.x, player.topLeft.y), player.color, sf::Vector2f(0.f, 0.f))
		};
		window.draw(verticies, 5, sf::LineStrip);
		if(showSpeed){
			text.setString(""+player.velocity.x + ", " +player.velocity.y);
			window.draw(text);
		}
		window.display();
	}
}
