#pragma once
#include "sprite_renderer.hpp"
#include "util.hpp"


class World;


class Particle {
public:
    enum Layer { BACK, FRONT };

    virtual ~Particle() {}
    virtual bool update() = 0;
    virtual void draw(SpriteRenderer& ren) const = 0;
    virtual Layer layer() const = 0;
};



class SparkParticle : public Particle {
public:
    SparkParticle(World const& world, vec2 const& pos);
    bool update() override;
    void draw(SpriteRenderer& ren) const override;
    Layer layer() const override { return BACK; }

protected:
    World const& m_world;
    vec2         m_pos;
    vec2         m_vel;
    int          m_ttl;
    Color        m_color    = {255, 255, 255, 255};
    float        m_friction = 1;
};



class LaserParticle : public SparkParticle {
public:
    LaserParticle(World const& world, vec2 const& pos) : SparkParticle(world, pos) {
        m_color    = {0, 155, 155, 255};
        m_friction = 0.7;
    }
};
