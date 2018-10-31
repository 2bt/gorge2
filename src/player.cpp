#include "player.hpp"
#include "world.hpp"


namespace {

    constexpr std::array<vec2, 4> BALL_POLYGON = {
        vec2{ 2.5, 2.5},
        vec2{2.5, -2.5},
        vec2{-2.5, -2.5},
        vec2{ -2.5, 2.5},
    };

    constexpr std::array<vec2, 6> PLAYER_POLYGON = {
        vec2{4, 4},
        vec2{4, 0},
        vec2{1, -3},
        vec2{-1, -3},
        vec2{-4, 0},
        vec2{-4, 4},
    };
    constexpr std::array<vec2, 8> PLAYER_FIELD_POLYGON = {
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
    constexpr std::array<vec2, 4> SMALL_LASER_POLYGON = {
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


Ball::Ball(World& world, int dir)
    : m_world(world), m_dir(dir), m_offset(7 * m_dir, 2)
{}
void Ball::reset() {
    m_alive = false;
}
void Ball::activate(vec2 const& player_pos, int player_tick) {
    if (m_alive) return;
    m_alive = true;
    m_pos   = player_pos;
    m_tick  = player_tick;
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
void Ball::hit() {
    assert(m_alive);
    m_alive = false;
    make_small_explosion(m_world, m_pos, true);
}
void Ball::update(vec2 const& player_pos) {
    if (!m_alive) return;
    ++m_tick;
    m_glide = std::min(m_glide + 0.02f, 0.3f);
    m_pos += (player_pos + m_offset - m_pos) * m_glide;
    transform(m_polygon, BALL_POLYGON, m_pos);

    // collision with wall
    CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
    if (info.distance > 0) hit();

    // collision with enemies
    for (auto& e : m_world.get_enemies()) {
        CollisionInfo info = polygon_collision(m_polygon, e->get_polygon());
        if (info.distance > 0) {
            hit();
            e->hit(2);
        }
    }
}
void Ball::draw(SpriteRenderer& ren) const {
    if (!m_alive) return;
    ren.push();
    ren.translate(m_pos);
    ren.scale({m_dir, 1});
    ren.draw(frame(Sprite::BALL, m_tick / 4 % frame_count(Sprite::BALL)));
    ren.pop();
}


void Player::reset() {
    m_tick             = 0;
    m_pos              = { 0, 40 };
    m_blast_vel        = {};
    m_blast_delay      = 0;
    m_shoot_delay      = 0;
    m_side_shot        = false;
    m_invincible_delay = 0;

    m_shoot_period     = 10; // 10
    m_alive            = true;
    m_shield           = 3;
    m_score            = 0;
    m_energy           = 0;

    m_balls[0].reset();
    m_balls[1].reset();
}

void Player::activate_balls() {
    m_balls[0].activate(m_pos, m_tick);
    m_balls[1].activate(m_pos, m_tick);
}

void Player::hit(CollisionInfo const& info) {
    // blast
    if (info.distance > 0) {
        m_pos += info.normal * info.distance;
        transform(m_polygon, PLAYER_POLYGON, m_pos);
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

void Player::update(fx::Input const& input) {
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
    m_pos += m_blast_vel + vec2(input.x, input.y) * speed;
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

    // update balls
    m_balls[0].update(m_pos);
    m_balls[1].update(m_pos);

    // shoot
    if (input.y > 0) m_side_shot = false;
    if (input.y < 0) m_side_shot = true;
    if (m_shoot_delay > 0) --m_shoot_delay;
    if (input.a && m_shoot_delay == 0 && m_blast_delay == 0) {
        m_shoot_delay = m_shoot_period;
        m_world.spawn_laser(std::make_unique<Laser>(m_world, m_pos - vec2(0, 1), vec2(0, -1), false));
        m_balls[0].shoot(m_side_shot);
        m_balls[1].shoot(m_side_shot);
    }


    if (m_invincible_delay > 0) --m_invincible_delay;
}


void Player::draw(SpriteRenderer& ren) const {
    if (!m_alive) return;

    ren.set_color();
    m_balls[0].draw(ren);
    m_balls[1].draw(ren);

	if (m_invincible_delay % 8 >= 4) return;
    ren.draw(frame(Sprite::PLAYER, m_tick / 8 % 2), m_pos);

    // debug
//    DB_REN.set_color(255, 0, 0);
//    DB_REN.polygon(m_polygon);
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
        transform(m_polygon, m_is_small ? SMALL_LASER_POLYGON : LASER_POLYGON, m_pos, m_ang);

        // collision with wall
        CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
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
    ren.push();
    ren.translate(m_pos);
    ren.rotate(m_ang);
    ren.draw(frame(m_is_small ? Sprite::SMALL_LASER : Sprite::LASER));
    ren.pop();

    // debug
//    DB_REN.set_color(255, 0, 0);
//    DB_REN.polygon(m_polygon);
}
