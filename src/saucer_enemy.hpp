#pragma once
#include "enemy.hpp"
#include "world.hpp"


class SaucerEnemy : public Enemy {
public:
    SaucerEnemy(World& world, uint32_t seed, vec2 const& pos);
    void die() override;
    void sub_update() override;

protected:
    vec2  m_vel = {};
    vec2  m_dst_vel;
    float m_phase;
    bool  m_shooting = false;
};
