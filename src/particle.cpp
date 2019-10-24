#include "particle.hpp"
#include "world.hpp"


bool Particle::update() {
    m_pos += m_vel + vec2(0, Wall::SPEED);
    m_vel *= m_friction;
    return --m_ttl >= 0;
}

namespace {
    class SmokeParticle : public Particle {
    public:
        SmokeParticle(vec2 const& pos) {
            m_friction = 0.8;
            m_layer    = BACK;
            m_pos      = pos;
            float ang  = rnd.get_float(0, M_PI * 2);
            m_vel      = vec2(std::sin(ang), std::cos(ang)) * rnd.get_float(0.5, 1.25);
            m_ttl      = rnd.get_int(18, 26);
        }
        void do_rocket_smoke() {
            m_ttl = 8;
            m_vel = {};
        }
        void draw(SpriteRenderer& ren) const override {
            int f = std::max<int>(0, frame_count(Sprite::SMOKE) - m_ttl / 3 - 1);
            ren.set_color({30, 30, 30, 150});
            ren.draw(frame(Sprite::SMOKE, f), m_pos);
        }
    };

    class SparkParticle : public Particle {
    public:
        SparkParticle(vec2 const& pos) {
            m_friction = 0.95;
            m_layer    = FRONT;
            m_pos      = pos;
            float ang  = rnd.get_float(0, M_PI * 2);
            m_vel      = vec2(std::sin(ang), std::cos(ang)) * rnd.get_float(0.5, 1);
            m_ttl      = rnd.get_int(10, 15);
            m_color    = {255, rnd.get_float(0, 255), 0, 255};
        }
        void draw(SpriteRenderer& ren) const override {
            Color c = m_color;
            c.a = std::min(255, m_ttl * 50);
            ren.set_color(c);
            ren.draw(frame(Sprite::SPARK), m_pos);
        }
    private:
        Color m_color;
    };


    class AnimationParticle : public Particle {
    public:
        AnimationParticle(Sprite sprite, int frame_length = 3)
            : m_sprite(sprite)
            , m_frame_length(frame_length)
        {
            m_ttl = frame_count(m_sprite) * m_frame_length ;
        }
        void draw(SpriteRenderer& ren) const override {
            int f = std::max(0, frame_count(m_sprite) - m_ttl / m_frame_length - 1);
            ren.set_color();
            ren.draw(frame(m_sprite, f), m_pos);
        }
    private:
        const Sprite m_sprite;
        const int    m_frame_length;
    };


    class ExplosionParticle : public AnimationParticle {
    public:
        ExplosionParticle(vec2 const& pos)
            : AnimationParticle(Sprite::EXPLOSION)
        {
            m_layer = FRONT;
            m_pos   = pos;
        }
    };

    class SparkleParticle : public AnimationParticle {
    public:
        SparkleParticle(vec2 const& pos, bool menu = false)
            : AnimationParticle(Sprite::SPARKLE)
        {
            m_layer = FRONT;
            m_pos   = pos;
            if (menu) m_vel = vec2(0, -Wall::SPEED);
        }
        int& ttl() { return m_ttl; }
    };

}

void make_rocket_smoke(World& world, vec2 const& pos) {
    world.spawn_particle<SmokeParticle>(pos)->do_rocket_smoke();
}

void make_explosion(World& world, vec2 const& pos) {
    for (int i = 0; i < 10; ++i) world.spawn_particle<SmokeParticle>(pos);
    for (int i = 0; i < 20; ++i) {
        vec2 p = {
            rnd.get_float(-2.5, 2.5),
            rnd.get_float(-2.5, 2.5)
        };
        world.spawn_particle<SparkParticle>(pos + p);
    }
    world.spawn_particle<ExplosionParticle>(pos);
    world.get_bump().spawn(pos);
}


void make_small_explosion(World& world, vec2 const& pos, bool smoke) {
    if (smoke) {
        for (int i = 0; i < 3; ++i) world.spawn_particle<SmokeParticle>(pos);
    }
    for (int i = 0; i < 10; ++i) {
        vec2 p = {
            rnd.get_float(-0.5, 0.5),
            rnd.get_float(-0.5, 0.5)
        };
        world.spawn_particle<SparkParticle>(pos + p);
    }
}

void make_sparkle(World& world, vec2 const& pos, bool menu) {
    world.spawn_particle<SparkleParticle>(pos, menu);
}

void make_sparkles(World& world, vec2 const& pos) {
    float ang = rnd.get_float(0, 2 * M_PI);
    for (int i = 0; i < 3; ++i) {

        auto p = world.spawn_particle<SparkleParticle>(pos + vec2(std::sin(ang), std::cos(ang)) * 3.0f);
        p->ttl() -= i * 5;

        ang += M_PI * 2 / 3;
    }
}
