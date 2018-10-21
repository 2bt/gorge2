#include "world.hpp"
#include "sprite.hpp"

#include <SDL2/SDL.h>


void World::init() {
    m_background.init();

//    // reset randomly
//    m_random.seed();
//    reset(m_random.get_int(0, 0x7fffffff));

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


Player::Input get_input() {
    Uint8 const* ks = SDL_GetKeyboardState(nullptr);
    return {
        ks[SDL_SCANCODE_RIGHT] - ks[SDL_SCANCODE_LEFT],
        ks[SDL_SCANCODE_DOWN] - ks[SDL_SCANCODE_UP],
        ks[SDL_SCANCODE_X],
        ks[SDL_SCANCODE_Y] | ks[SDL_SCANCODE_Z],
    };

}


void World::update() {

    m_background.update();
    m_wall.update();
    //m_player.update(input);
}


void World::draw(SpriteRenderer& ren) {
    // background
    m_background.draw(ren);

    // wall
    m_wall.draw(ren);

    ren.flush();

    m_player.update(get_input());
    m_player.draw(ren);

    for (auto it = m_lasers.begin(); it != m_lasers.end();) {
         if ((*it)->update()) ++it;
         else it = m_lasers.erase(it);
    }
    for (auto& l : m_lasers) l->draw(ren);

}
