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

    float get_speed() const { return m_speed; };

    CollisionInfo check_collision(vec2 const* polygon, int len) const;
    template<class T>
    CollisionInfo check_collision(T const& polygon) const {
        return check_collision(polygon.data(), polygon.size());
    }

private:
    void generate();
    vec2 get_tile_position(int x, int y) const;
    glm::ivec2 get_tile_address(vec2 const& pos) const;

    enum {
        W = 38,
        H = 38
    };

    template<size_t H, size_t W>
    using Grid = std::array<std::array<int, W>, H>;

    Random      m_random;
    int         m_row_counter;
    float       m_offset;
    float       m_speed;

    Grid<30, W> m_data;
    Grid<H, W>  m_gen_data;

    float       m_radius;
    vec2        m_cursor;
    int         m_island_delay;
};
