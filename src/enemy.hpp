#pragma once
#include "sprite.hpp"
#include "util.hpp"


class World;


class Enemy {
public:
    Enemy(World& world, uint32_t seed) : m_world(world) {
        m_random.seed(seed);
    }
    virtual ~Enemy() {}
    bool update();
    void draw(SpriteRenderer& ren) const;
    virtual void die() {};
    virtual void sub_update() = 0;
    std::vector<vec2> const& get_polygon() const { return m_polygon; }
protected:
    World& m_world;
    Random m_random;
    int    m_tick  = 0;
    bool   m_alive = true;

    // set these in subclass
    vec2   m_pos;
    Sprite m_sprite;
    int    m_frame_length = 4;
    int    m_shild        = 1;

    std::vector<vec2> m_polygon;
};



class SquareEnemy : public Enemy {
public:
    SquareEnemy(World& world, uint32_t seed, vec2 const& pos);
    void sub_update() override;

protected:

    vec2 m_vel;
    int  m_delay;
};
