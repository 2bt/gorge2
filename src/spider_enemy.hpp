#pragma once
#include "enemy.hpp"
#include "world.hpp"


class SpiderEnemy : public Enemy {
public:
    SpiderEnemy(World& world, uint32_t seed, vec2 const& pos, Footing footing);
    void sub_update() override;
    void sub_draw(SpriteRenderer& ren) const override;
protected:
    vec2  m_vel = {};
    vec2  m_normal;
    int   m_dir;
    float m_sprite_ang;
    bool  m_floating = false;
    bool  m_shooting = false;

    int   m_delay;
};
