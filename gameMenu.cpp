#include <algorithm>
#include <iostream>
#include <fstream>

#include "gameMenu.h"

bool gameMenu::update(const uint8_t& input, level& cLevel, playerObject& player, sf::Keyboard::Key keys[]){
	if(input&1)
		intdex=std::max(index-1,0);
	else if(input&2)
		index=std::min(index+1,(int)options.size()-1);
	if(input&32){
		switch(state){
			case menustate::main:
				switch(index){
					case 0:
						state=menustate::lvlselect;
						options={"back"};
						//TODO file reading stuff to get names of files
						// just have a file in levels/ that lists the name of every accessable level
						index=0;
						break;
					case 1:
						state=menustate::control;
						options=controls;
						index=0;
						break;
					default:
						break;
				}
				break;
			case menustate::control:
				switch(index){
					case 0:
						state=menustate::contUp;
						break;
					case 1:
						state=menustate::contDown;
						break;
					case 2:
						state=menustate::contLeft;
						break;
					case 3:
						state=menustate::contRight;
						break;
					case 4:
						state=menustate::contShift;
						break;
					case 5:
						state=menustate::contJump;
						break;
					case 6:
						state=menustate::main;
						options=mainmenu;
						index=0;
						break;
					default:
						break;
				}
				break;
			case menustate::lvlselect:
				if(index==0){
					state=menustate::main;
					options=mainmenu;
					index=0;
				}else{
					cLevel.load_level(options[index],player);
					return true;
				}
				break;
			default:
				break;
		}
}

void gameMenu::get_contents(std::vector<std::string>& text){
	options[index].insert(0,"->");
	text=options;
	options[index]=options[index].supstr(1);
}
