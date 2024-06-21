#ifndef GAMEMENU_H
#define GAMEMENU_H


#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cstdint>

#include "level.h"
#include "gameObjects.h"

class gameMenu{
public:
	
	gameMenu();

	bool update(const uint8_t& input, level& cLevel, playerObject& player, sf::Keyboard::Key keys[]);

	void get_contents(std::vector<std::string>& text, sf::Keyboard::Key keys[]);

private:
	enum class menustate:uint8_t {main, control, change, lvlselect};

	menustate state=menustate::main;
	int index=0; //this is signed because i am lazy
	std::string mainmenu[2]={"load level", "controls"};
	std::string controls[7]={"back", "up: ", "down: ", "left: ", "right: ", "shift: ", "jump: "};
	std::vector<std::string> options;
};

#endif
