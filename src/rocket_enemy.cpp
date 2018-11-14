#include "rocket_enemy.hpp"

namespace {
    constexpr std::array<vec2, 4> ROCKET_ENEMY_POLYGON = {
        vec2{4, 4},
        vec2{1, -5},
        vec2{-1, -5},
        vec2{-4, 4},
    };
}

RocketEnemy::RocketEnemy(World& world, uint32_t seed, vec2 const& pos, Footing footing) : Enemy(world, seed, pos) {
    m_sprite     = Sprite::ROCKET;
    m_shield     = 2;
    m_score      = 150;
    m_energy     = 1;
    m_ang        = get_angle(footing);
    m_normal     = { std::sin(m_ang), -std::cos(m_ang) };
    m_polygon.resize(ROCKET_ENEMY_POLYGON.size());
    transform(m_polygon, ROCKET_ENEMY_POLYGON, m_pos, m_ang);
}

void RocketEnemy::sub_update() {
    m_pos.y += Wall::SPEED;
    transform(m_polygon, ROCKET_ENEMY_POLYGON, m_pos, m_ang);

    Player const& player = m_world.get_player();
    if (m_active) {
        if (m_tick % 4 == 0) {
            make_rocket_smoke(m_world, m_pos - m_normal * 4.0f);
        }
        m_vel += m_normal * 0.025f;
        m_pos += m_vel;
        transform(m_polygon, ROCKET_ENEMY_POLYGON, m_pos, m_ang);
        CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
        if (info.distance > 0) {
            m_score = 0;
            m_energy = 0;
            hit(m_shield);
        }

    }
    else if (m_pos.y > -75 && player.is_alive()) {
        vec2 d = player.get_pos() - m_pos;
        float dot = glm::dot(m_normal, d);
        if (dot > 0) {
            float cross = m_normal.x * d.y - m_normal.y * d.x;
            if (std::abs(cross) < 14 && can_see_player()) {
                m_active = true;
            }
        }
    }
}
