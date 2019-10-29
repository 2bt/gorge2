#pragma once
#include "enemy.hpp"
#include "world.hpp"


class SquareEnemy : public Enemy {
public:
    SquareEnemy(World& world, uint32_t seed, vec2 const& pos);
    void die() override { m_world.maybe_spawn_spawn_powerup(m_pos, 2); }
    void sub_update() override;

private:
    vec2 m_vel;
    int  m_delay;
};
