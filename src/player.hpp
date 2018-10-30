#pragma once
#include "sprite.hpp"
#include "util.hpp"
#include "fx.hpp"
#include <array>


class World;


class Ball {
public:
    Ball(World& world, int dir);
    void reset();
    void activate(vec2 const& player_pos, int player_tick);
    void shoot(bool side_shot);
    void hit();
    void update(vec2 const& player_pos);
    void draw(SpriteRenderer& ren) const;

    bool is_alive() const { return m_alive; }
    std::array<vec2, 4> const& get_polygon() const { return m_polygon; }

private:
    World&              m_world;
    int const           m_dir;
    vec2 const          m_offset;

    std::array<vec2, 4> m_polygon;

    bool                m_alive;
    vec2                m_pos;
    float               m_glide;
    int                 m_tick;
};


class Player {
public:
    Player(World& world)
        : m_world(world)
        , m_balls({ Ball(world, -1), Ball(world, 1) })
    {}

    void reset();
    void update(fx::Input const& input);
    void draw(SpriteRenderer& ren) const;

    void activate_balls();

    bool is_alive() const { return m_alive; }
    bool is_invincible() const { return m_invincible_delay > 0; }
    vec2 const& get_pos() const { return m_pos; }
    int get_score() const { return m_score; }
    int get_shield() const { return m_shield; }
    std::array<vec2, 6> const& get_polygon() const { return m_polygon; }
    std::array<Ball, 2>& get_balls() { return m_balls; }

    void inc_score(int points) { m_score += points; }
    void hit(CollisionInfo const& info = {});

    enum {
        MAX_SHIELD = 3,
        MAX_ENERGY = 30
    };

private:

    World&              m_world;

    std::array<vec2, 6> m_polygon;

    bool                m_alive;
    vec2                m_pos;

    int                 m_tick;

    int                 m_score;
    int                 m_shield;
    int                 m_energy;
    bool                m_field_active;
    int                 m_speed_boost;
    int                 m_invincible_delay;

    int                 m_shoot_delay;
    bool                m_side_shot;
    int                 m_shoot_period;

    int                 m_blast_delay;
    vec2                m_blast_vel;

    std::array<Ball, 2> m_balls;

    // TODO:
    // balls
    // energy blast
};



class Laser {
public:
    Laser(World& world, vec2 const& pos, vec2 const& vel, bool is_small);
    bool update();
    void draw(SpriteRenderer& ren) const;

private:
    World&              m_world;
    vec2                m_pos;
    vec2                m_vel;
    float               m_ang;
    std::array<vec2, 4> m_polygon;
    bool                m_is_small;
};


