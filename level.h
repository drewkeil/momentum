#ifndef LEVEL_H
#define LEVEL_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "gameObjects.h"

class level{
public:

	//drawn should be an empty vector
	void get_drawn(std::vector<aabb>& drawn);

	void load_level(std::string name, std::string prevName);

	void ready_player(playerObject& p);

	void collide_player(playerObject& p);

	bool level_finished(playerObject& p);

private:
	std::vector<aabb> platforms;
	std::vector<aabb> spikes;
	std::vector<std::string> next;
	vector2 spawn;
	float deathHeight;
	std::vector<aabb> goals;
	std::string name;
};

#endif
