#pragma once
#include "enemy.hpp"
#include "world.hpp"


class CannonEnemy : public Enemy {
public:
    CannonEnemy(World& world, uint32_t seed, vec2 const& pos, Footing footing);
    void sub_update() override;
    void sub_draw(SpriteRenderer& ren) const override;
protected:
    float m_cannon_ang;
    int   m_delay;
};
