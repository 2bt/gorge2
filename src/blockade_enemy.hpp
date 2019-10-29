#pragma once
#include "enemy.hpp"
#include "world.hpp"


class BlockadeEnemy : public Enemy {
public:
    BlockadeEnemy(World& world, uint32_t seed, vec2 const& pos, BlockadeEnemy* left_neighbor);
    void die() override;
    void sub_update() override;
private:
    std::array<BlockadeEnemy*, 2> m_neighbors = {};
    int m_ttl = -1;
};
