#include "particle.hpp"
#include "world.hpp"


SparkParticle::SparkParticle(World const& world, vec2 const& pos)
    : m_world(world)
{
    m_pos = pos;
    float ang = rnd.get_float(0, M_PI * 2);
    m_vel = vec2(std::sin(ang), std::cos(ang)) * rnd.get_float(0.5, 1);
    m_ttl = rnd.get_int(3, 7);
}

bool SparkParticle::update() {
    m_pos += m_vel;
    m_pos.y += m_world.get_wall().get_speed();
    m_vel *= m_friction;
    return --m_ttl > 0;
}

void SparkParticle::draw(SpriteRenderer& ren) const {
    ren.set_color(m_color);
    ren.draw(frame(Sprite::SPARK), m_pos);
}
