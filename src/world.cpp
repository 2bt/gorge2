#include "world.hpp"
#include "square_enemy.hpp"

#include <SDL2/SDL.h>


void World::init() {
    m_background.init();
}

void World::free() {
    m_background.free();
}

void World::reset(uint32_t seed) {
    m_random.seed(seed);
    m_background.reset(m_random.get_int(0, 0x7fffffff));
    m_wall.reset(m_random.get_int(0, 0x7fffffff));
    m_player.reset();
    m_tick = 0;

    m_enemies.clear();
    m_lasers.clear();
    m_bullets.clear();
    m_particles.clear();

}


void World::spawn_laser(vec2 const& pos, vec2 const& vel) {
    m_lasers.push_back(std::make_unique<Laser>(*this, pos, vel));
}
void World::spawn_bullet(vec2 const& pos, vec2 const& vel, Bullet::Desc const& desc) {
    m_bullets.push_back(std::make_unique<Bullet>(*this, pos, vel, desc));
}

void World::spawn_particle(std::unique_ptr<Particle> p) {
    m_particles.push_back(std::move(p));
}

void World::spawn_enemy(std::unique_ptr<Enemy> e) {
    m_enemies.push_back(std::move(e));
}


// TODO
Player::Input get_input() {
    Uint8 const* ks = SDL_GetKeyboardState(nullptr);
    return {
        ks[SDL_SCANCODE_RIGHT] - ks[SDL_SCANCODE_LEFT],
        ks[SDL_SCANCODE_DOWN] - ks[SDL_SCANCODE_UP],
        ks[SDL_SCANCODE_X],
        ks[SDL_SCANCODE_Y] | ks[SDL_SCANCODE_Z],
    };
}

template<class T>
void update_all(std::vector<std::unique_ptr<T>>& vs) {
    for (auto it = vs.begin(); it != vs.end();) {
         if ((*it)->update()) ++it;
         else it = vs.erase(it);
    }
}


void World::update() {

    // spawn enemy
    if (m_tick % 100 == 0) {
        spawn_enemy<SquareEnemy>(vec2(0, -70));
    }
    ++m_tick;

    m_background.update();
    m_wall.update();
    m_player.update(get_input());
    update_all(m_enemies);
    update_all(m_lasers);
    update_all(m_bullets);
    update_all(m_particles);
}


void World::draw(SpriteRenderer& ren) {
    m_background.draw(ren);
    m_player.draw(ren);
    for (auto& e : m_enemies) e->draw(ren);
    for (auto& l : m_lasers) l->draw(ren);
    for (auto& b : m_bullets) b->draw(ren);
    for (auto& p : m_particles) {
        if (p->get_layer() == Particle::BACK) p->draw(ren);
    }
    m_wall.draw(ren);
    // TODO: bump


    for (auto& p : m_particles) {
        if (p->get_layer() == Particle::FRONT) p->draw(ren);
    }
}
