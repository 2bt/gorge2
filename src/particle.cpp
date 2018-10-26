#include "particle.hpp"
#include "world.hpp"


bool Particle::update() {
    m_pos += m_vel;
    m_vel *= m_friction;
    return --m_ttl > 0;
}


void SparkParticle::draw(SpriteRenderer& ren) const {
    ren.set_color(m_color);
    ren.draw(frame(Sprite::SPARK), m_pos);
}
