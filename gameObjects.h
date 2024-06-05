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

struct camData{
	aabb bounds;
	vector2 size; // width, height
	vector2 offset; // from center of player
	float moveAmount; // fraction of distance to player per frame
	//float zoomSpeed; // speed at which to zoom in/out if needed (not implemented)
};

#define PLAYER_HEIGHT 20
#define PLAYER_WIDTH 10
class playerObject:public aabb{
	friend class level;
public:
	vector2 spawnPoint;
	sf::Color color;
	camData camera;

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

//Unsure if I actualy want these in the game. I'd rather have
// stuff that is fun in this game and not just add every default
// platformer feature just because if it doesent have interesting 
// interactions with the main mechanic.
//I'll have to play around with the ones implemented in the
// origional java version to see how fun/necessary moving
// platforms are for this game and how well they interact
// with the main mechanic.
class movingPlatform:public aabb{
	public:
	movingPlatform(std::istream& is);

	void move();

	private:
	struct moveInfo{
		vector2 position;
		float maxSpeed;
		float acceleration;
		uint32_t frameDelay;
	};
	std::vector<moveInfo> path;
	uint32_t delay;
	uint32_t moveIdx;
	float velocity;
	bool started; // need some way to set and check start moving conditions
};

#endif
