#pragma once
#include "enemy.hpp"
#include "world.hpp"


class TwisterEnemy : public Enemy {
public:
    TwisterEnemy(World& world, uint32_t seed, vec2 const& pos, Footing footing);
    void sub_update() override;
protected:
    float m_dst_ang;
    int   m_delay;
    int   m_count;
};
