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
			if(event.type==sf::Event::Closed)
				window.close();
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
		window.display();
	}
}
