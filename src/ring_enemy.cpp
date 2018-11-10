#include "ring_enemy.hpp"

namespace {
    constexpr std::array<vec2, 8> RING_ENEMY_POLYGON = {
        vec2{2, 4},
        vec2{3, 2},
        vec2{3, -2},
        vec2{2, -4},
        vec2{-2, -4},
        vec2{-3, -2},
        vec2{-3, 2},
        vec2{-2, 4},
    };
}

RingEnemy::RingEnemy(World& world, uint32_t seed, vec2 const& pos) : Enemy(world, seed, pos) {
    m_sprite = Sprite::RING;
    m_shield = 2;
    m_score  = 100;
    m_energy = 1;
    m_tick  = m_random.get_int(0, 100);
    m_delay = m_random.get_int(0, 200);
    turn(0.2, M_PI - 0.2);
    m_polygon.resize(RING_ENEMY_POLYGON.size());
    transform(m_polygon, RING_ENEMY_POLYGON, m_pos);
}

void RingEnemy::turn(float ang1, float ang2) {
    float ang = m_random.get_float(ang1, ang2);
    m_vel = vec2(std::cos(ang), std::sin(ang)) * 0.375f;
}

void RingEnemy::sub_update() {

    m_pos += m_vel + vec2(0, Wall::SPEED);

    transform(m_polygon, RING_ENEMY_POLYGON, m_pos);
    CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
    if (info.distance > 0) {
        m_pos += info.normal * info.distance;
        turn(0, 2 * M_PI);
        transform(m_polygon, RING_ENEMY_POLYGON, m_pos);
    }

    m_delay += m_random.get_int(1, 4);
    if (m_delay > 200) {
        m_delay = 0;
        if (m_random.get_float(0, 1) < 0.6) {
            turn(0, M_PI);
        }
        else {
            turn(M_PI, M_PI * 2);
        }
    }
}
