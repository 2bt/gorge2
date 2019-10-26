#pragma once
#include "util.hpp"
#include "enemy.hpp"
#include <functional>


class World;


class Populator {
public:
    Populator(World& world) : m_world(world) {}
    void reset(uint32_t seed);
    void next_wall_row();
    void update();

protected:

    void spawn();

    enum Footing {
        F_NONE,
        F_NORTH,
        F_WEST,
        F_SOUTH,
        F_EAST,
    };

    struct Spot {
        int x;
        Footing footing = F_NONE;
    };

    bool get_random_spot(Spot& s);
    bool get_random_wall_spot(Spot& s);
    vec2 get_spot_pos(Spot s) const;

    World&            m_world;
    Random            m_random;
    int               m_tick;
    int               m_wall_row_count;
    int               m_blockades_todo;

    std::vector<Spot> m_spots;
    std::vector<Spot> m_wall_spots;

    struct SpawnGroup {
        int  weight;
        bool needs_wall;
        std::function<void(vec2 const&, float)> spawn_func;
    };
    std::vector<SpawnGroup> m_spawn_groups;
    int                     m_spawn_rate;
    int                     m_saucer_spawn_delay;
};
