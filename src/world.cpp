#include "world.hpp"
#include "sprite.hpp"

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
}


void World::make_laser(vec2 const& pos, vec2 const& vel) {
    m_lasers.push_back(std::make_unique<Laser>(*this, pos, vel));
}

void World::add_particle(std::unique_ptr<Particle> p) {
    m_particles.push_back(std::move(p));
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

    m_background.update();
    m_wall.update();
    m_player.update(get_input());
    update_all(m_lasers);
    update_all(m_particles);
}


void World::draw(SpriteRenderer& ren) {
    // background
    m_background.draw(ren);

    // wall
    m_wall.draw(ren);

    ren.flush();

    m_player.draw(ren);

    for (auto& l : m_lasers) l->draw(ren);
    for (auto& p : m_particles) p->draw(ren);



}
