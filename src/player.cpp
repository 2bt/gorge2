#include "player.hpp"
#include "sprite.hpp"
#include "debug_renderer.hpp"
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

};


void Player::reset() {
    m_tick        = 0;
    m_shild       = 3;
    m_pos         = { 0, 40 };
    m_blast_vel   = {};
    m_shoot_delay = 0;
}


void Player::update(Input const& input) {
    ++m_tick;
    m_blast_vel *= 0.85f;

    float speed = 0.75;
    if (input.a || m_shoot_delay > 0) speed *= 0.5;

    // move
    m_pos += m_blast_vel + vec2(input.dx, input.dy) * speed;

    transform(m_polygon, PLAYER_POLYGON, m_pos);

    // collision
    auto info = m_world.get_wall().check_collision(m_polygon);
    if (info.distance > 0) {
        

    }


    // shoot
}


void Player::draw(SpriteRenderer& ren) {


    ren.draw(frame(Sprite::PLAYER, m_tick / 8 % 2), m_pos);


    // debug
    DB_REN.set_color(255, 0, 0);
    DB_REN.polygon(m_polygon);
}
