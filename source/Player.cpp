#include "Player.hpp"
#include "Render.hpp"
#include <SFML/Window.hpp>

Player::Player() : speed(DEFAULT_PLAYER_SPEED)
				 , hp(DEFAULT_PLAYER_HP){
					id = PLAYER_ID;
					collision.halfExtents = { TILE_SIZE * 0.35f, TILE_SIZE * 0.35f };
}

void Player::handleInput(Render& render, float dt)
{
	if(!render.getWindow().hasFocus()){
		body.velocity = Vector2D{0.f, 0.f};
		return;
	}

	Vector2D dir{0.f, 0.f};
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        dir.x -= 1.f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        dir.x += 1.f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        dir.y -= 1.f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        dir.y += 1.f;

	if(dir.x != 0.f || dir.y != 0.f)
		dir = dir.normalized();

	body.velocity = dir * speed;
	body.integrate(dt);
}