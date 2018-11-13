#pragma once
#include "util.hpp"
#include "enemy.hpp"


class World;


class Populator {
public:
    Populator(World& world) : m_world(world) {}
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

};
