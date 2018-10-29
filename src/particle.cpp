#include "particle.hpp"
#include "world.hpp"


bool Particle::update() {
    m_pos += m_vel;
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
        void draw(SpriteRenderer& ren) const override {
            int f = std::max<int>(0, frame_count(Sprite::SMOKE) - 1 - std::floor(m_ttl * 0.33));
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

    class ExplosionParticle : public Particle {
    public:
        ExplosionParticle(vec2 const& pos) {
            m_layer    = FRONT;
            m_pos      = pos;
            m_friction = 1.0;
            m_vel      = {0, 0.25}; // XXX: move with walls
            m_ttl      = frame_count(Sprite::EXPLOSION) * 3;
        }
        void draw(SpriteRenderer& ren) const override {
            int f = frame_count(Sprite::EXPLOSION) - m_ttl / 3 - 1;
            ren.set_color();
            ren.draw(frame(Sprite::EXPLOSION, f), m_pos);
        }
    private:
        Color m_color;
    };


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
