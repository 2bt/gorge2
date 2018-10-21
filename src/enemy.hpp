#pragma once
#include "sprite_renderer.hpp"
#include "util.hpp"


class World;


class Enemy {
public:
    Enemy(World& world) : m_world(world) {}
    virtual ~Enemy() {}
    virtual bool update() = 0;
    virtual void draw(SpriteRenderer& ren) const = 0;
protected:
    World& m_world;
};



class FooEnemy : public Enemy {
public:
    FooEnemy(World const& world, vec2 const& pos);
    bool update() override;
    void draw(SpriteRenderer& ren) const override;

protected:
    vec2         m_pos;
    vec2         m_vel;
};
