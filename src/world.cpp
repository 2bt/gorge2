#include "resource.hpp"
#include "world.hpp"


void World::init() {
    m_bump.init();
    m_background.init();
}

void World::free() {
    m_bump.free();
    m_background.free();
}

void World::resized() {
    m_bump.resized();
}

void World::reset(uint32_t seed) {
    printf("seed %u\n", seed);

    m_tick  = 0;
    m_shake = 0;
    m_random.seed(seed);
    m_bump.reset();
    m_background.reset(m_random.get_int(0, 0x7fffffff));
    m_wall.reset(m_random.get_int(0, 0x7fffffff));
    m_populator.reset(m_random.get_int(0, 0x7fffffff));

    m_shock_wave.reset();
    m_player.reset();
    m_enemies.clear();
    m_lasers.clear();
    m_bullets.clear();
    m_particles.clear();
    m_items.clear();
}

void World::spawn_laser(std::unique_ptr<Laser> l) {
    m_lasers.push_back(std::move(l));
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
void World::spawn_item(std::unique_ptr<Item> i) {
    m_items.push_back(std::move(i));
}


template<class T>
void update_all(std::vector<std::unique_ptr<T>>& vs) {
    // don't use iterators because vs might grow during update
    for (int i = 0; i < (int) vs.size();) {
        if (vs[i]->update()) ++i;
        else vs.erase(vs.begin() + i);
    }
}

extern Player::Input g_keyboard_input;

void World::update() {

    Player::Input input = g_keyboard_input;

    ++m_tick;
    m_shake *= 0.95;

    m_populator.update();
    m_bump.update();
    m_background.update();
    m_wall.update();
    m_shock_wave.update();

    m_player.update(input);

    update_all(m_enemies);
    update_all(m_lasers);
    update_all(m_bullets);
    update_all(m_particles);
    update_all(m_items);
}


void World::draw(SpriteRenderer& ren) {
    ren.push();
    ren.translate(vec2(rnd.get_float(-m_shake, m_shake), rnd.get_float(-m_shake, m_shake)));

    m_bump.draw_begin(ren);
    {
        m_background.draw(ren);
        for (auto& p : m_particles) {
            if (p->get_layer() == Particle::BACK) p->draw(ren);
        }
        for (auto& i : m_items) i->draw(ren);
        m_player.draw(ren);
        m_shock_wave.draw(ren);

        // use special shader for enemies
        ren.set_shader(resource::shader(resource::SID_FLASH));
        for (auto& e : m_enemies) e->draw(ren);
        ren.set_shader();

        for (auto& l : m_lasers) l->draw(ren);
        for (auto& b : m_bullets) b->draw(ren);
        m_wall.draw(ren);
    }
    m_bump.draw_end(ren);

    for (auto& p : m_particles) {
        if (p->get_layer() == Particle::FRONT) p->draw(ren);
    }

    ren.pop();

    // HUD
    float w = gfx::screen()->width() / (float) gfx::screen()->height() * 75;
    // hearts
    ren.set_color();
    for (int i = 0; i < Player::MAX_SHIELD; ++i) {
        int f = 1;
        if (i <= m_player.get_shield() - 1 ) {
            if (m_player.get_shield() == 1) f = m_tick % 16 < 8;
            else f = 0;
        }
        ren.draw(frame(Sprite::HEALTH, f), {-w + 6 + i * 8 , -70});
    }
    // score
    char str[32];
    sprintf(str, "%07d", m_player.get_score());
    ren.push();
    ren.translate({w - 5 * 8, -71});
    shadow_print(ren, str);
    ren.pop();

    // energy
    ren.set_color({60, 60, 60, 100});
    rectangle(ren, {Player::MAX_ENERGY * -0.5, -73}, {Player::MAX_ENERGY * 0.5, -72});
    ren.set_color({0, 200, 200});
    if (m_player.is_field_active()) {
        if (m_tick % 8 < 4) ren.set_color({0, 127, 127});
    }
    else if (m_player.get_energy() == Player::MAX_ENERGY) {
        if (m_tick % 8 < 4) ren.set_color();
    }
    rectangle(ren, {Player::MAX_ENERGY * -0.5, -73}, {Player::MAX_ENERGY * -0.5 + m_player.get_energy(), -72});
}
