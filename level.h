#ifndef LEVEL_H
#define LEVEL_H

#include <iostream>
#include <vector>

#include "gameObjects.h"

class level{
public:
	//drawn should be an empty vector
	void get_drawn(std::vector<aabb>& drawn);

	void load_level(std::istream& is);

	void ready_player(playerObject& p);

	void collide_player(playerObject& p);

private:
	std::vector<aabb> platforms;
	std::vector<aabb> spikes;
	vector2 spawn;
};

#endif
