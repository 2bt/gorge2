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


TwisterEnemyChain::TwisterEnemyChain(World& world, vec2 const& pos) : m_world(world) {
    m_pos  = pos;
    m_path = std::make_shared<TwisterPath>();
}
bool TwisterEnemyChain::update() {
    m_pos.y += Wall::SPEED;
    if (m_tick++ % 30 == 0) {
        if (m_tick++ / 30 == 4) return false;
        m_world.spawn_enemy<TwisterEnemy>(m_pos, m_path);
    }
    return true;
}

TwisterEnemy::TwisterEnemy(World& world, uint32_t seed, vec2 const& pos, TwisterPath::Ptr path) : Enemy(world, seed, pos) {
    m_sprite  = Sprite::TWISTER;
    m_shield  = 2;
    m_score   = 120;
    m_energy  = 1;

    m_path = std::move(path);
    m_path->add_twister(this);
    if (m_path->is_leader(this)) {
        m_path->elements.push_back({
            get_angle(F_NORTH) - (float) M_PI * 0.5f,
            m_random.get_int(10, 200)
        });
    }
    m_dst_ang = m_path->elements[m_index].ang;
    m_ang     = m_dst_ang;

    m_polygon.resize(TWISTER_ENEMY_POLYGON.size());
    transform_points(m_polygon, TWISTER_ENEMY_POLYGON, m_pos);
}

void TwisterEnemy::die() {
    m_path->remove_twister(this);
}

void TwisterEnemy::sub_update() {
    m_pos.y += Wall::SPEED;
    m_pos += vec2(std::cos(m_ang), std::sin(m_ang)) * 0.5f;

    if      (m_ang < m_dst_ang) m_ang = std::min(m_ang + 0.1f, m_dst_ang);
    else if (m_ang > m_dst_ang) m_ang = std::max(m_ang - 0.1f, m_dst_ang);
    else {
        ++m_count;

        if (m_path->is_leader(this) && m_index == (int) m_path->elements.size() - 1) {

            transform_points(m_polygon, TURN_LEFT_POLYGON, m_pos, m_ang);
            CollisionInfo l = m_world.get_wall().check_collision(m_polygon);
            transform_points(m_polygon, TURN_RIGHT_POLYGON, m_pos, m_ang);
            CollisionInfo r = m_world.get_wall().check_collision(m_polygon);

            if (m_count >= m_path->elements[m_index].ticks || (l.distance > 0 && r.distance > 0)) {

                m_path->elements[m_index].ticks = m_count;
                m_count = 0;

                if (l.distance == 0 && r.distance == 0) {
                    if (std::abs(m_pos.y) > 70) m_dst_ang = M_PI * 0.5; // turn down
                    else m_dst_ang += (m_random.get_int(0, 1) ? -1 : 1) * M_PI * 0.5;
                }
                else if (l.distance < r.distance) m_dst_ang -= M_PI * 0.5;
                else                              m_dst_ang += M_PI * 0.5;
                m_path->elements.push_back({
                    m_dst_ang,
                    m_random.get_int(10, 200),
                });
                ++m_index;
            }
        }
        else {
            if (m_count >= m_path->elements[m_index].ticks) {
                m_count = 0;
                ++m_index;
                m_dst_ang = m_path->elements[m_index].ang;
            }
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
