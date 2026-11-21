#ifndef GAME_SPATIALGRID_HPP
#define GAME_SPATIALGRID_HPP

#include <vector>
#include "Global.hpp"
#include "Vector2D.hpp"
#include "Collision2D.hpp"
#include "Entity.hpp"
#include <cassert>
/**
 * @file SpatialGrid.hpp
 * @brief Uniform-grid spatial partition for fast neighbour queries.
 *
 * The world (pixels, Y down, same convention as Global.hpp) is divided into
 * square cells of side `cellSize`. Each cell holds pointers to the entities
 * whose CENTRE (body.position) falls inside it:
 *
 *      col = floor(x / cellSize),  row = floor(y / cellSize)
 *
 * Cells are stored row-major: index = row * cols + col. Positions outside the
 * world are clamped into the border cells, so queries never read out of range.
 *
 * Lifetime: the grid stores non-owning pointers. Entities must outlive the
 * grid contents, so call clear()/rebuild every step before they move or die.
 * Inactive entities (Entity::active == false) are not inserted.
 */
class SpatialGrid {
public:
    /**
     * @param worldWidth  World width in pixels (> 0).
     * @param worldHeight World height in pixels (> 0).
     * @param cellSize    Side of a cell in pixels (> EPSILON). A value close to
     *                    the most common search radius works well.
     */
    SpatialGrid(float worldWidth, float worldHeight, float cellSize);

    /// Empties every cell (keeps the allocated memory).
    void clear() noexcept;

    /// Registers an active entity in the cell that contains its position.
    void insert(const Entity& entity);

    /**
     * @brief Fixed-radius search: entities whose centre is within @p radius.
     * @param[out] out    Cleared, then filled with the matches.
     * @param      ignore Entity to skip (e.g. the searcher itself), or null.
     * @details Visits only the block of cells that covers the circle
     *          [floor((x-r)/s) .. floor((x+r)/s)] x [floor((y-r)/s) .. floor((y+r)/s)]
     *          and applies the exact distance test inside it.
     */
    void queryRadius(const Vector2D& center, float radius, std::vector<const Entity*>& out, const Entity* ignore = nullptr) const;

    /**
     * @brief Nearest neighbour with no preset radius.
     * @return The closest entity to @p from, or null if there is none.
     * @details 1) Expands square layers of cells around @p from until one holds
     *          a candidate. 2) Uses the candidate's distance as the radius of a
     *          circle and scans every cell it touches, because a diagonal cell
     *          outside the square may hold something closer.
     */
    [[nodiscard]] const Entity* nearest(const Vector2D& from,const Entity* ignore = nullptr) const;

    /**
     * @brief Broad phase: entities whose bounds MAY overlap @p box.
     * @param[out] out Cleared, then filled with a superset of the real overlaps;
     *                 run AABB::intersects on each result (narrow phase).
     * @details The scanned area is @p box grown by the largest half-extent
     *          inserted so far, since entities are indexed by their centre.
     */
    void queryCandidates(const AABB& box, std::vector<const Entity*>& out) const;

    [[nodiscard]] int   cols() const noexcept     { return m_cols; }
    [[nodiscard]] int   rows() const noexcept     { return m_rows; }
    [[nodiscard]] float cellSize() const noexcept { return m_cellSize; }

private:
    int   m_cols{1};
    int   m_rows{1};
    float m_cellSize{TILE_SIZE};
    Vector2D m_maxHalfExtent{};                       ///< Largest half-extents inserted.
    std::vector<std::vector<const Entity*>> m_cells;  ///< Row-major, cols * rows lists.

    /// Column / row of a world coordinate, clamped to the grid.
    [[nodiscard]] int cellCol(float x) const noexcept;
    [[nodiscard]] int cellRow(float y) const noexcept;

    [[nodiscard]] bool inGrid(int col, int row) const noexcept {
        return col >= 0 && col < m_cols && row >= 0 && row < m_rows;
    }

    /// Updates best/bestD2 with the closest entity of one cell (ignores out-of-grid cells).
    void scanCell(int col, int row, const Vector2D& from, const Entity* ignore, const Entity*& best, float& bestD2) const;

    /// Scans only the border of the square layer at Chebyshev distance @p layer.
    void scanRing(int col0, int row0, int layer, const Vector2D& from, const Entity* ignore, const Entity*& best, float& bestD2) const;
};

#endif // GAME_SPATIALGRID_HPP
