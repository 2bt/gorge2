#include "resource.hpp"
#include "world.hpp"


void World::init_buttons() {
    float aspect_ratio = (float) app::screen_size().x / app::screen_size().y;
    m_button_dpad.reset({-75 * aspect_ratio + 25, 20});
    m_button_a.reset({75 * aspect_ratio - 25, 20});
    m_button_b.reset({75 * aspect_ratio - 25, -20});
}


void World::init() {
    m_bump.init();
    m_background.init();

    init_buttons();
}

void World::free() {
    m_bump.free();
    m_background.free();
}

void World::resized() {
    m_bump.resized();

    init_buttons();
}

void World::reset(uint32_t seed) {
    m_tick            = 0;
    m_shake           = 0;
    m_powerup_balance = 0;
    m_flame_balance   = 0;

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

void World::maybe_spawn_spawn_powerup(vec2 const& pos, int amount) {
    m_powerup_balance += amount;
    if (m_powerup_balance >= 7 && BallItem::count() == 0 &&
            (!m_player.get_balls()[0].is_alive() || !m_player.get_balls()[1].is_alive())) {
        m_powerup_balance -= 7;
        spawn_item<BallItem>(pos);
        return;
    }
    if (m_powerup_balance >= 15 && HeartItem::count() == 0 &&
            m_player.get_shield() < Player::MAX_SHIELD) {
        m_powerup_balance -= 15;
        spawn_item<HeartItem>(pos);
        return;
    }
    if (m_powerup_balance >= 20) {
        m_powerup_balance -= 20;
        spawn_item<MoneyItem>(pos);
        return;
    }
}
void World::maybe_spawn_spawn_flame(vec2 const& pos, int amount) {
    m_flame_balance += amount;
    if (m_flame_balance >= 5) {
        m_flame_balance -= 5;
        spawn_item<FlameItem>(pos);
        return;
    }
}


template<class T>
void update_all(std::vector<std::unique_ptr<T>>& v) {
    // don't use iterators because v might grow during update
    for (int i = 0; i < (int) v.size();) {
        if (v[i]->update()) ++i;
        else v.erase(v.begin() + i);
    }
}

extern Player::Input g_keyboard_input;

void World::update() {

    // input
    for (auto const& t : app::touches()) {
        if (!t.just_pressed()) continue;
        if (!m_button_dpad.touch && t.pos.x < 0) {
            m_button_dpad.touch   = &t;
            m_button_dpad.trg_pos = t.pos;
        }
        else if (!m_button_a.touch && t.pos.x > 0 && t.pos.y > 0) {
            m_button_a.touch = &t;
        }
        else if (!m_button_b.touch && t.pos.x > 0 && (t.pos.y < 0 || m_button_a.touch)) {
            m_button_b.touch = &t;
        }
    }
    for (auto& b : m_buttons) b.update();

    Player::Input input;
    input.a   = m_button_a.is_pressed();
    input.b   = m_button_b.is_pressed();
    input.mov = m_button_dpad.get_move();

    // keyboard input
    input.a |= g_keyboard_input.a;
    input.b |= g_keyboard_input.b;
    input.mov = glm::clamp(input.mov + g_keyboard_input.mov, -1.0f, 1.0f);


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
    ren.translate({ rnd.get_float(-m_shake, m_shake), rnd.get_float(-m_shake, m_shake) });

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
    float w = (float) app::screen_size().x / app::screen_size().y * 75;
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

    draw_buttons(ren);
}
void World::draw_buttons(SpriteRenderer& ren) {
    for (auto const& b : m_buttons) {

        if (b.id == 2) {
            vec2 pos = b.pos;
            auto rect = frame(Sprite::TOUCH, 3);

            ren.set_color({255, 255, 255, 30});
            ren.draw(rect, pos);

            // energy
            float e = m_player.get_energy() / Player::MAX_ENERGY;
            rect.pos.y += rect.size.y * (1 - e);
            pos.y      += rect.size.y * (1 - e) * 0.5;
            rect.size.y *= e;

            ren.set_color({0, 200, 200, 100});
            if (m_player.is_field_active() || m_player.get_energy() == Player::MAX_ENERGY) {
                float l = m_tick % 16 / 16.0;
                l = 0.5 + std::sin(l * 2 * M_PI) * 0.5;
                ren.set_color({
                    glm::mix(0,   255, l),
                    glm::mix(200, 255, l),
                    glm::mix(200, 255, l),
                    100});
            }

            ren.draw(rect, pos);
        }
        else {
            ren.set_color({255, 255, 255, 100});
            ren.draw(frame(Sprite::TOUCH, b.id + 1), b.pos);
        }
    }
    if (m_button_dpad.is_pressed()) {
        ren.set_color({255, 255, 255, 100});
        ren.draw(frame(Sprite::TOUCH, 0), m_button_dpad.touch->pos);
    }
}


void World::menu_update() {
    m_background.update();
    update_all(m_enemies);
    update_all(m_particles);
}


void World::menu_draw(SpriteRenderer& ren, std::function<void()> const& f) {
    m_background.draw(ren);
    for (auto& p : m_particles) {
        if (p->get_layer() == Particle::BACK) p->draw(ren);
    }

    f();

    for (auto& p : m_particles) {
        if (p->get_layer() == Particle::FRONT) p->draw(ren);
    }
}
