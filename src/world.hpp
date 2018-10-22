#pragma once
#include "background.hpp"
#include "wall.hpp"
#include "player.hpp"
#include "particle.hpp"
#include "enemy.hpp"
#include <memory>


class World {
public:

    void init();
    void free();

    void reset(uint32_t seed);
    void update();
    void draw(SpriteRenderer& ren);


    Wall const& get_wall() const { return m_wall; }
    void spawn_laser(vec2 const& pos, vec2 const& vel);

    void spawn_particle(std::unique_ptr<Particle> p);

    void spawn_enemy(std::unique_ptr<Enemy> e);
    template<class T, typename... Args>
    void spawn_enemy(Args&&... args) {
        spawn_enemy(std::make_unique<T>(
            *this,
            m_random.get_int(0, 0x7fffffff),
            std::forward<Args>(args)...));
    }



private:
    Random                                 m_random;
    Background                             m_background;
    Wall                                   m_wall;
    Player                                 m_player{*this};

    std::vector<std::unique_ptr<Laser>>    m_lasers;
    std::vector<std::unique_ptr<Enemy>>    m_enemies;
    std::vector<std::unique_ptr<Particle>> m_particles;
};
