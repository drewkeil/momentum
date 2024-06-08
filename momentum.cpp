#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>

#include "gameobjects.h"
#include "level.h"

class game{
public:
	game(): window(sf::VideoMode(640,360),"momentum"){
		window.setKeyRepeatEnabled(false);
		sf::Font font;
		if(!font.loadFromFile("fonts/OpenSans-Regular.ttf")){ // probably replace with different/custom font (also make sure i'm doing the distribution thing right)
			std::cerr<<"unable to load font"<<std::endl;
			exit(1);
		}
		text.setFont(font);
		text.setCharacterSize(15);
		text.setFillColor(sf::Color::Black);
		text.setPosition(5, 5);
		player.camera={{0,0,640,360},640,360,0,0,0};
		cLevel.load_level("test1", player); // I will add a main menu thingy soon
		cLevel.get_drawn(toDraw);
	}

	void run_game(){
		while(window.isOpen()){
			timer+=clock.restart().asSeconds();
			poll_events();
			if(timer>=0.015625f){
				timer=0;
				update();
			}
			render();
		}
	}

private:
	sf::RenderWindow window;
	sf::View view;
	sf::Clock clock;
	float timer=0.f;
	level cLevel;
	playerObject player;
	uint8_t input=0;
	bool jumpHeld=false;
	bool building=false;
	bool showSpeed=false;
	sf::Keyboard::Key up=sf::Keyboard::Key::W;
	sf::Keyboard::Key down=sf::Keyboard::Key::S;
	sf::Keyboard::Key left=sf::Keyboard::Key::A;
	sf::Keyboard::Key right=sf::Keyboard::Key::D;
	sf::Keyboard::Key shift=sf::Keyboard::Key::LShift;
	sf::Keyboard::Key jump=sf::Keyboard::Key::Space;
	std::vector<visibleObject> toDraw;	
	sf::Text text;

	void poll_events(){
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
						player.camera={{0,0,640,360},640,360,0,0,0};
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
	}

	void update(){
		player.process_input(input, jumpHeld);
		input&=207;
		player.update();
		if(cLevel.collide_player(player)){
			toDraw.clear();
			cLevel.get_drawn(toDraw);
			// set player camera to match default
		}
		player.update_camera(view);
	}

	void draw(const visibleObject& obj){
		aabb& rect=*obj.object;
		sf::Vertex verticies[5]= {
			sf::Vertex(sf::Vector2f(rect.topLeft.x, rect.topLeft.y), obj.color, sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(rect.topLeft.x+rect.size.x, rect.topLeft.y), obj.color, sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(rect.topLeft.x+rect.size.x, rect.topLeft.y+rect.size.y), obj.color, sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(rect.topLeft.x, rect.topLeft.y+rect.size.y), obj.color, sf::Vector2f(0.f, 0.f)),
			sf::Vertex(sf::Vector2f(rect.topLeft.x, rect.topLeft.y), obj.color, sf::Vector2f(0.f, 0.f))
		};
		window.draw(verticies, 5, sf::LineStrip);
	}

	void render(){
		window.setView(view);
		window.clear(sf::Color::White);
		for(visibleObject& obj:toDraw){
			draw(obj);
		}
		draw({&player, player.color});
		if(showSpeed){
			std::string str;
			player.show_velocity(str);
			text.setString(str);
			window.draw(text);
		}
		window.display();
	}
};

int main(int argc, char** argv){
	game momentum;
	momentum.run_game();
}
