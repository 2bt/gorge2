#pragma once
#include "enemy.hpp"
#include "world.hpp"


class SquareEnemy : public Enemy {
public:
    SquareEnemy(World& world, uint32_t seed, vec2 const& pos);
    void die() override;
    void sub_update() override;

protected:
    vec2 m_vel;
    int  m_delay;
};
