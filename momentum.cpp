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

	void run_game(){	// this is why people make game engines
		while(window.isOpen()){
			timer+=clock.restart().asSeconds();
			poll_events();
			switch(state){
				case gamestate::menu:
					menu_update();
					menu_render();
					break;
				case gamestate::playing:
					if(timer>=0.015625f){
						timer=0;
						playing_update();
					}
					playing_render();
					break;
				case gamestate::paused:
					paused_update();
					paused_render();
					break;
			}
		}
	}

private:
	enum class:uint8_t gamestate {menu, playing, paused};

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
	sf::Keyboard::Key buttons[7]={sf::Keyboard::Key::W, sf::Keyboard::Key::S, sf::Keyboard::Key::A, sf::Keyboard::Key::D, sf::Keyboard::Key::LShift, sf::Keyboard::Key::Space, sf::Keyboard::key pressed};
	std::vector<visibleObject> toDraw;	
	sf::Text text;
	gamestate state=gamestate::menu;

	void poll_events(){
		sf::Event event;
		while(window.pollEvent(event)){
			switch(event.type){
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					input|=event.key.code==buttons[0] ? 1:0; //up
					input|=event.key.code==buttons[1] ? 2:0; //down
					input|=event.key.code==buttons[2] ? 4:0; //left
					input|=event.key.code==buttons[3] ? 8:0; //right
					input|=event.key.code==buttons[4] ? 16:0; //shift
					input|=event.key.code==buttons[5] ? 32:0; //jump
					jumpHeld=jumpHeld||event.key.code==buttons[5];
					if(building&&event.key.code==sf::Keyboard::Key::L){ //probably delete this
						cLevel.load_level("lvltest", player);
						toDraw.clear();
						cLevel.get_drawn(toDraw);
						player.camera={{0,0,640,360},640,360,0,0,0};
					}
					break;
				case sf::Event::KeyReleased:
					input^=event.key.code==buttons[0] ? 1:0;
					input^=event.key.code==buttons[1] ? 2:0;
					input^=event.key.code==buttons[2] ? 4:0;
					input^=event.key.code==buttons[3] ? 8:0;
					jumpHeld=!(event.key.code==buttons[5]);
					break;
				default:
					break;
			}
		}
	}

	void playing_update(){
		player.process_input(input, jumpHeld);
		input&=207;
		player.update();
		if(cLevel.collide_player(player)){
			toDraw.clear();
			cLevel.get_drawn(toDraw);
			player.camera={{0,0,640,360},640,360,0,0,0};
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

	void playing_render(){
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
