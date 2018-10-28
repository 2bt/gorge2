#pragma once
#include "bump.hpp"
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
    void resized();

    void reset(uint32_t seed);
    void update();
    void draw(SpriteRenderer& ren);


    Bump& get_bump() { return m_bump; }
    Wall const& get_wall() const { return m_wall; }
    Player& get_player() { return m_player; }
    Player const& get_player() const { return m_player; }
    std::vector<std::unique_ptr<Enemy>> const& get_enemies() const { return m_enemies; }


    void spawn_laser(std::unique_ptr<Laser> l);
    void spawn_bullet(vec2 const& pos, vec2 const& vel, Bullet::Desc const& desc);
    void spawn_particle(std::unique_ptr<Particle> p);
    void spawn_enemy(std::unique_ptr<Enemy> e);

    template<class T, typename... Args>
    T* spawn_particle(Args&&... args) {
        auto unique = std::make_unique<T>(std::forward<Args>(args)...);
        auto raw = unique.get();
        spawn_particle(std::move(unique));
        return raw;
    }

    template<class T, typename... Args>
    T* spawn_enemy(Args&&... args) {
        auto unique = std::make_unique<T>(
            *this,
            m_random.get_int(0, 0x7fffffff),
            std::forward<Args>(args)...);
        auto raw = unique.get();
        spawn_enemy(std::move(unique));
        return raw;
    }


private:
    Random                                 m_random;
    Bump                                   m_bump;
    Background                             m_background;
    Wall                                   m_wall;
    int                                    m_tick;

    Player                                 m_player{*this};
    std::vector<std::unique_ptr<Enemy>>    m_enemies;

    std::vector<std::unique_ptr<Laser>>    m_lasers;
    std::vector<std::unique_ptr<Bullet>>   m_bullets;

    std::vector<std::unique_ptr<Particle>> m_particles;
};
