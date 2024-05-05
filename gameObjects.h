#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <cstdint>
#include <SFML/Graphics.hpp>
#include <string>

struct vector2{
	float x;
	float y;
};

class aabb{
public: 
	vector2 topLeft;
	vector2 size; // width, height

	aabb();

	aabb(float x1, float y1, float width, float height);

	bool colliding(aabb& other);
};

struct visibleObject{
	aabb* object;
	sf::Color color; // this will be replaced with sprite/texture stuff later
};

#define PLAYER_HEIGHT 20
#define PLAYER_WIDTH 10
class playerObject:public aabb{
	friend class level;
public:
	vector2 spawnPoint;
	sf::Color color;

	playerObject();

	void collide(aabb& other);

	// input is a bitmask representing which buttons are pressed
	// from lsb to msb: up, down, left, right, shift, jump
	void process_input(uint8_t input, bool jumpHeld);

	void respawn();
	
	void update();

	void show_velocity(std::string& str);

private:
	
	vector2 velocity;
	int coyote;
	int jumpBuffer;
	int shiftBuffer;
	bool grounded;
	int shifts;
	int jumpTimer;
};

#endif
