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


        Player& player = m_world.get_player();

        if (player.is_alive()) {

            CollisionInfo info = polygon_collision(m_polygon, player.get_polygon());
            if (info.distance > 0) {
                player.hit();
                for (int i = 0; i < 10; ++i) {
                    m_world.spawn_particle<SparkParticle>(info.where, m_desc.spark_desc);
                }
                return false;
            }

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
