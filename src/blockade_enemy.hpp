#pragma once
#include "enemy.hpp"
#include "world.hpp"


class BlockadeEnemy : public Enemy {
public:
    BlockadeEnemy(World& world, uint32_t seed, vec2 const& pos);
    void die() override;
    void sub_update() override;
    void link(BlockadeEnemy* left) {
        m_neighbors[0] = left;
        left->m_neighbors[1] = this;
    }
protected:
    std::array<BlockadeEnemy*, 2> m_neighbors = {};
    int m_ttl = -1;
};
