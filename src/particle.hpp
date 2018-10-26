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
    float m_friction;

    vec2  m_pos;
    vec2  m_vel;
    int   m_ttl;
};


class SparkParticle : public Particle {
public:
    void draw(SpriteRenderer& ren) const override;
protected:
    Color m_color;
};
