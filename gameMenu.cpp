#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "gameMenu.h"

gameMenu::gameMenu(){
	options.resize(2);
	options[0]=mainmenu[0];
	options[1]=mainmenu[1];
}

bool gameMenu::update(const uint8_t& input, level& cLevel, playerObject& player, sf::Keyboard::Key keys[]){
	if(state==menustate::change){
		if(keys[6]==sf::Keyboard::Key::Unknown)
			return false;
		keys[index-1]=keys[6];
		state=menustate::control;
		return false;
	}
	if(input&1)
		index=std::max(index-1,0);
	else if(input&2)
		index=std::min(index+1,(int)options.size()-1);
	if(input&32){
		switch(state){
			case menustate::main:
				switch(index){
					case 0:{
						state=menustate::lvlselect;
						std::ifstream fin;
						fin.open("levels/startpoints");
						if(!fin.is_open()){
							std::cerr<<"unable to open levels/startpoints"<<std::endl;
							exit(1);
						}
						options.clear();
						options.push_back("back");
						std::string tmp;
						while(fin>>tmp)
							options.push_back(tmp);
						fin.close();
						index=0;
						break;
					}case 1:
						state=menustate::control;
						options.resize(7);
						for(int i=0;i<7;++i)
							options[i]=controls[i];
						index=0;
						break;
					default:
						break;
				}
				break;
			case menustate::control:
				switch(index){
					case 0:
						state=menustate::main;
						options.resize(2);
						options[0]=mainmenu[0];
						options[1]=mainmenu[1];
						index=0;
						break;
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
						state=menustate::change;
						keys[6]=sf::Keyboard::Key::Unknown;
						break;
					default:
						break;
				}
				break;
			case menustate::lvlselect:
				if(index==0){
					state=menustate::main;
					options.resize(2);
					options[0]=mainmenu[0];
					options[1]=mainmenu[1];
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
	return false;
}

void gameMenu::get_contents(std::vector<std::string>& text, sf::Keyboard::Key keys[]){
	options[index].insert(0,"->");
	text=options;
	options[index]=options[index].substr(2);
	if(state==menustate::control||state==menustate::change){
		for(size_t i=1;i<text.size();++i)
			text[i]+=sf::Keyboard::getDescription(sf::Keyboard::delocalize(keys[i-1]));
		if(state==menustate::change)
			text[index]+=" [press any key to set control]";
	}
}
