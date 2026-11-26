#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"
#include "Global.hpp"
#include "Projectile.hpp"
#include "Vector2D.hpp"

class Render;
class CollisionManager;
class TileMap;

namespace sf {
   class RenderWindow;
}

class Player : public Entity{
public:
	Player();

	float speed;
	float hp;

	void input(const sf::RenderWindow& window);
	bool consumeShot(Vector2D& dir);
	void updateProjectiles(float dt, const CollisionManager& collisions,
	                      const TileMap& map);

	[[nodiscard]] int projectileCapacity() const noexcept;
	[[nodiscard]] const Projectile& projectileAt(int index) const;

private:
	void updateMouseInput(const sf::RenderWindow& window);
	[[nodiscard]] Vector2D keyboardDirection() const;
	void updateShotInput();
	void createProjectile(const Vector2D& direction);
	bool     spaceWasDown{false};   // estado da tecla no frame anterior
    bool     shotRequested{false};
    Vector2D facing{0.f, 1.f};      // última direção não nula (começa para baixo)
	Projectile projectiles[MAX_PLAYER_PROJECTILES];
	Vector2D targetPos{0.f, 0.f};
	bool isMovingToTarget = false;
};
#endif //PLAYER_HPP
