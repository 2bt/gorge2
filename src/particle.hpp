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
    virtual Layer get_layer() const { return FRONT; }
};



class SparkParticle : public Particle {
public:

    struct Desc {
        Color color;
        float friction;
    };

    SparkParticle(World const& world, vec2 const& pos, Desc const& desc);
    bool update() override;
    void draw(SpriteRenderer& ren) const override;

protected:
    World const& m_world;
    vec2         m_pos;
    vec2         m_vel;
    int          m_ttl;

    Desc const&  m_desc;
};

