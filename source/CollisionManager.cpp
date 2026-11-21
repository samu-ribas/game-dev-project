#include "CollisionManager.hpp"
#include "TileMap.hpp"
#include <cassert>
#include <cmath>

namespace {
    /// True if the cell is a wall. Cells outside the map count as walls, which
    /// also keeps entities from leaving the world.
    bool isSolid(const TileMap& map, int col, int row) {
        if (col < 0 || col >= map.width() || row < 0 || row >= map.height()) {
            return true;
        }
        return map.isWall(col, row);
    }

    /// True if any tile covered by the box is solid.
    bool overlapsWall(const TileMap& map, const AABB& box) {
        int c0, r0, c1, r1;
        map.toIndices(box.min, c0, r0);
        map.toIndices(box.max, c1, r1);

        for (int r = r0; r <= r1; ++r) {
            for (int c = c0; c <= c1; ++c) {
                if (isSolid(map, c, r)) {
                    return true;
                }
            }
        }
        return false;
    }
}

CollisionManager::CollisionManager(float worldWidth, float worldHeight, float cellSize)
    : m_grid(worldWidth, worldHeight, cellSize) {}

bool CollisionManager::checkCollision(const Entity& a, const Entity& b) const
{
    return a.bounds().intersects(b.bounds());
}

void CollisionManager::rebuild(const std::vector<const Entity*>& entities)
{
    m_grid.clear();
    for (const Entity* e : entities) {
        if (e) {
            m_grid.insert(*e);
        }
    }
}

void CollisionManager::findInRadius(const Vector2D& center, float radius, std::vector<const Entity*>& out, const Entity* ignore) const
{
    m_grid.queryRadius(center, radius, out, ignore);
}

const Entity* CollisionManager::findNearest(const Vector2D& from, const Entity* ignore) const
{
    return m_grid.nearest(from, ignore);
}

void CollisionManager::findCollisions(const Entity& entity, std::vector<const Entity*>& out) const
{
    std::vector<const Entity*> candidates;
    m_grid.queryCandidates(entity.bounds(), candidates);

    out.clear();
    for (const Entity* other : candidates) {
        if (other != &entity && checkCollision(entity, *other)) {
            out.push_back(other);
        }
    }
}

void CollisionManager::moveAndSlide(Entity& entity, const TileMap& map, float dt) const
{
    RigidBody2D& body = entity.body;
    const Vector2D half = entity.collision.halfExtents;

    // Precondition: the entity starts the step OUTSIDE every wall. If this fails,
    // somebody moved the body before this call (e.g. integrate() called twice per
    // frame, once inside handleInput) or the entity spawned inside a wall.
    assert(!overlapsWall(map, entity.bounds()) && "entity starts inside a wall: body integrated twice per frame?");

    // 1) Integrate, remembering where we were and where the physics wants to go.
    const Vector2D from = body.position;
    body.integrate(dt);
    const Vector2D to = body.position;

    assert(std::abs(to.x - from.x) < TILE_SIZE && std::abs(to.y - from.y) < TILE_SIZE);

    // 2) X axis only. Y stays at its old value, which is known to be free.
    body.position = Vector2D{ to.x, from.y };
    if (to.x != from.x && overlapsWall(map, entity.bounds())) {
        const AABB box = entity.bounds();
        int col, row;

        if (to.x > from.x) {                       // moving right: wall is in the box's right column
            map.toIndices(box.max, col, row);
            body.position.x = static_cast<float>(col) * TILE_SIZE - half.x - COLLISION_SKIN;
        } else {                                   // moving left: wall is in the box's left column
            map.toIndices(box.min, col, row);
            body.position.x = static_cast<float>(col + 1) * TILE_SIZE + half.x + COLLISION_SKIN;
        }
        body.velocity.x = 0.0f;
    }

    // 3) Y axis, starting from the already corrected X.
    body.position.y = to.y;
    if (to.y != from.y && overlapsWall(map, entity.bounds())) {
        const AABB box = entity.bounds();
        int col, row;

        if (to.y > from.y) {                       // moving down: wall is in the box's bottom row
            map.toIndices(box.max, col, row);
            body.position.y = static_cast<float>(row) * TILE_SIZE - half.y - COLLISION_SKIN;
        } else {                                   // moving up: wall is in the box's top row
            map.toIndices(box.min, col, row);
            body.position.y = static_cast<float>(row + 1) * TILE_SIZE + half.y + COLLISION_SKIN;
        }
        body.velocity.y = 0.0f;
    }
}