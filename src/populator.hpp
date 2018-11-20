#pragma once
#include "util.hpp"
#include "enemy.hpp"
#include <functional>


class World;


class Populator {
public:
    Populator(World& world);
    void reset(uint32_t seed);
    void next_wall_row();
    void update();

protected:

    struct Spot {
        int x;
        Footing footing;
    };

    bool get_random_spot(Spot& s);
    bool get_random_wall_spot(Spot& s);
    vec2 get_spot_pos(Spot s) const;

    World&            m_world;
    Random            m_random;
    int               m_tick;
    int               m_wall_row_count;

    std::vector<Spot> m_spots;
    std::vector<Spot> m_wall_spots;

    struct SpawnGroup {
        int  weight;
        bool needs_wall;
        std::function<void(vec2 const&, Footing)> spawn_func;
    };
    const std::vector<SpawnGroup>   m_spawn_groups;
    std::discrete_distribution<int> m_spawn_dist;
};
