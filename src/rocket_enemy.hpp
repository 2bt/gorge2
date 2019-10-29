#pragma once
#include "enemy.hpp"
#include "world.hpp"


class RocketEnemy : public Enemy {
public:
    RocketEnemy(World& world, uint32_t seed, vec2 const& pos, float ang);
    void sub_update() override;
    void sub_draw(SpriteRenderer& ren) const override;
    void die() override;
    ~RocketEnemy() override;

private:
    vec2 m_vel = {};
    vec2 m_normal;
    bool m_active = false;
    int  m_engine_sound;
};
