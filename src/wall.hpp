#pragma once
#include "sprite.hpp"
#include "util.hpp"
#include "random.hpp"
#include <array>

class Wall {
public:

    void reset(uint32_t seed);
    void update();
    void draw(SpriteRenderer& ren);

private:
    void generate();

    enum {
        W = 38,
        H = 38
    };

    template<size_t H, size_t W>
    using Grid = std::array<std::array<int, W>, H>;

    Random      m_random;
    int         m_row_counter;
    float       m_offset;
    Grid<30, W> m_data;
    Grid<H, W>  m_gen_data;

    float       m_radius;
    vec2        m_cursor;
    int         m_island_delay;
};
