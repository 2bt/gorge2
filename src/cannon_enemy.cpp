#include "cannon_enemy.hpp"
#include "audio.hpp"

namespace {
    std::array<vec2, 6> CANNON_ENEMY_POLYGON = {
        vec2{4, 4},
        vec2{4, 0},
        vec2{2, -4},
        vec2{-2, -4},
        vec2{-4, 0},
        vec2{-4, 4},
    };
}

CannonEnemy::CannonEnemy(World& world, uint32_t seed, vec2 const& pos, float ang) : Enemy(world, seed, pos) {
    m_sprite     = Sprite::CANNON;
    m_shield     = 2;
    m_score      = 300;
    m_energy     = 2;
    m_ang        = ang;
    m_cannon_ang = m_ang + m_random.get_float(-1.3, 1.3);
    m_delay      = m_random.get_int(100, 150);
    m_polygon.resize(CANNON_ENEMY_POLYGON.size());
    transform_points(m_polygon, CANNON_ENEMY_POLYGON, m_pos, m_ang);
}

void CannonEnemy::sub_update() {
    m_pos.y += Wall::SPEED;
    transform_points(m_polygon, CANNON_ENEMY_POLYGON, m_pos, m_ang);

    if (std::abs(m_pos.y) > 80) return;

    Player const& player = m_world.get_player();
    if (!player.is_alive()) return;


    vec2 dir = player.get_pos() - m_pos;

    float ang = std::atan2(dir.x, -dir.y);
    float diff = std::fmod(m_ang - ang + 3 * M_PI, 2 * M_PI) - M_PI;
    if (std::abs(diff) < 1.75 && m_delay < 50 && can_see_player()) {

        float speed = 0.05;
        float d = std::fmod(m_cannon_ang - ang + 3 * M_PI, 2 * M_PI) - M_PI;
        if      (d >  speed) m_cannon_ang -= speed;
        else if (d < -speed) m_cannon_ang += speed;
        else {
            m_cannon_ang -= d;
            if (m_delay == 0) {
                m_delay = m_random.get_int(150, 250);
                dir = glm::normalize(dir);
                m_world.spawn_bullet(m_pos + dir * 4.0f, dir * 1.0f, PLASMA_BULLET_DESC);
                audio::play_sound(audio::ST_PLASMA, m_pos);
            }
        }
    }
    if (m_delay > 0) --m_delay;
}

void CannonEnemy::sub_draw(SpriteRenderer& ren) const {
    ren.draw(frame(m_sprite, 1), m_pos, m_cannon_ang);
    ren.draw(frame(m_sprite, 0), m_pos, m_ang);
}
