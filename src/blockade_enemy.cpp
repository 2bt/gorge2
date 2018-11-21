#include "blockade_enemy.hpp"

namespace {
    constexpr std::array<vec2, 4> BLOCKADE_ENEMY_POLYGON = {
		vec2{4, 3},
		vec2{4, -3},
		vec2{-4, -3},
		vec2{-4, 3},
    };
}

BlockadeEnemy::BlockadeEnemy(World& world, uint32_t seed, vec2 const& pos) : Enemy(world, seed, pos) {
    m_sprite = Sprite::BLOCKADE;
    m_shield = 20;
    m_score  = 2000;
    m_energy = 1;
    m_polygon.resize(BLOCKADE_ENEMY_POLYGON.size());
    transform_points(m_polygon, BLOCKADE_ENEMY_POLYGON, m_pos);
    m_world.get_wall().mark_tile(m_pos);
}

void BlockadeEnemy::die() {
    // TODO: clear wall cell
    m_world.get_wall().erase_tile_mark(m_pos);

    if (m_neighbors[0]) {
        m_neighbors[0]->m_ttl = 9;
        m_neighbors[0]->m_neighbors[1] = nullptr;
    }

    if (m_neighbors[1]) {
        m_neighbors[1]->m_ttl = m_ttl == -1 ? 5 : 10;
        m_neighbors[1]->m_neighbors[0] = nullptr;
    }
}

void BlockadeEnemy::sub_update() {
    m_pos.y += Wall::SPEED;
    transform_points(m_polygon, BLOCKADE_ENEMY_POLYGON, m_pos);
    if (m_ttl >= 0 && --m_ttl <= 0) {
        m_score = 0;
        hit(m_shield);
    }
}
