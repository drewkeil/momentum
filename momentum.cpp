#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <fstream>

#include "gameObjects.h"
#include "level.h"

int main(){
	sf::RenderWindow window(sf::VideoMode(640,360),"test");
	sf::Clock clock;
	float timer=0.f;
	level cLevel;
	playerObject player;
	uint8_t input=0;
	sf::Keyboard::Key up=sf::Keyboard::Key::W;
	sf::Keyboard::Key down=sf::Keyboard::Key::S;
	sf::Keyboard::Key left=sf::Keyboard::Key::A;
	sf::Keyboard::Key right=sf::Keyboard::Key::D;
	sf::Keyboard::Key shift=sf::Keyboard::Key::LShift;
	sf::Keyboard::Key jump=sf::Keyboard::Key::Space;
	std::vector<aabb> toDraw;
	std::ifstream fin;
	fin.open("levels/lv1.txt");
	if(!fin.is_open()){
		std::cerr<<"failed to load level"<<std::endl;
		return 1;
	}
	cLevel.load_level(fin);
	cLevel.ready_player(player);
	cLevel.get_drawn(toDraw);
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
					break;
				case sf::Event::KeyReleased:
					input^=event.key.code==up ? 1:0;
					input^=event.key.code==down ? 2:0;
					input^=event.key.code==left ? 4:0;
					input^=event.key.code==right ? 8:0;
					input^=event.key.code==shift ? 16:0;
					input^=event.key.code==jump ? 32:0;
					break;
				default:
					break;
			}

		}
		if(timer>=0.015625f){
			timer=0;
			player.process_input(input);
			player.update();
			cLevel.collide_player(player);
		}
		window.clear(sf::Color::White);
		for(aabb rect:toDraw){
			sf::RectangleShape shape(sf::Vector2f(rect.size.x, rect.size.y));
			shape.setPosition(rect.topLeft.x, rect.topLeft.y);
			shape.setFillColor(sf::Color::White);
			shape.setOutlineColor(sf::Color::Black);
			shape.setOutlineThickness(1.f);
			window.draw(shape);
		}
		sf::RectangleShape shape(sf::Vector2f(player.size.x, player.size.y));
		shape.setPosition(player.topLeft.x, player.topLeft.y);
		shape.setFillColor(sf::Color::White);
		shape.setOutlineColor(sf::Color::Black);
		shape.setOutlineThickness(1.f);
		window.draw(shape);
		window.display();
	}
}
