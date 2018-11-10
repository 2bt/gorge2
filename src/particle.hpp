#pragma once
#include "sprite_renderer.hpp"
#include "util.hpp"


class World;


class Particle {
public:
    enum Layer { BACK, FRONT };

    virtual ~Particle() {}
    virtual bool update();
    virtual void draw(SpriteRenderer& ren) const = 0;
    Layer get_layer() const { return m_layer; }

protected:
    Layer m_layer = FRONT;
    float m_friction = 1;

    vec2  m_pos;
    vec2  m_vel = {};
    int   m_ttl;
};


void make_explosion(World& world, vec2 const& pos);
void make_small_explosion(World& world, vec2 const& pos, bool smoke);
void make_sparkle(World& world, vec2 const& pos);
void make_sparkles(World& world, vec2 const& pos);
