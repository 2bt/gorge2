#include "log.hpp"
#include "populator.hpp"
#include "world.hpp"
#include "square_enemy.hpp"
#include "ring_enemy.hpp"
#include "twister_enemy.hpp"
#include "cannon_enemy.hpp"
#include "rocket_enemy.hpp"
#include "spider_enemy.hpp"
#include "saucer_enemy.hpp"
#include "blockade_enemy.hpp"



void Populator::reset(uint32_t seed) {
    m_random.seed(seed);
    m_tick           = 0;
    m_wall_row_count = 0;
    m_blockades_todo = 0;
    m_spots.clear();
    m_wall_spots.clear();
    m_spawn_groups.clear();
    m_spawn_rate = 0;
}

enum {
    Y = 27
};

void Populator::next_wall_row() {
    m_spots.clear();
    m_wall_spots.clear();

    auto const& data = m_world.get_wall().get_data();

    // blockade
    if (++m_wall_row_count % 50 == 0) {
        ++m_blockades_todo;
    }
    if (m_blockades_todo > 0) {
        // check whether line is suitable for blockade
        int x;
        for (x = 1; x < Wall::W; ++x) {
            if (data[Y][x] != 0) continue;
            if (data[Y][x - 1] > 1 || data[Y][x + 1] > 1) break;
        }
        if (x == Wall::W) {
            --m_blockades_todo;
            // set up blockade
            BlockadeEnemy* prev = nullptr;
            for (x = 1; x < Wall::W; ++x) {
                if (data[Y][x] == 0) {
                    vec2 p = m_world.get_wall().get_tile_position(x, Y) - vec2(0, Wall::SPEED);
                    BlockadeEnemy* b = m_world.spawn_enemy<BlockadeEnemy>(p, prev);
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

    if (m_tick % 60 == 0)
    switch (m_tick / 60) {
    case 0:
        m_spawn_rate = 70;
        m_spawn_groups = {
            { 20, false, [this](vec2 const& pos, float ){ m_world.spawn_enemy<RingEnemy>(pos); } },
        };
        break;

    case 10:
        m_spawn_rate = 120;
        m_spawn_groups.push_back(
            { 7, false, [this](vec2 const& pos, float ){ m_world.spawn_particle<TwisterEnemyChain>(m_world, pos); } }
        );
        break;

    case 17:
        m_spawn_groups = {};
        break;

    case 23:
        m_spawn_groups = {
            { 20, false, [this](vec2 const& pos, float    ){ m_world.spawn_enemy<RingEnemy>(pos); } },
            {  7, false, [this](vec2 const& pos, float    ){ m_world.spawn_particle<TwisterEnemyChain>(m_world, pos); } },
            { 10, false, [this](vec2 const& pos, float    ){ m_world.spawn_enemy<SquareEnemy>(pos); } },
        };
        break;

    case 40:
        m_spawn_groups = {
            { 20, false, [this](vec2 const& pos, float    ){ m_world.spawn_enemy<RingEnemy>(pos); } },
            {  7, false, [this](vec2 const& pos, float    ){ m_world.spawn_particle<TwisterEnemyChain>(m_world, pos); } },
            { 20, false, [this](vec2 const& pos, float    ){ m_world.spawn_enemy<SquareEnemy>(pos); } },
            { 20, true,  [this](vec2 const& pos, float ang){ m_world.spawn_enemy<CannonEnemy>(pos, ang); } },
            { 20, true,  [this](vec2 const& pos, float ang){ m_world.spawn_enemy<RocketEnemy>(pos, ang); } },
        };
        break;

    case 90:
        m_spawn_groups = {
            { 20, false, [this](vec2 const& pos, float    ){ m_world.spawn_enemy<RingEnemy>(pos); } },
            {  7, false, [this](vec2 const& pos, float    ){ m_world.spawn_particle<TwisterEnemyChain>(m_world, pos); } },
            { 20, false, [this](vec2 const& pos, float    ){ m_world.spawn_enemy<SquareEnemy>(pos); } },
            { 20, true,  [this](vec2 const& pos, float ang){ m_world.spawn_enemy<CannonEnemy>(pos, ang); } },
            { 20, true,  [this](vec2 const& pos, float ang){ m_world.spawn_enemy<RocketEnemy>(pos, ang); } },
            { 20, true,  [this](vec2 const& pos, float ang){ m_world.spawn_enemy<SpiderEnemy>(pos, ang); } },
        };
        break;

    case 120:
        m_spawn_rate = 100;
        break;

    case 160:
        m_spawn_groups.push_back({ 1, false, [this](vec2 const& pos, float){ m_world.spawn_enemy<SaucerEnemy>(pos); } });
        LOGI("Populator::update: start spawning SaucerEnemy");
        break;

    default: break;
    }



    if (m_tick % m_spawn_rate == 0) {
        std::vector<int> weights;
        for (SpawnGroup const& g : m_spawn_groups) weights.push_back(g.weight);
        auto dist = std::discrete_distribution<int>(weights.begin(), weights.end());
        SpawnGroup const& g = m_spawn_groups[m_random.get(dist)];
        Spot s;
        if (!g.needs_wall && !get_random_spot(s)) return;
        if (g.needs_wall && !get_random_wall_spot(s)) return;

        static constexpr std::array<float, 5> ANGLES = {
            0, // F_NONE
            M_PI,
            M_PI / 2,
            0,
            -M_PI / 2,
        };
        g.spawn_func(get_spot_pos(s), ANGLES[s.footing]);
    }


    ++m_tick;
}
