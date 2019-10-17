#include "square_enemy.hpp"

namespace {
    std::array<vec2, 8> SQUARE_ENEMY_POLYGON = {
        vec2{2, 4},
        vec2{4, 2},
        vec2{4, -2},
        vec2{2, -4},
        vec2{-2, -4},
        vec2{-4, -2},
        vec2{-4, 2},
        vec2{-2, 4}
    };

    std::array<vec2, 8> BOUNCE_POLYGON = {
        vec2{4, 8},
        vec2{8, 4},
        vec2{8, -4},
        vec2{4, -8},
        vec2{-4, -8},
        vec2{-8, -4},
        vec2{-8, 4},
        vec2{-4, 8}
    };


    const Bullet::Desc RAPID_BULLET_DESC = {
        true,
        Sprite::RAPID_BULLET,
        1,
        {
            vec2{-0.5, 1.5},
            vec2{0.5, 1.5},
            vec2{0.5, -1.5},
            vec2{-0.5, -1.5},
        },
        Color(87, 153, 87, 200),
    };

}

SquareEnemy::SquareEnemy(World& world, uint32_t seed, vec2 const& pos) : Enemy(world, seed, pos) {
    m_sprite = Sprite::SQUARE;
    m_shield = 4;
    m_score  = 350;
    m_energy = 2;
    m_tick   = m_random.get_int(0, 100);
    m_delay  = m_random.get_int(30, 300);
    m_vel    = {m_random.get_float(-5, 5), 1};
    m_vel    = glm::normalize(m_vel) * 0.275f;
    m_polygon.resize(SQUARE_ENEMY_POLYGON.size());
    transform_points(m_polygon, SQUARE_ENEMY_POLYGON, m_pos);
}

void SquareEnemy::sub_update() {
    m_pos += m_vel + vec2(0, Wall::SPEED);
    float p = std::sin(m_tick * 0.1);
    m_pos += vec2(m_vel.y, -m_vel.x) * p;

    transform_points(m_polygon, BOUNCE_POLYGON, m_pos);
    CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
    if (info.distance > 0) {
        m_vel += info.normal * 0.015f;
    }
    else {
        m_vel.y += 0.002;
        m_vel = glm::normalize(m_vel) * 0.275f;
    }

    transform_points(m_polygon, SQUARE_ENEMY_POLYGON, m_pos);
    info = m_world.get_wall().check_collision(m_polygon);
    if (info.distance > 0) {
        m_pos += info.normal * info.distance;
        transform_points(m_polygon, SQUARE_ENEMY_POLYGON, m_pos);
    }

    // shoot
    --m_delay;
    if (m_delay == 20 && !can_see_player()) {
        m_delay = m_random.get_int(22, 70);
    }
    if (m_delay == 10 || m_delay == 0) {
        vec2 dir = m_world.get_player().get_pos() - m_pos;
        float ang = std::atan2(dir.x, dir.y) + m_random.get_float(-0.2, 0.2);
        vec2 vel = vec2(std::sin(ang), std::cos(ang)) * m_random.get_float(1, 1.06);
        m_world.spawn_bullet(m_pos, vel, RAPID_BULLET_DESC);
    }
    if (m_delay <= 0) m_delay = m_random.get_int(200, 300);
}
