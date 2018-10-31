#include "item.hpp"
#include "world.hpp"

namespace {

    constexpr std::array<vec2, 4> BIG_ITEM_POLYGON = {
        vec2{3, 3},
        vec2{3, -3},
        vec2{-3, -3},
        vec2{-3, 3},
    };

    constexpr std::array<vec2, 8> BIG_ITEM_BOUNCE_POLYGON = {
        vec2{6, 12},
        vec2{12, 6},
        vec2{12, -6},
        vec2{6, -12},
        vec2{-6, -12},
        vec2{-12, -6},
        vec2{-12, 6},
        vec2{-6, 12},
    };

    constexpr std::array<vec2, 4> ENERGY_ITEM_POLYGON = {
        vec2{2, 2},
        vec2{2, -2},
        vec2{-2, -2},
        vec2{-2, 2},
    };
}


void Item::draw(SpriteRenderer& ren) {
    ren.set_color();
    ren.draw(frame(m_sprite, m_tick / m_frame_length % frame_count(m_sprite)), m_pos);
}


bool EnergyItem::update() {
    ++m_tick;
    m_pos.y += m_world.get_wall().get_speed();
    m_pos += m_vel;
    m_vel *= 0.98f;
    if (m_pos.y > 80) return false;

    transform(m_polygon, ENERGY_ITEM_POLYGON, m_pos);
    CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
    if (info.distance > 0) {
        m_pos += info.normal * info.distance;
        transform(m_polygon, ENERGY_ITEM_POLYGON, m_pos);

        // reflect
        m_vel -= glm::dot(m_vel, info.normal) * info.normal * 2.0f;
    }

    Player& player = m_world.get_player();
    if (player.is_alive()) {

        // magnetics
        vec2 d = player.get_pos() - m_pos;
        float len = glm::length(d);
        if (len < 20)  m_pos += d * (1.25f / len);

        CollisionInfo info = polygon_collision(m_polygon, player.get_polygon());
        if (info.distance > 0) {
            make_sparkle(m_world, m_pos);
            player.inc_score(80);
            player.inc_energy();
            return false;
        }
    }

    return true;
}

void make_energy_items(World& world, Random& rnd, vec2 const& pos, int count) {
    for (int i = 0; i < count; ++i) {
        float ang = rnd.get_float(0, 2 * M_PI);
        vec2 vel = vec2(std::sin(ang), std::cos(ang)) * rnd.get_float(0.1, 0.625);
        world.spawn_item<EnergyItem>(pos, vel);
    }
}


bool BigItem::update() {
    ++m_tick;
    m_pos.y += m_world.get_wall().get_speed();
    m_pos += vec2(std::cos(m_tick * 0.1), std::sin(m_tick * 0.1)) * 0.25f;
    if (m_pos.y > 80) return false;

    transform(m_bounce_polygon, BIG_ITEM_BOUNCE_POLYGON, m_pos);
    CollisionInfo info = m_world.get_wall().check_collision(m_bounce_polygon);
    if (info.distance > 0) {
        m_pos += info.normal * 0.075f;
    }

    Player& player = m_world.get_player();
    if (player.is_alive()) {

        // magnetics
        vec2 d = player.get_pos() - m_pos;
        float len = glm::length(d);
        if (len < 15)  m_pos += d * (0.75f / len);

        transform(m_polygon, BIG_ITEM_POLYGON, m_pos);
        CollisionInfo info = polygon_collision(m_polygon, player.get_polygon());
        if (info.distance > 0) {
            make_sparkles(m_world, m_pos);
            player.inc_score(m_score);
            collect();
            return false;
        }
    }

    return true;
}


void BallItem::collect() {
    m_world.get_player().activate_balls();
}
