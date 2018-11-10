#pragma once
#include "enemy.hpp"
#include "world.hpp"


enum Footing {
    F_NORTH,
    F_WEST,
    F_SOUTH,
    F_EAST,
};


class CannonEnemy : public Enemy {
public:
    CannonEnemy(World& world, uint32_t seed, vec2 const& pos, Footing footing);
    void sub_update() override;
    void sub_draw(SpriteRenderer& ren) const override;
protected:
    float m_ang;
    float m_cannon_ang;
    int   m_delay;
};
