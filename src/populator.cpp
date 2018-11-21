#include "populator.hpp"
#include "world.hpp"
#include "square_enemy.hpp"
#include "ring_enemy.hpp"
#include "twister_enemy.hpp"
#include "cannon_enemy.hpp"
#include "rocket_enemy.hpp"
#include "spider_enemy.hpp"
#include "blockade_enemy.hpp"


Populator::Populator(World& world) : m_world(world), m_spawn_groups({
        { 30, false, [this](vec2 const& p, Footing  ){ m_world.spawn_enemy<SquareEnemy>(p); } },
        { 20, false, [this](vec2 const& p, Footing  ){ m_world.spawn_enemy<RingEnemy>(p); } },
        {  7, false, [this](vec2 const& p, Footing  ){ m_world.spawn_chain<TwisterEnemyChain>(p); } },
        { 20, true,  [this](vec2 const& p, Footing f){ m_world.spawn_enemy<CannonEnemy>(p, f); } },
        { 20, true,  [this](vec2 const& p, Footing f){ m_world.spawn_enemy<RocketEnemy>(p, f); } },
        { 20, true,  [this](vec2 const& p, Footing f){ m_world.spawn_enemy<SpiderEnemy>(p, f); } },
    })
{
    std::vector<int> weights;
    for (SpawnGroup const& g : m_spawn_groups) weights.push_back(g.weight);
    m_spawn_dist = std::discrete_distribution<int>(weights.begin(), weights.end());
}

void Populator::reset(uint32_t seed) {
    m_random.seed(seed);
    m_tick           = 0;
    m_wall_row_count = 0;
    m_spots.clear();
    m_wall_spots.clear();

    m_blockades = 0;
}

enum {
    Y = 27
};

void Populator::next_wall_row() {
    m_spots.clear();
    m_wall_spots.clear();

    auto const& data = m_world.get_wall().get_data();

    // blockade
    ++m_wall_row_count;
    if (m_wall_row_count >= 30) {
        m_wall_row_count = 0;
        ++m_blockades;
    }
    if (m_blockades > 0) {
        // check whether line is suitable for blockade
        int x;
        for (x = 1; x < Wall::W; ++x) {
            if (data[Y][x] != 0) continue;
            if (data[Y][x - 1] > 1 || data[Y][x + 1] > 1) break;
        }
        if (x == Wall::W) {
            --m_blockades;
            // set up blockade
            BlockadeEnemy* prev = nullptr;
            for (x = 1; x < Wall::W; ++x) {
                if (data[Y][x] == 0) {
                    vec2 p = m_world.get_wall().get_tile_position(x, Y) - vec2(0, Wall::SPEED);
                    BlockadeEnemy* b = m_world.spawn_enemy<BlockadeEnemy>(p);
                    if (prev) b->link(prev);
                    prev = b;
                }
                else {
                    prev = nullptr;
                }
            }

            // don't collect spots
            return;
        }
    }

    // collect spots
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
    int i = m_random.get_int(0, m_spots.size() - 1);
    s = m_spots[i];
    m_spots.erase(m_spots.begin() + i);
    return true;
}

bool Populator::get_random_wall_spot(Spot& s) {
    if (m_wall_spots.empty()) return false;
    int i = m_random.get_int(0, m_wall_spots.size() - 1);
    s = m_wall_spots[i];
    m_wall_spots.erase(m_wall_spots.begin() + i);
    return true;
}

vec2 Populator::get_spot_pos(Spot s) const {
    return m_world.get_wall().get_tile_position(s.x, Y);
}

void Populator::update() {
    ++m_tick;

    if (m_tick % 70 == 0) {
        SpawnGroup const& g = m_spawn_groups[m_random.get(m_spawn_dist)];
        Spot s;
        if (!g.needs_wall && !get_random_spot(s)) return;
        if (g.needs_wall && !get_random_wall_spot(s)) return;
        g.spawn_func(get_spot_pos(s), s.footing);
    }
}
