#include <SFML/Graphics.hpp>
#include <iostream>

int main(){
	sf::RenderWindow window(sf::VideoMode(1000,1000),"test");
	sf::CircleShape shape(50.f);
	shape.setFillColor(sf::Color::Red);
	sf::Clock clock;
	while(window.isOpen()){
		std::cout<<clock.restart().asSeconds()<<std::endl;
		sf::Event event;
		while(window.pollEvent(event)){
			if(event.type==sf::Event::Closed)
				window.close();
		}
		window.clear();
		window.draw(shape);
		window.display();
	}
}
