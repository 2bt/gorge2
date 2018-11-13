#include "populator.hpp"
#include "world.hpp"
#include "square_enemy.hpp"
#include "ring_enemy.hpp"
#include "cannon_enemy.hpp"

void Populator::reset(uint32_t seed) {
    m_random.seed(seed);
    m_tick = 0;
}


void Populator::update() {

    ++m_tick;

    // spawn enemy
    if (m_tick % 100 == 0) {
        int i = m_tick / 100 % 3;
        if (i == 0)      m_world.spawn_enemy<SquareEnemy>(vec2(0, -70));
        else if (i == 1) m_world.spawn_enemy<RingEnemy>(vec2(0, -70));
        else if (i == 2) m_world.spawn_enemy<CannonEnemy>(vec2(0, -70), F_NORTH);
    }

}
