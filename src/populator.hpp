#pragma once
#include "util.hpp"


class World;


class Populator {
public:
    Populator(World& world) : m_world(world) {}
    void reset(uint32_t seed);
    void update();

protected:
    World&            m_world;
    Random            m_random;
    int               m_tick;
};
