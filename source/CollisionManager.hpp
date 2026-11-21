#ifndef GAME_COLLISION_MANAGER
#define GAME_COLLISION_MANAGER

#include <vector>
#include "Entity.hpp"
#include "SpatialGrid.hpp"
#include "TileMap.hpp"
#include <cmath>

/**
 * @file CollisionManager.hpp
 * @brief Entity-vs-entity queries and collisions, accelerated by a SpatialGrid.
 *
 * Typical use, once per simulation step (after integrating the bodies):
 * @code
 *   collisions.rebuild(entities);
 *   collisions.findInRadius(player.body.position, DEFAULT_PLAYER_VISION, seen, &player);
 *   collisions.findCollisions(player, hits);
 * @endcode
 * Stored pointers are non-owning: rebuild() again whenever entities move.
 */
class CollisionManager
{
public:
    /**
     * @param worldWidth  World width in pixels.
     * @param worldHeight World height in pixels.
     * @param cellSize    Grid cell side in pixels (see SpatialGrid).
     */
    CollisionManager(float worldWidth, float worldHeight, float cellSize);
    ~CollisionManager() = default;

    /// Narrow phase: do the two entities' bounding boxes overlap?
    [[nodiscard]] bool checkCollision(const Entity& a, const Entity& b) const;

    /// Clears the grid and inserts every active entity of @p entities.
    void rebuild(const std::vector<const Entity*>& entities);

    /// Entities within @p radius of @p center (excluding @p ignore). Clears @p out first.
    void findInRadius(const Vector2D& center, float radius, std::vector<const Entity*>& out, const Entity* ignore = nullptr) const;

    /// Closest entity to @p from (excluding @p ignore), or null.
    [[nodiscard]] const Entity* findNearest(const Vector2D& from, const Entity* ignore = nullptr) const;

    /// Every entity colliding with @p entity (broad phase + narrow phase). Clears @p out first.
    void findCollisions(const Entity& entity, std::vector<const Entity*>& out) const;

    void moveAndSlide(Entity& entity, const TileMap& map, float dt) const;

private:
    SpatialGrid m_grid;
};

#endif //GAME_COLLISION_MANAGER
