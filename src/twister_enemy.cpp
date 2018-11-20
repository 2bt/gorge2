#include "twister_enemy.hpp"

namespace {
    constexpr std::array<vec2, 6> TWISTER_ENEMY_POLYGON = {
        vec2{4, 1},
        vec2{4, -1},
        vec2{-2, -4.5},
        vec2{-4, -2},
        vec2{-4, 2},
        vec2{-2, 4.5},
    };
    constexpr std::array<vec2, 6> TURN_LEFT_POLYGON = {
        vec2{10, 3.5},
        vec2{15, -1.5},
        vec2{15, -12},
        vec2{10, -17},
        vec2{-4, -17},
        vec2{-4, 3.5},
    };
    constexpr std::array<vec2, 6> TURN_RIGHT_POLYGON = {
        vec2{10, 17},
        vec2{15, 12},
        vec2{15, 1.5},
        vec2{10, -3.5},
        vec2{-4, -3.5},
        vec2{-4, 17},
    };
}


TwisterEnemyChain::TwisterEnemyChain(World& world, uint32_t seed, vec2 const& pos) : m_world(world) {
    m_pos     = pos;
    m_count   = 4;
}
bool TwisterEnemyChain::update() {
    m_pos.y += Wall::SPEED;
    if (m_tick++ % 32 == 0) {
        m_world.spawn_enemy<TwisterEnemy>(m_pos, F_NORTH, m_seed);
        if (--m_count <= 0) return false;
    }
    return true;
}


TwisterEnemy::TwisterEnemy(World& world, uint32_t seed, vec2 const& pos, Footing footing, uint32_t path_seed) : Enemy(world, seed, pos) {
    m_sprite  = Sprite::TWISTER;
    m_shield  = 2;
    m_score   = 120;
    m_energy  = 2;
    m_dst_ang = get_angle(footing) - M_PI * 0.5;
    m_ang     = m_dst_ang;
    m_count   = m_path_random.get_int(10, 200);
    m_path_random.seed(path_seed ?: seed);
    m_polygon.resize(TWISTER_ENEMY_POLYGON.size());
    transform_points(m_polygon, TWISTER_ENEMY_POLYGON, m_pos);
}

void TwisterEnemy::sub_update() {
    m_pos.y += Wall::SPEED;
    m_pos += vec2(std::cos(m_ang), std::sin(m_ang)) * 0.5f;

    if      (m_ang < m_dst_ang) m_ang = std::min(m_ang + 0.1f, m_dst_ang);
    else if (m_ang > m_dst_ang) m_ang = std::max(m_ang - 0.1f, m_dst_ang);
    else {
        --m_count;

        transform_points(m_polygon, TURN_LEFT_POLYGON, m_pos, m_ang);
        CollisionInfo l = m_world.get_wall().check_collision(m_polygon);
        transform_points(m_polygon, TURN_RIGHT_POLYGON, m_pos, m_ang);
        CollisionInfo r = m_world.get_wall().check_collision(m_polygon);

        if (m_count <= 0 || (l.distance > 0 && r.distance > 0)) {
            m_count = m_path_random.get_int(10, 200);
            if (l.distance == 0 && r.distance == 0) {
                if (std::abs(m_pos.y) > 75) m_dst_ang = M_PI * 0.5; // turn down
                else m_dst_ang += (m_path_random.get_int(0, 1) ? -1 : 1) * M_PI * 0.5;
            }
            else if (l.distance < r.distance) m_dst_ang -= M_PI * 0.5;
            else                              m_dst_ang += M_PI * 0.5;
        }
    }

    transform_points(m_polygon, TWISTER_ENEMY_POLYGON, m_pos, m_ang);

    if (m_random.get_int(0, 600) == 0 && can_see_player()) {
        vec2 dir = m_world.get_player().get_pos() - m_pos;
        float ang = atan2(dir.y, dir.x);
        ang += m_random.get_float(-0.2, 0.2);
        vec2 vel = vec2(std::cos(ang), std::sin(ang)) * 1.0f;
        m_world.spawn_bullet(m_pos, vel, PLASMA_BULLET_DESC);
    }
}
