#pragma once
#include "sprite.hpp"
#include "util.hpp"


class World;


class Item {
public:
    Item(World& World, vec2 const& pos)
        : m_world(World)
        , m_pos(pos)
        , m_tick(rnd.get_int(0, 100)) {}
    virtual ~Item() {}
    virtual bool update() = 0;
    void draw(SpriteRenderer& ren);

protected:
    World& m_world;
    int    m_frame_length = 3;
    Sprite m_sprite;

    vec2   m_pos;
    int    m_tick;
};

class EnergyItem : public Item {
public:
    EnergyItem(World& World, vec2 const& pos, vec2 const& vel)
        : Item(World, pos)
        , m_vel(vel)
    {
        m_sprite       = Sprite::ENERGY_ITEM;
        m_frame_length = 2;
    }
    bool update() override;
private:
    union {
        std::array<vec2, 4> m_polygon;
    };
    vec2 m_vel;
};


class BigItem : public Item {
public:
    BigItem(World& World, vec2 const& pos) : Item(World, pos) {}
    bool update() override;
    virtual void collect() = 0;
protected:
    union {
        std::array<vec2, 4> m_polygon;
        std::array<vec2, 8> m_bounce_polygon;
    };
    int m_score;
};

class BallItem : public BigItem {
public:
    BallItem(World& World, vec2 const& pos) : BigItem(World, pos) {
        m_sprite = Sprite::BALL_ITEM;
        m_score  = 1000;
    }
    void collect() override;
};


void make_energy_items(World& world, Random& rnd, vec2 const& pos, int count);
