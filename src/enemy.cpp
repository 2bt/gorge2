#include "enemy.hpp"
#include "world.hpp"
#include "debug_renderer.hpp"


bool Enemy::update() {
    if (!m_alive) {
        die();
        return false;
    }

    ++m_tick;

    sub_update();
    return true;
}

void Enemy::hit(int damage) {
    m_flash = 3;
    m_shield -= damage;
    if (m_shield <= 0) {
        m_alive = false;
        m_world.get_player().inc_score(m_score);
    }
}

bool Enemy::can_see_player() const {
    return !m_world.get_wall().check_sight(m_pos, m_world.get_player().get_pos());
}

void Enemy::draw(SpriteRenderer& ren) const {
    ren.draw(frame(m_sprite, m_tick / m_frame_length % frame_count(m_sprite)), m_pos);

    // DEBUG
//    bool c = m_world.get_wall().check_sight(m_pos, m_world.get_player().get_pos());
//    if (c) DB_REN.set_color(255, 0, 0);
//    else DB_REN.set_color(200, 200, 200);
//    DB_REN.line(m_pos, m_world.get_player().get_pos());
}

Bullet::Bullet(World& world, vec2 const& pos, vec2 const& vel, Desc const& desc)
    : m_world(world), m_pos(pos), m_vel(vel), m_desc(desc)
{
    m_ang = m_desc.rotate ? std::atan2(m_vel.x, -m_vel.y) : 0;
}

bool Bullet::update() {
    ++m_tick;
    for (int i = 0; i < 2; ++i) {
        m_pos += m_vel;
        if (std::abs(m_pos.x) > 124 || std::abs(m_pos.y) > 80) {
            return false;
        }

        // collision with wall
        transform(m_polygon, m_desc.polygon, m_pos, m_ang);
        CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
        if (info.distance > 0) {
            for (int i = 0; i < 10; ++i) {
                m_world.spawn_particle<SparkParticle>(info.where, m_desc.spark_desc);
            }
            return false;
        }


    }

    return true;
}

void Bullet::draw(SpriteRenderer& ren) const {
    ren.push();
    ren.translate(m_pos);
    ren.rotate(m_ang);
    ren.draw(frame(m_desc.sprite, m_tick / m_desc.frame_length % frame_count(m_desc.sprite)));
    ren.pop();
}


namespace {
    constexpr std::array<vec2, 8> SQUARE_ENEMY_POLYGON = {
        vec2{2, 4},
        vec2{4, 2},
        vec2{4, -2},
        vec2{2, -4},
        vec2{-2, -4},
        vec2{-4, -2},
        vec2{-4, 2},
        vec2{-2, 4}
    };

    constexpr std::array<vec2, 8> BOUNCE_POLYGON = {
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
            vec2{0.5, 1.5},
            vec2{0.5, -1.5},
            vec2{-0.5, -1.5},
            vec2{-0.5, 1.5},
        },
        {
            Color(87, 153, 87, 155),
            0.7
        }
    };

}

SquareEnemy::SquareEnemy(World& world, uint32_t seed, vec2 const& pos) : Enemy(world, seed) {
    m_pos    = pos;
    m_sprite = Sprite::SQUARE;
    m_shield = 4;
    m_score  = 350;

    m_tick  = m_random.get_int(1, 100);

//    m_delay = m_random.get_int(1, 300);
    m_delay = 100;

    m_vel   = {m_random.get_float(-4, 4), 1};
    m_vel   = glm::normalize(m_vel) * 0.275f;
    m_polygon.resize(SQUARE_ENEMY_POLYGON.size());
}

void SquareEnemy::sub_update() {

    m_pos += m_vel;
    m_pos.y += m_world.get_wall().get_speed();

    float p = std::sin(m_tick * 0.1);

    m_pos += vec2(m_vel.y, -m_vel.x) * p;

    transform(m_polygon, BOUNCE_POLYGON, m_pos);
    CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
    if (info.distance > 0) {
        m_vel += info.normal * 0.015f;
    }
    else {
        m_vel.y += 0.002;
        m_vel = glm::normalize(m_vel) * 0.275f;
    }

    transform(m_polygon, SQUARE_ENEMY_POLYGON, m_pos);
    info = m_world.get_wall().check_collision(m_polygon);
    if (info.distance > 0) {
        m_pos += info.normal * info.distance;
        transform(m_polygon, SQUARE_ENEMY_POLYGON, m_pos);
    }


    if (std::abs(m_pos.y) > 75) return;
    if (!m_world.get_player().is_alive()) return;


    // shoot
    --m_delay;
    if (m_delay == 20 && !can_see_player()) {
        m_delay = m_random.get_int(22, 70);
    }
    if (m_delay == 10 || m_delay == 0) {
        vec2 dir = m_world.get_player().get_pos() - m_pos;
        float ang = std::atan2(dir.x, dir.y) + m_random.get_float(-0.2, 0.2);
        vec2 vel = vec2(std::sin(ang), std::cos(ang)) * m_random.get_float(0.5, 0.53);
        m_world.spawn_bullet(m_pos, vel, RAPID_BULLET_DESC);
    }
    if (m_delay <= 0) m_delay = m_random.get_int(200, 300);
}


