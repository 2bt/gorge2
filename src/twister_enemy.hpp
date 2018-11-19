#pragma once
#include "enemy.hpp"
#include "world.hpp"


class TwisterEnemyChain : public Particle {
public:
    TwisterEnemyChain(World& world, uint32_t seed, vec2 const& pos);
    bool update() override;
    void draw(SpriteRenderer& ren) const override {}
private:
    World&   m_world;
    uint32_t m_seed;
    int      m_count;
    int      m_tick = 0;
};

class TwisterEnemy : public Enemy {
public:
    TwisterEnemy(World& world, uint32_t seed, vec2 const& pos, Footing footing, uint32_t path_seed = 0);
    void sub_update() override;
protected:
    Random m_path_random;
    float  m_dst_ang;
    int    m_count;
};
