#pragma once
#include "sprite_renderer.hpp"
#include "util.hpp"
#include <array>


class World;


class Player {
public:
    Player(World const& world) : m_world(world) {}

    struct Input {
        int dx;
        int dy;
        bool a;
        bool b;
    };


    void reset();
    void update(Input const& input);
    void draw(SpriteRenderer& ren);

    enum {
        MAX_SHIELD = 3,
        MAX_ENERGY = 30
    };

private:
    void hit(CollisionInfo const& info);

    World const&        m_world;

    std::array<vec2, 6> m_polygon;

    bool                m_alive;
    vec2                m_pos;

    int                 m_tick;

    int                 m_score;
    int                 m_shild;
    int                 m_energy;
    bool                m_field_active;
    int                 m_speed_boost;
    int                 m_invcible_delay;

    int                 m_shoot_delay;
    bool                m_side_shot;

    int                 m_blast_delay;
    vec2                m_blast_vel;

    // TODO:
    // balls
    // energy blast
};
