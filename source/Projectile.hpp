#ifndef GAME_PROJECTILE_HPP
#define GAME_PROJECTILE_HPP

#include "Entity.hpp"

/** A moving player projectile without collision handling yet. */
class Projectile : public Entity {
public:
    Projectile() noexcept;

    /** Activates the projectile with a position and direction. */
    void launch(const Vector2D& position, const Vector2D& direction) noexcept;

    /** Moves the projectile and expires it after its configured lifetime. */
    void update(float dt) noexcept;

    /// Disables the projectile after a collision or another terminal event.
    void deactivate() noexcept;

private:
    float m_lifetime{0.0f};
};

#endif // GAME_PROJECTILE_HPP
