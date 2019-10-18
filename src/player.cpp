#include "log.hpp"
#include "player.hpp"
#include "world.hpp"
#include "debug_renderer.hpp"
#include "resource.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>


namespace {

    std::array<vec2, 4> BALL_POLYGON = {
        vec2{ 2.5, 2.5},
        vec2{2.5, -2.5},
        vec2{-2.5, -2.5},
        vec2{ -2.5, 2.5},
    };
    std::array<vec2, 8> BALL_FIELD_POLYGON = {
        vec2{2, 4},
        vec2{4, 2},
        vec2{4, -2},
        vec2{2, -4},
        vec2{-2, -4},
        vec2{-4, -2},
        vec2{-4, 2},
        vec2{-2, 4}
    };

    std::array<vec2, 6> PLAYER_POLYGON = {
        vec2{4, 4},
        vec2{4, 0},
        vec2{1, -3},
        vec2{-1, -3},
        vec2{-4, 0},
        vec2{-4, 4},
    };
    std::array<vec2, 8> PLAYER_FIELD_POLYGON = {
        vec2{4, 6},
        vec2{6, 4},
        vec2{6, 0},
        vec2{1, -5},
        vec2{-1, -5},
        vec2{-6, 0},
        vec2{-6, 4},
        vec2{-4, 6},
    };
    std::array<vec2, 4> LASER_POLYGON = {
        vec2{-0.5, 2.5},
        vec2{0.5, 2.5},
        vec2{0.5, -2.5},
        vec2{-0.5, -2.5},
    };
    std::array<vec2, 4> SMALL_LASER_POLYGON = {
        vec2{-0.5, 1.25},
        vec2{0.5, 1.25},
        vec2{0.5, -1.25},
        vec2{-0.5, -1.25},
    };

    class LaserParticle : public Particle {
    public:
        LaserParticle(vec2 const& pos) {
            m_friction = 0.7;
            m_layer    = FRONT;
            m_pos      = pos;
            float ang  = rnd.get_float(0, M_PI * 2);
            m_vel      = vec2(std::sin(ang), std::cos(ang)) * rnd.get_float(0.5, 1);
            m_ttl      = rnd.get_int(3, 7);
        }
        void draw(SpriteRenderer& ren) const override {
            ren.set_color({0, 155, 155, 200});
            ren.draw(frame(Sprite::SPARK), m_pos);
        }
    };
}


void ShockWave::reset() {
    m_alive = false;
}
void ShockWave::activate(vec2 const& pos) {
    m_alive  = true;
    m_pos    = pos;
    m_tick   = 0;
    m_level  = 0;
    m_radius = 0;
    update();
}
bool ShockWave::overlap(vec2 const* poly, int len) const {
    if (!m_alive) return false;
    for (int i = 0; i < len; ++i) {
        if (glm::distance2(poly[i], m_pos) < m_radius * m_radius) return true;
    }
    return false;
}
void ShockWave::update() {
    if (!m_alive) return;
    m_pos.y += Wall::SPEED;
    ++m_tick;
    if (m_tick > 56) m_alive = false;
    float level = m_tick * 0.018;
    m_radius = (1 - std::pow(2, (level * -5))) * 40;
}
void ShockWave::draw(SpriteRenderer& ren) const {
    if (!m_alive) return;

    ren.push_state();
    ren.set_texture(resource::texture(resource::TID_SHOCKWAVE));
    ren.set_color();
    ren.draw({{m_tick % 8 * 80, m_tick / 8 * 80}, {80, 80}}, m_pos);
    ren.pop_state();
}


