#include "Player.hpp"
#include "Render.hpp"
#include "CollisionManager.hpp"
#include <SFML/Window.hpp>
#include <cassert>

Player::Player() : speed(DEFAULT_PLAYER_SPEED)
				 , hp(DEFAULT_PLAYER_HP){
					id = PLAYER_ID;
					collision.halfExtents = { TILE_SIZE * 0.35f, TILE_SIZE * 0.35f };
}

void Player::input(const sf::RenderWindow& window)
{
    // 1. Detecta o clique esquerdo do mouse para definir o destino
    updateMouseInput(window);

    // 2. Calcula a direção até o destino clicado
    Vector2D dir = keyboardDirection();
    if (dir.x != 0.0f || dir.y != 0.0f) {
        isMovingToTarget = false;
        facing = dir;
    }

    if (isMovingToTarget)
    {
        // Substitua 'body.position' pelo nome exato da posição no seu struct body
        Vector2D toTarget = targetPos - body.position;
        float distance = toTarget.length();

        // Margem de parada (5 pixels) para o boneco não ficar tremendo no destino
        if (distance > 5.f) {
            dir = toTarget.normalized();
            facing = dir;
        }else{
            // Chegou ao destino
            body.position = targetPos;
            isMovingToTarget = false;
        }
    }

    // 3. Aplica a velocidade no seu sistema de física
    body.velocity = dir * speed;

    // 4. Lógica de tiro mantida intacta
    updateShotInput();
}

void Player::updateMouseInput(const sf::RenderWindow& window)
{
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        return;
    }

    const sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    const sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    targetPos = Vector2D{ worldPos.x, worldPos.y };
    isMovingToTarget = true;
}

Vector2D Player::keyboardDirection() const
{
    Vector2D dir{ 0.0f, 0.0f };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        dir.x -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        dir.x += 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        dir.y -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        dir.y += 1.0f;
    }

    if (dir.x != 0.0f || dir.y != 0.0f) {
        dir.normalize();
    }
    return dir;
}

void Player::updateShotInput()
{
    const bool spaceDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
    if (spaceDown && !spaceWasDown) {
        shotRequested = true;
    }
    spaceWasDown = spaceDown;
}

bool Player::consumeShot(Vector2D& dir)
{
	if(!shotRequested)
		return false;
	shotRequested = false;
	dir = facing;
	return true;
}

void Player::updateProjectiles(float dt, const CollisionManager& collisions,
                               const TileMap& map)
{
	if (dt <= 0.0f) {
		return;
	}

	for (int i = 0; i < MAX_PLAYER_PROJECTILES; ++i) {
		collisions.updateProjectile(projectiles[i], map, dt, this);
	}

	Vector2D shotDirection;
	if (consumeShot(shotDirection)) {
		createProjectile(shotDirection);
	}
}

int Player::projectileCapacity() const noexcept
{
	return MAX_PLAYER_PROJECTILES;
}

const Projectile& Player::projectileAt(int index) const
{
	assert(index >= 0 && index < MAX_PLAYER_PROJECTILES);
	return projectiles[index];
}

void Player::createProjectile(const Vector2D& direction)
{
	const Vector2D spawnPosition = body.position + direction * (collision.halfExtents.x + BULLET_RADIUS + EPSILON);

	for (int i = 0; i < MAX_PLAYER_PROJECTILES; ++i) {
		if (!projectiles[i].active) {
			projectiles[i].launch(spawnPosition, direction);
			return;
		}
	}
}
