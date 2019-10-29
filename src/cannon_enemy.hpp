#pragma once
#include "enemy.hpp"
#include "world.hpp"


class CannonEnemy : public Enemy {
public:
    CannonEnemy(World& world, uint32_t seed, vec2 const& pos, float ang);
    void sub_update() override;
    void sub_draw(SpriteRenderer& ren) const override;
    void die() override { m_world.maybe_spawn_spawn_powerup(m_pos, 2); }
private:
    float m_cannon_ang;
    int   m_delay;
};