Ball::Ball(World& world, int dir)
    : m_world(world), m_dir(dir), m_offset(7 * m_dir, 2)
{}
void Ball::reset() {
    m_alive = false;
}
void Ball::activate() {
    if (m_alive) return;
    m_alive = true;
    m_pos   = m_world.get_player().get_pos();
    m_glide = 0;
}
void Ball::shoot(bool side_shot) {
    if (!m_alive) return;
    vec2 pos, vel;
    if (side_shot) {
        pos = m_pos - vec2(4 * m_dir, 0);
        vel = {m_dir, -0.1};
    }
    else {
        pos = m_pos + vec2(0, 4);
        vel = {0.1 * m_dir, -1};
    }
    m_world.spawn_laser(std::make_unique<Laser>(m_world, pos, vel, true));
}
void Ball::hit(CollisionInfo const& info) {
    if (m_world.get_player().is_field_active() && info.distance == 0) return;
    assert(m_alive);
    m_alive = false;
    make_small_explosion(m_world, m_pos, true);
}
void Ball::update() {
    if (!m_alive) return;
    m_glide = std::min(m_glide + 0.02f, 0.3f);
    m_pos += (m_world.get_player().get_pos() + m_offset - m_pos) * m_glide;
    transform_points(m_polygon, BALL_POLYGON, m_pos);
    transform_points(m_field_polygon, BALL_FIELD_POLYGON, m_pos);

    // collision with wall
    CollisionInfo info = m_world.get_wall().check_collision(m_polygon, true);
    if (info.distance > 0) hit(info);

    // collision with enemies
    for (auto& e : m_world.get_enemies()) {
        CollisionInfo info = polygon_collision(m_polygon, e->get_polygon());
        if (info.distance > 0) {
            hit(info);
            e->hit(2);
        }
    }
}
void Ball::draw(SpriteRenderer& ren) const {
    int tick = m_world.get_player().get_tick();
    if (!m_alive) return;
    ren.push();
    ren.translate(m_pos);
    ren.scale({m_dir, 1});
    ren.draw(frame(Sprite::BALL, tick / 4 % frame_count(Sprite::BALL)));
    ren.pop();
}
void Ball::draw_field(SpriteRenderer& ren) const {
    int tick = m_world.get_player().get_tick();
    if (!m_alive) return;
    int f1 = tick / 4 % frame_count(Sprite::BALL_FIELD);
    int f2 = (tick + 3) / 4 % frame_count(Sprite::BALL_FIELD);
    ren.set_color({0, 255, 255, 50});
    ren.draw(frame(Sprite::BALL_FIELD, f1), m_pos);
    ren.set_color({100 + 100 * std::sin(tick / 2), 255, 255, 50});
    ren.draw(frame(Sprite::BALL_FIELD, f2), m_pos);
}


void Player::reset() {
    m_tick             = 0;
    m_pos              = { 0, 40 };
    m_blast_vel        = {};
    m_blast_delay      = 0;
    m_shoot_delay      = 0;
    m_side_shot        = false;
    m_field_active     = false;
    m_speed_boost      = 0;
    m_invincible_delay = 0;
    m_old_input_b      = false;

    m_shoot_period     = 12;
    m_alive            = true;
    m_shield           = MAX_SHIELD;
    m_score            = 0;
    m_energy           = 0;

    m_balls[0].reset();
    m_balls[1].reset();
}

