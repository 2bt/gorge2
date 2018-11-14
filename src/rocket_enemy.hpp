#pragma once
#include "enemy.hpp"
#include "world.hpp"


class RocketEnemy : public Enemy {
public:
    RocketEnemy(World& world, uint32_t seed, vec2 const& pos, Footing footing);
    void sub_update() override;
protected:
    vec2 m_vel = {};
    vec2 m_normal;
    bool m_active = false;
};
