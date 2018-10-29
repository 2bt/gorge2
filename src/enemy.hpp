#pragma once
#include "sprite.hpp"
#include "util.hpp"
#include "particle.hpp"


class World;


class Enemy {
public:
    Enemy(World& world, uint32_t seed) : m_world(world) {
        m_random.seed(seed);
    }
    virtual ~Enemy() {}
    bool update();
    void draw(SpriteRenderer& ren) const;

    virtual void die() {}
    virtual void sub_update() = 0;

    bool is_alive() const { return m_alive; }
    std::vector<vec2> const& get_polygon() const { return m_polygon; }

    void hit(int damage);

protected:
    bool can_see_player() const;

    World&            m_world;
    Random            m_random;

    int               m_flash = 0;
    int               m_tick  = 0;
    bool              m_alive = true;

    std::vector<vec2> m_polygon;

    // set these in subclass
    vec2              m_pos;
    Sprite            m_sprite;
    int               m_frame_length = 4;
    int               m_shield;
    int               m_score;

};



class Bullet {
public:

    struct Desc {
        bool                rotate;
        Sprite              sprite;
        int                 frame_length;
        std::array<vec2, 4> polygon;
        Color               spark_color;
    };

    Bullet(World& world, vec2 const& pos, vec2 const& vel, Desc const& desc);
    bool update();
    void draw(SpriteRenderer& ren) const;

private:
    void make_sparks(vec2 const& pos);

    World&              m_world;
    vec2                m_pos;
    vec2                m_vel;
    float               m_ang;
    int                 m_tick = 0;
    std::array<vec2, 4> m_polygon;

    Desc const&         m_desc;
};
