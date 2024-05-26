#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "gameObjects.h"
#include "level.h"

void render(sf::RenderWindow&, sf::Text&, playerObject&, std::vector<visibleObject>&, bool);

int main(int argc, char** argv){
	sf::RenderWindow window(sf::VideoMode(640,360),"Momentum");
	window.setKeyRepeatEnabled(false);
	sf::Clock clock;
	float timer=0.f;
	level cLevel;
	playerObject player;
	uint8_t input=0;
	bool jumpHeld=false;
	bool building=false;
	sf::Keyboard::Key up=sf::Keyboard::Key::W;
	sf::Keyboard::Key down=sf::Keyboard::Key::S;
	sf::Keyboard::Key left=sf::Keyboard::Key::A;
	sf::Keyboard::Key right=sf::Keyboard::Key::D;
	sf::Keyboard::Key shift=sf::Keyboard::Key::LShift;
	sf::Keyboard::Key jump=sf::Keyboard::Key::Space;
	std::vector<visibleObject> toDraw;

	sf::Font arial;
	if(!arial.loadFromFile("fonts/arial.ttf")){ // TODO: use a font I can actualy distribute
		std::cerr<<"unable to load font"<<std::endl;
		return 1;
	}
	sf::Text text;
	text.setFont(arial);
	text.setCharacterSize(15);
	text.setFillColor(sf::Color::Black);
	text.setPosition(5, 5);
	bool showSpeed=false;

	if(argc>1){ // should make a propper actual options processing thing eventualy
		if(argv[1]==std::string("--building"))
			building=true;
		else{
			if(argv[1]==std::string("--speed")){
				showSpeed=true;
				cLevel.load_level(argv[2], player);
			}else
				cLevel.load_level(argv[1], player);
			player.respawn();
			cLevel.get_drawn(toDraw);
		}
	}else{
		cLevel.load_level("lv1", player);
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
					jumpHeld=jumpHeld||event.key.code==jump;
					if(building&&event.key.code==sf::Keyboard::Key::L){
						cLevel.load_level("lvltest", player);
						toDraw.clear();
						cLevel.get_drawn(toDraw);
					}
					break;
				case sf::Event::KeyReleased:
					input^=event.key.code==up ? 1:0;
					input^=event.key.code==down ? 2:0;
					input^=event.key.code==left ? 4:0;
					input^=event.key.code==right ? 8:0;
					jumpHeld=!(event.key.code==jump);
					break;
				default:
					break;
			}

		}
		if(timer>=0.015625f){
			timer=0;
			player.process_input(input, jumpHeld);
			input&=207;
			player.update();
			if(cLevel.collide_player(player)){
				toDraw.clear();
				cLevel.get_drawn(toDraw);
			}
		}
		render(window, text, player, toDraw, showSpeed);
	}
}

void render(sf::RenderWindow& window, sf::Text& text, playerObject& player, std::vector<visibleObject>& toDraw, bool showSpeed){
	window.clear(sf::Color::White);
	for(visibleObject obj:toDraw){
		aabb rect=*obj.object;
			sf::Vertex verticies[5]= {
			sf::Vertex(sf::Vector2f(rect.topLeft.x, rect.topLeft.y), obj.color, sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(rect.topLeft.x+rect.size.x, rect.topLeft.y), obj.color, sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(rect.topLeft.x+rect.size.x, rect.topLeft.y+rect.size.y), obj.color, sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(rect.topLeft.x, rect.topLeft.y+rect.size.y), obj.color, sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(rect.topLeft.x, rect.topLeft.y), obj.color, sf::Vector2f(0.f, 0.f))
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
		std::string str;
		player.show_velocity(str);
		text.setString(str);
		window.draw(text);
	}
	window.display();
}
