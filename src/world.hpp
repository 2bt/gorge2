#pragma once
#include "random.hpp"
#include "background.hpp"
#include "wall.hpp"
#include "player.hpp"
#include "particle.hpp"
#include <memory>


class World {
public:

    void init();
    void free();

    void reset(uint32_t seed);
    void update();
    void draw(SpriteRenderer& ren);


    Wall const& get_wall() const { return m_wall; }
    void make_laser(vec2 const& pos, vec2 const& vel);

    void add_particle(std::unique_ptr<Particle> p);


private:
    Random                                 m_random;
    Background                             m_background;
    Wall                                   m_wall;
    Player                                 m_player{*this};

    std::vector<std::unique_ptr<Laser>>    m_lasers;
    std::vector<std::unique_ptr<Particle>> m_particles;
};
