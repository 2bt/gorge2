#include "player.hpp"
#include "sprite.hpp"


namespace {
	constexpr std::array<vec2, 6> PLAYER_POLYGON = {
        vec2{16, 16},
        vec2{16, 0},
        vec2{4, -12},
        vec2{-4, -12},
        vec2{-16, 0},
        vec2{-16, 16},
    };


	constexpr std::array<vec2, 8> FIELD_POLYGON = {
		vec2{16, 24},
        vec2{24, 16},
        vec2{24, 0},
        vec2{4, -20},
        vec2{-4, -20},
        vec2{-24, 0},
        vec2{-24, 16},
        vec2{-16, 24},
    };

};


std::vector<vec2> s_polygon = {

};

void Player::reset() {
    m_tick      = 0;
    m_shild     = 3;
    m_pos       = { 0, 40 };
    m_blast_vel = {};
}


void Player::update(Input const& input) {
    ++m_tick;
    m_blast_vel *= 0.85f;

    float speed = 0.75;
    if (input.a || m_shoot_delay > 0) speed *= 0.5;

    // move
    m_pos += m_blast_vel + vec2(input.dx, input.dy) * speed;


    // shoot
}


void Player::draw(SpriteRenderer& ren) {


    ren.draw(frame(Sprite::PLAYER, m_tick / 8 % 2), m_pos);
}
