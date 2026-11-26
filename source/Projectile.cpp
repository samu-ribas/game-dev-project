#include "Projectile.hpp"

Projectile::Projectile() noexcept
{
    active = false;
    collision.halfExtents = { BULLET_RADIUS, BULLET_RADIUS };
    body.acceleration = { 0.0f, 0.0f };
}

void Projectile::launch(const Vector2D& position, const Vector2D& direction) noexcept
{
    const float directionLengthSquared = direction.length_squared();
    if (directionLengthSquared <= EPSILON * EPSILON) {
        active = false;
        m_lifetime = 0.0f;
        body.velocity = { 0.0f, 0.0f };
        return;
    }

    const Vector2D normalizedDirection = direction / direction.length();
    body.position = position;
    body.velocity = normalizedDirection * BULLET_SPEED;
    body.acceleration = { 0.0f, 0.0f };
    m_lifetime = BULLET_LIFETIME;
    active = true;
}

void Projectile::update(float dt) noexcept
{
    if (!active || dt <= 0.0f) {
        return;
    }

    body.integrate(dt);
    m_lifetime -= dt;
    if (m_lifetime <= 0.0f) {
        deactivate();
    }
}

void Projectile::deactivate() noexcept
{
    active = false;
    m_lifetime = 0.0f;
    body.velocity = { 0.0f, 0.0f };
}
