#include "spider_enemy.hpp"
#include <glm/gtc/epsilon.hpp>

namespace {
    std::array<vec2, 8> SPIDER_ENEMY_POLYGON = {
        vec2{1, 4},
        vec2{4, 1},
        vec2{4, -1},
        vec2{1, -4},
        vec2{-1, -4},
        vec2{-4, -1},
        vec2{-4, 1},
        vec2{-1, 4},
    };

    const Bullet::Desc SPIDER_BULLET_DESC = {
        false,
        Sprite::SPIDER_BULLET,
        4,
        {
            vec2{-1, 1},
            vec2{1, 1},
            vec2{1, -1},
            vec2{-1, -1},
        },
        Color(153, 144, 0, 200)
    };
}

SpiderEnemy::SpiderEnemy(World& world, uint32_t seed, vec2 const& pos, float ang) : Enemy(world, seed, pos) {
    m_sprite     = Sprite::SPIDER;
    m_shield     = 2;
    m_score      = 550;
    m_energy     = 2;
    m_sprite_ang = ang;
    m_delay      = m_random.get_int(200, 300);
    m_normal     = { std::sin(m_sprite_ang), -std::cos(m_sprite_ang) };
    if      (glm::epsilonEqual<float>(ang, -M_PI / 2, 0.01)) m_dir = -1;
    else if (glm::epsilonEqual<float>(ang,  M_PI / 2, 0.01)) m_dir = 1;
    else                        m_dir = m_random.get_int(0, 1) ?: -1;
    m_polygon.resize(SPIDER_ENEMY_POLYGON.size());
    transform_points(m_polygon, SPIDER_ENEMY_POLYGON, m_pos);
}

void SpiderEnemy::sub_update() {
    m_pos.y += Wall::SPEED;

    if (m_floating) {
        m_sprite_ang += m_dir * 0.05;
        transform_points(m_polygon, SPIDER_ENEMY_POLYGON, m_pos);
        return;
    }

    // stick to wall
    vec2 old_pos = m_pos;
    m_pos -= m_normal * 0.25f;
    transform_points(m_polygon, SPIDER_ENEMY_POLYGON, m_pos);
    CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
    if (info.distance > 0) {
        m_pos += info.normal * info.distance;
        m_normal = info.normal;
    }
    else {
        m_pos = old_pos;
        m_floating = true;
        m_dir = m_random.get_int(0, 1) ?: -1;
    }

    --m_delay;
    if (m_delay > 0) {

        // crawl
        m_pos += vec2(-m_normal.y, m_normal.x) * (m_dir * 0.2f);
        transform_points(m_polygon, SPIDER_ENEMY_POLYGON, m_pos);
        CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
        if (info.distance > 0) {
            m_pos += info.normal * info.distance;
            m_normal = info.normal;
        }

        // turn
        float dst_ang = std::atan2(m_normal.x, -m_normal.y);
        float ad = std::fmod(dst_ang - m_sprite_ang + 3 * M_PI, 2 * M_PI) - M_PI;
        m_sprite_ang += clamp(ad, -0.06f, 0.06f);

        // sight check
        if (m_delay == 1) {
            if (!can_see_player()) m_delay = m_random.get_int(5, 100);
        }
    }
    else {
        if (m_delay == 0) {
            m_shooting = true;
            m_tick = 0;
        }
        if (m_delay < -28 * 3) {
            m_delay = m_random.get_int(200, 300);
            m_dir   = m_random.get_int(0, 1) ?: -1;
            m_shooting = false;
        }

        // shoot
        if (m_tick >= 28 && m_tick % 8 == 0) {
            float l = (m_tick - 28) / 56.0f;
            if (m_dir == -1) l = 1 - l;
            float ang = l * M_PI - m_sprite_ang;
            vec2 vel = vec2(std::cos(ang), -std::sin(ang)) * 0.75f;
            m_world.spawn_bullet(m_pos, vel, SPIDER_BULLET_DESC);
        }

    }

    transform_points(m_polygon, SPIDER_ENEMY_POLYGON, m_pos);
}
void SpiderEnemy::sub_draw(SpriteRenderer& ren) const {
    int f = m_shooting ? 4 : 0;
    f += m_tick / 7 % 4;
    ren.push();
    ren.translate(m_pos);
    ren.scale({m_dir, 1});
    ren.rotate(m_sprite_ang * m_dir);
    ren.draw(frame(m_sprite, f));
    ren.pop();
}
