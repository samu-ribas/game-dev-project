#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"
#include "Global.hpp"
#include "Vector2D.hpp"

class Render;

class Player : public Entity{
public:
	Player();

	float speed;
	float hp;
	
	void handleInput(Render& render, float dt);

};

#endif //PLAYER_HPP