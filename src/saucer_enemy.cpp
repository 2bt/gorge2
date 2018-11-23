#include "saucer_enemy.hpp"

namespace {
    constexpr std::array<vec2, 8> SAUCER_ENEMY_POLYGON = {
        vec2{-4, 6},
        vec2{4, 6},
        vec2{10, 2},
        vec2{10, 0},
        vec2{2, -7},
        vec2{-2, -7},
        vec2{-10, 0},
        vec2{-10, 2},
    };

    constexpr std::array<vec2, 8> BOUNCE_POLYGON = {
        vec2{6, 12},
        vec2{12, 6},
        vec2{12, -6},
        vec2{6, -14},
        vec2{-6, -14},
        vec2{-12, -6},
        vec2{-12, 6},
        vec2{-6, 12},
    };

    const Bullet::Desc SAUCER_BULLET_DESC = {
        true,
        Sprite::SAUCER_BULLET,
        1,
        {
            vec2{-0.5, 2.5},
            vec2{0.5, 2.5},
            vec2{0.5, -2.5},
            vec2{-0.5, -2.5},
        },
        Color(153, 153, 72, 200),
    };

}

SaucerEnemy::SaucerEnemy(World& world, uint32_t seed, vec2 const& pos) : Enemy(world, seed, pos) {
    m_sprite = Sprite::SAUCER;
    m_shield = 40;
    m_score  = 5000;
    m_energy = 10;

    m_dst_vel = {m_random.get_int(0, 1) ?: -1, -1};
    m_phase = m_random.get_float(0, M_PI * 2);

    m_polygon.resize(SAUCER_ENEMY_POLYGON.size());
    transform_points(m_polygon, SAUCER_ENEMY_POLYGON, m_pos);
}

void SaucerEnemy::die() {
//    m_world.spawn_item<BallItem>(m_pos);
}

void SaucerEnemy::sub_update() {

    m_pos += m_vel + vec2(0, Wall::SPEED + std::sin(m_tick * 0.125) * 0.175);


    constexpr float ACCEL = 0.025;
    m_vel = glm::clamp(m_dst_vel * 0.25f, m_vel - ACCEL, m_vel + ACCEL);


    transform_points(m_polygon, BOUNCE_POLYGON, m_pos);
    CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
    if (info.distance > 0) {
        if (info.normal.x < -0.05) m_dst_vel.x = -1;
        if (info.normal.x >  0.05) m_dst_vel.x = 1;
        if (info.normal.y < -0.05) m_dst_vel.y = -1;
        if (info.normal.y >  0.05) m_dst_vel.y = 0.5;
    }
    else {
        if (m_tick < 2000) {
            float oy = std::sin(m_phase + m_tick * 0.005) * 38 - 20;
            m_dst_vel.y = clamp(oy - m_pos.y, -1.75f, 0.5f);
        }
        else {
            m_dst_vel.y = 0.5;
        }
    }

    transform_points(m_polygon, SAUCER_ENEMY_POLYGON, m_pos);

    if (m_pos.y < -67) return;

    // shoot
    int t = m_tick % 200;
    if (t == 0) m_shooting = m_world.get_player().is_alive();
    if (m_shooting && t >= 5 && t < 90 && t % 5 == 0) {
        int p = t / 5 % 4 - 1;
        if (p == 2) p = 0;
        m_world.spawn_bullet(m_pos + vec2(p * 5, 6), {0, 2.5}, SAUCER_BULLET_DESC);
    }
}
