#include "enemy.hpp"
#include "world.hpp"
#include "debug_renderer.hpp"


namespace {
    class BulletParticle : public Particle {
    public:
        BulletParticle(vec2 const& pos, Color const& color) {
            m_color    = color;
            m_friction = 0.9;
            m_layer    = FRONT;
            m_pos      = pos;
            float ang  = rnd.get_float(0, M_PI * 2);
            m_vel      = vec2(std::sin(ang), std::cos(ang)) * rnd.get_float(0.5, 1);
            m_ttl      = rnd.get_int(3, 7);
        }
        void draw(SpriteRenderer& ren) const override {
            ren.set_color(m_color);
            ren.draw(frame(Sprite::SPARK), m_pos);
        }
    private:
        Color m_color;
    };
}


void Enemy::hit(int damage) {
    if (!m_alive) return;
    m_flash = 5;
    m_shield -= damage;
    if (m_shield <= 0) {
        m_alive = false;
        die();
        make_explosion(m_world, m_pos);
        make_energy_items(m_world, m_random, m_pos, m_energy);
        m_world.get_player().inc_score(m_score);
    }
}
bool Enemy::can_see_player() const {
    if (std::abs(m_pos.y) > 75) return false;
    Player const& player = m_world.get_player();
    if (!player.is_alive()) return false;
    return !m_world.get_wall().check_sight(m_pos, player.get_pos());
}
bool Enemy::update() {
    if (!m_alive) return false;
    if (!m_entered_screen && m_pos.y < -85) m_entered_screen = true;
    if (m_pos.y > 85) return false;
    ++m_tick;
    if (m_flash > 0) --m_flash;

    // collision with shock wave
    if (!m_hit_by_shock_wave && m_world.get_shock_wave().overlap(m_polygon)) {
        m_hit_by_shock_wave = true;
        hit(8);
    }
    if (m_alive) sub_update();

    return m_alive;
}
void Enemy::sub_draw(SpriteRenderer& ren) const {
    ren.draw(frame(m_sprite, m_tick / m_frame_length % frame_count(m_sprite)), m_pos, m_ang);
}
void Enemy::draw(SpriteRenderer& ren) const {
    ren.set_color({255, 255, 255, m_flash > 0 ? 127 : 255});
    sub_draw(ren);
    // DEBUG
//    DB_REN.set_color(255, 0, 0);
//    DB_REN.polygon(m_polygon);
}


const Bullet::Desc PLASMA_BULLET_DESC = {
    false,
    Sprite::PLASMA_BULLET,
    2,
    {
        vec2{-1, 1},
        vec2{1, 1},
        vec2{1, -1},
        vec2{-1, -1},
    },
    Color(153, 21, 21, 200),
};


Bullet::Bullet(World& world, vec2 const& pos, vec2 const& vel, Desc const& desc)
    : m_world(world), m_pos(pos), m_vel(vel), m_desc(desc)
{
    m_ang = m_desc.rotate ? std::atan2(m_vel.x, -m_vel.y) : 0;
}
void Bullet::make_sparks(vec2 const& pos) {
    for (int i = 0; i < 10; ++i) {
        m_world.spawn_particle<BulletParticle>(pos, m_desc.spark_color);
    }
}
bool Bullet::update() {
    ++m_tick;
    for (int i = 0; i < 2; ++i) {
        m_pos += m_vel * 0.5f;
        if (std::abs(m_pos.x) > 124 || std::abs(m_pos.y) > 80) {
            return false;
        }

        // collision with wall
        transform_points(m_polygon, m_desc.polygon, m_pos, m_ang);
        CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
        if (info.distance > 0) {
            make_sparks(info.where);
            return false;
        }

        // collision with shock wave
        if (m_world.get_shock_wave().overlap(m_polygon)) {
            make_sparks(m_pos);
            return false;
        }

        Player& player = m_world.get_player();
        if (player.is_alive()) {

            // collision with player
            if (!player.is_invincible()) {
                CollisionInfo info;
                if (player.is_field_active()) {
                    info = polygon_collision(m_polygon, player.get_field_polygon());
                }
                else {
                    info = polygon_collision(m_polygon, player.get_polygon());
                }
                if (info.distance > 0) {
                    player.hit();
                    make_sparks(info.where);
                    return false;
                }
            }

            // collision with balls
            for (Ball& b : player.get_balls()) {
                if (!b.is_alive()) continue;
                CollisionInfo info;
                if (player.is_field_active()) {
                    info = polygon_collision(m_polygon, b.get_field_polygon());
                }
                else {
                    info = polygon_collision(m_polygon, b.get_polygon());
                }
                if (info.distance > 0) {
                    b.hit();
                    make_sparks(info.where);
                    return false;
                }
            }
        }
    }

    return true;
}

void Bullet::draw(SpriteRenderer& ren) const {
    ren.draw(frame(m_desc.sprite, m_tick / m_desc.frame_length % frame_count(m_desc.sprite)), m_pos, m_ang);
}
