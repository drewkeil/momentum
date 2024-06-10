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
	bool update(const uint8_t& input, level& cLevel, playerObject& player, sf::Keyboard::Key keys[]);

	void get_contents(std::vector<std::string>& text);

private:
	enum class menustate:uint8_t {main, control, contUp, contDown, contLeft, contRight, contShift, contJump, lvlselect};

	menustate state;
	int index; //this is signed because i am lazy and want to use std::max for bounds checking
	std::string mainmenu[]={"load level", "controls"};
	std::string controls[]={"up:", "down:", "left:", "right:", "shift:", "jump:", "back"};
	std::vector<std::string> options;
};

#endif
