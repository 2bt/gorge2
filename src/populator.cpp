#include "populator.hpp"
#include "world.hpp"
#include "square_enemy.hpp"
#include "ring_enemy.hpp"
#include "cannon_enemy.hpp"
#include "rocket_enemy.hpp"
#include "spider_enemy.hpp"


void Populator::reset(uint32_t seed) {
    m_random.seed(seed);
    m_tick           = 0;
    m_wall_row_count = 0;
    m_spots.clear();
    m_wall_spots.clear();
}

enum {
    Y = 27
};

void Populator::next_wall_row() {
    ++m_wall_row_count;

    m_spots.clear();
    m_wall_spots.clear();

    auto const& data = m_world.get_wall().get_data();

    for (int x = 1; x < Wall::W - 1; ++x) {
        if (data[Y][x] != 0) continue;
        m_spots.push_back({ x });
        if (data[Y + 1][x] == 1) m_wall_spots.push_back({ x, F_NORTH });
        if (data[Y][x - 1] == 1) m_wall_spots.push_back({ x, F_WEST  });
        if (data[Y - 1][x] == 1) m_wall_spots.push_back({ x, F_SOUTH });
        if (data[Y][x + 1] == 1) m_wall_spots.push_back({ x, F_EAST  });
    }
}

bool Populator::get_random_spot(Spot& s) {
    if (m_spots.empty()) return false;
    s = m_spots[m_random.get_int(0, m_spots.size() - 1)];
    return true;
}

bool Populator::get_random_wall_spot(Spot& s) {
    if (m_wall_spots.empty()) return false;
    s = m_wall_spots[m_random.get_int(0, m_wall_spots.size() - 1)];
    return true;
}
vec2 Populator::get_spot_pos(Spot s) const {
    return m_world.get_wall().get_tile_position(s.x, Y);
}

void Populator::update() {
    ++m_tick;

    // spawn enemy
    if (m_tick % 70 == 0) {
        Spot s;
        int i = m_random.get_int(0, 4);
        if (i < 2) {
            if (!get_random_spot(s)) return;
            vec2 p = get_spot_pos(s);
            if (i == 0)      m_world.spawn_enemy<SquareEnemy>(p);
            else if (i == 1) m_world.spawn_enemy<RingEnemy>(p);
        }
        else {
            if (!get_random_wall_spot(s)) return;
            vec2 p = get_spot_pos(s);
            if (i == 2)      m_world.spawn_enemy<CannonEnemy>(p, s.footing);
            else if (i == 3) m_world.spawn_enemy<RocketEnemy>(p, s.footing);
            else if (i == 4) m_world.spawn_enemy<SpiderEnemy>(p, s.footing);
        }
    }
}
