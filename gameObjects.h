#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <cstdint>
#include <SFML/Graphics.hpp>

struct vector2{
	float x;
	float y;
};

class aabb{
public: 
	vector2 topLeft;
	vector2 size; // width, height
	sf::Color color;

	aabb();

	aabb(float x1, float y1, float width, float height);

	bool colliding(aabb& other);
};

class playerObject:public aabb{
public:
	vector2 spawnPoint;

	playerObject();

	void collide(aabb& other);

	// input is a bitmask representing which buttons are pressed
	// from lsb to msb: up, down, left, right, shift, jump
	void process_input(uint8_t input);

	void respawn();
	
	void update();

private:
	
	vector2 velocity;
	int coyote;
	int jumpBuffer;
	int shiftBuffer;
	bool grounded;
	int shifts;
};

#endif
