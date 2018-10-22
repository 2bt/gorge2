#include "enemy.hpp"
#include "world.hpp"


bool Enemy::update() {
    if (!m_alive) {
        die();
        return false;
    }

    ++m_tick;


    sub_update();
    return true;
}

void Enemy::draw(SpriteRenderer& ren) const {
    ren.draw(frame(m_sprite, m_tick / m_frame_length % frame_count(m_sprite)), m_pos);
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
}

SquareEnemy::SquareEnemy(World& world, uint32_t seed, vec2 const& pos) : Enemy(world, seed) {
    m_pos    = pos;
    m_sprite = Sprite::SQUARE;
    m_shild  = 4;

	m_tick  = m_random.get_int(1, 100);
	m_delay = m_random.get_int(1, 300);
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

}

