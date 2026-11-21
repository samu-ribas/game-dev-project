#include "SpatialGrid.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

SpatialGrid::SpatialGrid(float worldWidth, float worldHeight, float cellSize)
    : m_cellSize(cellSize)
{
    assert(worldWidth > 0.0f && worldHeight > 0.0f && cellSize > EPSILON);

    m_cols = std::max(1, static_cast<int>(std::ceil(worldWidth  / cellSize)));
    m_rows = std::max(1, static_cast<int>(std::ceil(worldHeight / cellSize)));
    m_cells.resize(static_cast<size_t>(m_cols) * static_cast<size_t>(m_rows));
}

int SpatialGrid::cellCol(float x) const noexcept {
    // Clamp in float BEFORE converting, so huge values never overflow the int.
    const float f = std::floor(x / m_cellSize);
    return static_cast<int>(std::clamp(f, 0.0f, static_cast<float>(m_cols - 1)));
}

int SpatialGrid::cellRow(float y) const noexcept {
    const float f = std::floor(y / m_cellSize);
    return static_cast<int>(std::clamp(f, 0.0f, static_cast<float>(m_rows - 1)));
}

void SpatialGrid::clear() noexcept {
    for (auto& cell : m_cells) {
        cell.clear();
    }
    m_maxHalfExtent = Vector2D{};
}

void SpatialGrid::insert(const Entity& entity) {
    if (!entity.active) {
        return;
    }

    const Vector2D& p = entity.body.position;
    const int col = cellCol(p.x);
    const int row = cellRow(p.y);
    m_cells[static_cast<size_t>(row) * m_cols + col].push_back(&entity);

    m_maxHalfExtent.x = std::max(m_maxHalfExtent.x, entity.collision.halfExtents.x);
    m_maxHalfExtent.y = std::max(m_maxHalfExtent.y, entity.collision.halfExtents.y);
}

// ---------------------------------------------------------------------------
// Fixed-radius search
// ---------------------------------------------------------------------------
void SpatialGrid::queryRadius(const Vector2D& center, float radius,
                              std::vector<const Entity*>& out,
                              const Entity* ignore) const
{
    out.clear();
    if (radius < 0.0f) {
        return;
    }

    // Block of cells that covers the circle.
    const int col0 = cellCol(center.x - radius);
    const int col1 = cellCol(center.x + radius);
    const int row0 = cellRow(center.y - radius);
    const int row1 = cellRow(center.y + radius);

    const float r2 = radius * radius;

    for (int row = row0; row <= row1; ++row) {
        for (int col = col0; col <= col1; ++col) {
            for (const Entity* e : m_cells[static_cast<size_t>(row) * m_cols + col]) {
                if (e == ignore) {
                    continue;
                }
                // The block is a bit bigger than the circle: exact test needed.
                const Vector2D d = e->body.position - center;
                if (d.length_squared() <= r2) {
                    out.push_back(e);
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Nearest neighbour (layer expansion + confirmation circle)
// ---------------------------------------------------------------------------
void SpatialGrid::scanCell(int col, int row, const Vector2D& from, const Entity* ignore,
                           const Entity*& best, float& bestD2) const
{
    if (!inGrid(col, row)) {
        return;
    }
    for (const Entity* e : m_cells[static_cast<size_t>(row) * m_cols + col]) {
        if (e == ignore) {
            continue;
        }
        const float d2 = (e->body.position - from).length_squared();
        if (d2 < bestD2) {
            bestD2 = d2;
            best = e;
        }
    }
}

void SpatialGrid::scanRing(int col0, int row0, int layer, const Vector2D& from,
                           const Entity* ignore, const Entity*& best, float& bestD2) const
{
    if (layer == 0) {
        scanCell(col0, row0, from, ignore, best, bestD2);
        return;
    }

    // Top and bottom sides (full width of the square).
    for (int c = col0 - layer; c <= col0 + layer; ++c) {
        scanCell(c, row0 - layer, from, ignore, best, bestD2);
        scanCell(c, row0 + layer, from, ignore, best, bestD2);
    }
    // Left and right sides (corners already done above).
    for (int r = row0 - layer + 1; r <= row0 + layer - 1; ++r) {
        scanCell(col0 - layer, r, from, ignore, best, bestD2);
        scanCell(col0 + layer, r, from, ignore, best, bestD2);
    }
}

const Entity* SpatialGrid::nearest(const Vector2D& from, const Entity* ignore) const {
    const int col0 = cellCol(from.x);
    const int row0 = cellRow(from.y);

    const Entity* best = nullptr;
    float bestD2 = std::numeric_limits<float>::max();

    // Beyond this layer the square lies entirely outside the grid.
    const int maxLayer = std::max({ col0, m_cols - 1 - col0, row0, m_rows - 1 - row0 });

    // Step 1: grow square layers until one contains a candidate.
    for (int layer = 0; layer <= maxLayer && !best; ++layer) {
        scanRing(col0, row0, layer, from, ignore, best, bestD2);
    }
    if (!best) {
        return nullptr;     // nothing (other than `ignore`) in the whole grid
    }

    // Step 2: the candidate is not necessarily the closest (diagonals). Use its
    // distance as the radius of a circle and scan every cell the circle touches.
    const float d = std::sqrt(bestD2);
    const int c0 = cellCol(from.x - d);
    const int c1 = cellCol(from.x + d);
    const int r0 = cellRow(from.y - d);
    const int r1 = cellRow(from.y + d);

    for (int r = r0; r <= r1; ++r) {
        for (int c = c0; c <= c1; ++c) {
            scanCell(c, r, from, ignore, best, bestD2);   // strict '<': duplicates are harmless
        }
    }
    return best;
}

// ---------------------------------------------------------------------------
// Broad phase for collisions
// ---------------------------------------------------------------------------
void SpatialGrid::queryCandidates(const AABB& box, std::vector<const Entity*>& out) const {
    out.clear();

    // Entities are indexed by centre, so grow the box by the largest half-extent.
    const int col0 = cellCol(box.min.x - m_maxHalfExtent.x);
    const int col1 = cellCol(box.max.x + m_maxHalfExtent.x);
    const int row0 = cellRow(box.min.y - m_maxHalfExtent.y);
    const int row1 = cellRow(box.max.y + m_maxHalfExtent.y);

    for (int row = row0; row <= row1; ++row) {
        for (int col = col0; col <= col1; ++col) {
            const auto& cell = m_cells[static_cast<size_t>(row) * m_cols + col];
            out.insert(out.end(), cell.begin(), cell.end());
        }
    }
}
