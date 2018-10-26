#include "player.hpp"
#include "world.hpp"


namespace {
    constexpr std::array<vec2, 6> PLAYER_POLYGON = {
        vec2{4, 4},
        vec2{4, 0},
        vec2{1, -3},
        vec2{-1, -3},
        vec2{-4, 0},
        vec2{-4, 4},
    };
    constexpr std::array<vec2, 8> FIELD_POLYGON = {
        vec2{4, 6},
        vec2{6, 4},
        vec2{6, 0},
        vec2{1, -5},
        vec2{-1, -5},
        vec2{-6, 0},
        vec2{-6, 4},
        vec2{-4, 6},
    };
    constexpr std::array<vec2, 4> LASER_POLYGON = {
        vec2{-0.5, 2.5},
        vec2{0.5, 2.5},
        vec2{0.5, -2.5},
        vec2{-0.5, -2.5},
    };

    class LaserSparkParticle : public SparkParticle {
    public:
        LaserSparkParticle(vec2 const& pos) {
            m_color    = Color(0, 155, 155, 200);
            m_friction = 0.7;
            m_layer    = FRONT;
            m_pos      = pos;
            float ang  = rnd.get_float(0, M_PI * 2);
            m_vel      = vec2(std::sin(ang), std::cos(ang)) * rnd.get_float(0.5, 1);
            m_ttl      = rnd.get_int(3, 7);
        }
    };
}


void Player::reset() {
    m_tick             = 0;
    m_pos              = { 0, 40 };
    m_blast_vel        = {};
    m_blast_delay      = 0;
    m_shoot_delay      = 0;
    m_side_shot        = false;

    m_shoot_period     = 15; // 10
    m_alive            = true;
    m_shield           = 3;
    m_invincible_delay = 0;
    m_score            = 0;
}


void Player::hit(CollisionInfo const& info) {
    // blast
    if (info.distance > 0) {
        m_pos += info.normal * info.distance;
        transform(m_polygon, PLAYER_POLYGON, m_pos);

        m_blast_vel = info.normal * 1.5f;
        m_blast_delay = 15;

    }

    // damage
    if (!is_invincible()) {
        m_invincible_delay = 120;
        if (--m_shield <= 0) {
            // game over
            m_alive = false;
        }
    }
}


void Player::update(Input const& input) {
    if (!m_alive) return;

    ++m_tick;
    m_blast_vel *= 0.87f;

    float speed = 0;
    if (m_blast_delay > 0) {
        --m_blast_delay;
    }
    else {
        speed = 0.75;
        if (input.a || m_shoot_delay > 0) speed *= 0.5;
    }

    // move
    m_pos += m_blast_vel + vec2(input.dx, input.dy) * speed;
    m_pos = clamp(m_pos, { -124, -72 }, { 124, 71 });

    // collision with wall
    transform(m_polygon, PLAYER_POLYGON, m_pos);
    CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
    if (info.distance > 0) hit(info);


    // collision with enemies
    for (auto& e : m_world.get_enemies()) {
        CollisionInfo info = polygon_collision(m_polygon, e->get_polygon());
        if (info.distance > 0) {
            hit(info);
            e->hit(2);
        }
    }


    // shoot
    if (input.dy > 0) m_side_shot = false;
    if (input.dy < 0) m_side_shot = true;
    if (m_shoot_delay > 0) --m_shoot_delay;
    if (input.a && m_shoot_delay == 0 && m_blast_delay == 0) {
        m_shoot_delay = m_shoot_period;

        m_world.spawn_laser(m_pos - vec2(0, 1), {0, -1});
    }


    if (m_invincible_delay > 0) --m_invincible_delay;
}


void Player::draw(SpriteRenderer& ren) const {
    if (!m_alive) return;
	if (m_invincible_delay % 8 >= 4) return;

    ren.draw(frame(Sprite::PLAYER, m_tick / 8 % 2), m_pos);

    // debug
//    DB_REN.set_color(255, 0, 0);
//    DB_REN.polygon(m_polygon);
}


Laser::Laser(World& world, vec2 const& pos, vec2 const& vel)
    : m_world(world), m_pos(pos), m_vel(vel)
{
    m_ang = std::atan2(m_vel.x, m_vel.y);
}

bool Laser::update() {
    for (int i = 0; i < 4; ++i) {
        m_pos += m_vel;
        if (std::abs(m_pos.x) > 124 || std::abs(m_pos.y) > 80)  return false;
        transform(m_polygon, LASER_POLYGON, m_pos, m_ang);

        // collision with wall
        CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
        if (info.distance > 0) {
            for (int i = 0; i < 10; ++i) {
                m_world.spawn_particle<LaserSparkParticle>(info.where);
            }
            return false;
        }

        // collision with enemies
        for (auto& e : m_world.get_enemies()) {
            CollisionInfo info = polygon_collision(m_polygon, e->get_polygon());
            if (info.distance > 0) {
                e->hit(2); // damage
                for (int i = 0; i < 10; ++i) {
                    m_world.spawn_particle<LaserSparkParticle>(info.where);
                }
                return false;
            }
        }
    }
    return true;
}

void Laser::draw(SpriteRenderer& ren) const {
    ren.push();
    ren.translate(m_pos);
    ren.rotate(m_ang);
    ren.draw(frame(Sprite::LASER));
    ren.pop();

    // debug
//    DB_REN.set_color(255, 0, 0);
//    DB_REN.polygon(m_polygon);
}
