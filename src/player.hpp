#pragma once
#include "sprite.hpp"
#include "util.hpp"
#include <array>


class World;


class ShockWave {
public:
    void reset();
    void activate(vec2 const& pos);
    void update();
    void draw(SpriteRenderer& ren) const;

    bool overlap(vec2 const* poly, int len) const;
    template<class T>
    bool overlap(T const& poly) const {
        return overlap(poly.data(), poly.size());
    }
private:
    vec2              m_pos;
    bool              m_alive;
    float             m_radius;
    float             m_level;
    int               m_tick;
};


class Ball {
public:
    Ball(World& world, int dir);
    void reset();
    void activate();
    void shoot(bool side_shot);
    void hit(CollisionInfo const& info = {});
    void update();
    void draw(SpriteRenderer& ren) const;
    void draw_field(SpriteRenderer& ren) const;

    bool is_alive() const { return m_alive; }
    std::array<vec2, 4> const& get_polygon() const { return m_polygon; }
    std::array<vec2, 8> const& get_field_polygon() const { return m_field_polygon; }

private:
    World&              m_world;
    int const           m_dir;
    vec2 const          m_offset;

    std::array<vec2, 4> m_polygon;
    std::array<vec2, 8> m_field_polygon;

    bool                m_alive;
    vec2                m_pos;
    float               m_glide;
};



class Player {
public:
    struct Input {
        vec2 mov;
        bool a;
        bool b;
    };

    Player(World& world)
        : m_world(world)
        , m_balls({ Ball(world, -1), Ball(world, 1) })
    {}

    void reset();
    void update(Input const& input);
    void draw(SpriteRenderer& ren) const;

    bool is_alive() const { return m_alive; }
    bool is_invincible() const { return m_invincible_delay > 0; }
    bool is_field_active() const { return m_field_active; }
    vec2 const& get_pos() const { return m_pos; }
    int get_tick() const { return m_tick; }
    int get_score() const { return m_score; }
    int get_shield() const { return m_shield; }
    float get_energy() const { return m_energy; }
    std::array<vec2, 6> const& get_polygon() const { return m_polygon; }
    std::array<vec2, 8> const& get_field_polygon() const { return m_field_polygon; }
    std::array<Ball, 2>& get_balls() { return m_balls; }

    void inc_score(int points) { m_score += points; }
    void inc_speed() { ++m_speed_boost; }
    void inc_shield() { m_shield = std::min<int>(MAX_SHIELD, m_shield + 1); }
    void inc_energy() { m_energy = std::min<float>(MAX_ENERGY, m_energy + 1); }
    void hit(CollisionInfo const& info = {});

    enum {
        MAX_SHIELD      = 3,
        MAX_ENERGY      = 26,
    };

private:

    World&              m_world;

    std::array<vec2, 6> m_polygon;
    std::array<vec2, 8> m_field_polygon;

    bool                m_alive;
    vec2                m_pos;

    int                 m_tick;

    int                 m_score;
    int                 m_shield;
    float               m_energy;
    bool                m_field_active;
    int                 m_speed_boost;
    int                 m_invincible_delay;
    bool                m_old_input_b;

    int                 m_shoot_delay;
    bool                m_side_shot;
    int                 m_shoot_period;

    int                 m_blast_delay;
    vec2                m_blast_vel;

    std::array<Ball, 2> m_balls;
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


