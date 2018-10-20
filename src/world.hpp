#pragma once
#include "random.hpp"
#include "background.hpp"
#include "wall.hpp"
#include "player.hpp"


class World {
public:

    void init();
    void free();

    void reset(uint32_t seed);
    void update();
    void draw(SpriteRenderer& ren);


    Wall const& get_wall() const { return m_wall; }



private:
    Random          m_random;
    Background      m_background;
    Wall            m_wall;
    Player          m_player{*this};
};
