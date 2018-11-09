#pragma once
#include "sprite.hpp"
#include "util.hpp"
#include <array>

class Wall {
public:
    static constexpr float SPEED = 0.25;

    void reset(uint32_t seed);
    void update();
    void draw(SpriteRenderer& ren);

    CollisionInfo check_collision(vec2 const* polygon, int len) const;
    template<class T>
    CollisionInfo check_collision(T const& polygon) const {
        return check_collision(polygon.data(), polygon.size());
    }

    bool check_sight(vec2 const& a, vec2 const& b) const;

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

    Grid<30, W> m_data;
    Grid<H, W>  m_gen_data;

    float       m_radius;
    vec2        m_cursor;
    int         m_island_delay;
};