void Player::hit(CollisionInfo const& info) {
    if (m_field_active && info.distance == 0) return;
    // blast
    if (info.distance > 0) {
        m_pos += info.normal * info.distance;
        transform_points(m_polygon, PLAYER_POLYGON, m_pos);
        m_blast_vel = info.normal * 1.5f;
        m_blast_delay = 15;
        make_small_explosion(m_world, info.where, false);
    }

    // damage
    if (!is_invincible()) {
        m_invincible_delay = 120;
        if (--m_shield <= 0) {
            // game over
            make_explosion(m_world, m_pos);
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
        speed = 0.75 + std::pow(m_speed_boost, 0.8f) * 0.1;
        if (input.a || m_shoot_delay > 0) speed *= 0.5;
    }

    // move
    m_pos += m_blast_vel + input.mov * speed;
    m_pos = clamp(m_pos, { -124, -72 }, { 124, 71 });

    // collision with wall
    transform_points(m_polygon, PLAYER_POLYGON, m_pos);
    CollisionInfo info = m_world.get_wall().check_collision(m_polygon, true);
    if (info.distance > 0) {
        hit(info);
        // instant death
        if (m_pos.y > 71) {
            m_invincible_delay = 0;
            m_shield = 0;
            hit();
        }
    }

    // collision with enemies
    for (auto& e : m_world.get_enemies()) {
        CollisionInfo info = polygon_collision(m_polygon, e->get_polygon());
        if (info.distance > 0) {
            hit(info);
            e->hit(2);
        }
    }

    // update balls
    m_balls[0].update();
    m_balls[1].update();

    // shoot
    if (input.mov.y > 0) m_side_shot = false;
    if (input.mov.y < 0) m_side_shot = true;
    if (m_shoot_delay > 0) --m_shoot_delay;
    if (input.a && m_shoot_delay == 0 && m_blast_delay == 0) {
        m_shoot_delay = m_shoot_period;
        m_world.spawn_laser(std::make_unique<Laser>(m_world, m_pos - vec2(0, 1), vec2(0, -1), false));
        m_balls[0].shoot(m_side_shot);
        m_balls[1].shoot(m_side_shot);
    }

    // field
    if (m_field_active) {
        m_energy -= 0.075;
        if (m_energy < 0) {
            m_energy = 0;
            m_field_active = false;
        }
        if (input.b && !m_old_input_b) {
            // BLAST
            m_energy = 0;
            m_field_active = false;
            m_world.get_shock_wave().activate(m_pos);
        }
    }
    else if (input.b && m_energy >= MAX_ENERGY) {
        m_field_active = true;
    }
    if (m_field_active) {
        transform_points(m_field_polygon, PLAYER_FIELD_POLYGON, m_pos);
    }

    m_old_input_b = input.b;


    if (m_invincible_delay > 0) --m_invincible_delay;
}


void Player::draw(SpriteRenderer& ren) const {
    if (!m_alive) return;

    if (m_field_active) {
        m_balls[0].draw_field(ren);
        m_balls[1].draw_field(ren);
        int f1 = m_tick / 4 % frame_count(Sprite::FIELD);
        int f2 = (m_tick + 3) / 4 % frame_count(Sprite::FIELD);
        ren.set_color({0, 255, 255, 50});
        ren.draw(frame(Sprite::FIELD, f1), m_pos);
        ren.set_color({100 + 100 * std::sin(m_tick / 2), 255, 255, 50});
        ren.draw(frame(Sprite::FIELD, f2), m_pos);
    }

    ren.set_color();
    m_balls[0].draw(ren);
    m_balls[1].draw(ren);

	if (m_invincible_delay % 8 >= 4) return;
    ren.draw(frame(Sprite::PLAYER, m_tick / 8 % 2), m_pos);

    // debug
//    DB_REN.set_color(255, 0, 0);
//    DB_REN.polygon(m_polygon);
//    DB_REN.polygon(m_field_polygon);
}


Laser::Laser(World& world, vec2 const& pos, vec2 const& vel, bool is_small)
    : m_world(world)
    , m_pos(pos)
    , m_vel(vel)
    , m_is_small(is_small)
{
    m_ang = std::atan2(m_vel.x, -m_vel.y);
}

bool Laser::update() {
    for (int i = 0; i < 4; ++i) {
        m_pos += m_vel;
        if (std::abs(m_pos.x) > 124 || std::abs(m_pos.y) > 80)  return false;
        transform_points(m_polygon, m_is_small ? SMALL_LASER_POLYGON : LASER_POLYGON, m_pos, m_ang);

        // collision with wall
        CollisionInfo info = m_world.get_wall().check_collision(m_polygon, true);
        if (info.distance > 0) {
            for (int i = 0; i < 10; ++i) {
                m_world.spawn_particle<LaserParticle>(info.where);
            }
            return false;
        }

        // collision with enemies
        for (auto& e : m_world.get_enemies()) {
            CollisionInfo info = polygon_collision(m_polygon, e->get_polygon());
            if (info.distance > 0) {
                e->hit(m_is_small ? 1 : 2); // damage
                for (int i = 0; i < 10; ++i) {
                    m_world.spawn_particle<LaserParticle>(info.where);
                }
                return false;
            }
        }
    }
    return true;
}

void Laser::draw(SpriteRenderer& ren) const {
    ren.draw(frame(m_is_small ? Sprite::SMALL_LASER : Sprite::LASER), m_pos, m_ang);

    // debug
//    DB_REN.set_color(255, 0, 0);
//    DB_REN.polygon(m_polygon);
}
