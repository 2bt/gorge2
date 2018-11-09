#pragma once
#include "enemy.hpp"
#include "world.hpp"


class RingEnemy : public Enemy {
public:
    RingEnemy(World& world, uint32_t seed, vec2 const& pos);
    void sub_update() override;

protected:
    void turn(float ang1, float ang2);

    vec2 m_vel;
    int  m_delay;
};
