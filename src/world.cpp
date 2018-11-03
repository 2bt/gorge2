#include "world.hpp"
#include "square_enemy.hpp"
#include "fx.hpp"


void World::init() {
    m_bump.free();
    m_background.init();
    m_shock_wave.init();
    m_flash_shader = gfx::Shader::create(R"(
        #version 100
        attribute vec2 in_pos;
        attribute vec2 in_tex_coord;
        attribute vec4 in_color;
        uniform vec2 tex_scale;
        varying vec2 ex_tex_coord;
        varying vec4 ex_color;
        void main() {
            gl_Position = vec4(in_pos.x, -in_pos.y, 0.0, 1.0);
            ex_color = in_color;
            ex_tex_coord = in_tex_coord * tex_scale;
        })", R"(
        #version 100
        precision mediump float;
        uniform sampler2D tex;
        varying vec2 ex_tex_coord;
        varying vec4 ex_color;
        void main() {
            vec4 tc = texture2D(tex, ex_tex_coord);
            tc = vec4(tc.rgb * ex_color.rgb, tc.a);
            gl_FragColor = tc + vec4(vec3(1.0) - tc.rgb, 0.0) * (1.0 - ex_color.a);
        })");
}

void World::free() {
    m_bump.free();
    m_background.free();
    m_shock_wave.free();
    delete m_flash_shader;
}

void World::resized() {
    m_bump.resized();
}

void World::reset(uint32_t seed) {
    m_tick = 0;

    m_random.seed(seed);

    m_bump.reset();
    m_background.reset(m_random.get_int(0, 0x7fffffff));
    m_wall.reset(m_random.get_int(0, 0x7fffffff));

    m_shock_wave.reset();
    m_player.reset();
    m_enemies.clear();
    m_lasers.clear();
    m_bullets.clear();
    m_particles.clear();

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

    m_bump.update();
    m_background.update();
    m_wall.update();
    m_shock_wave.update();
    m_player.update(fx::input());
    update_all(m_enemies);
    update_all(m_lasers);
    update_all(m_bullets);
    update_all(m_particles);
    update_all(m_items);
}


void World::draw(SpriteRenderer& ren) {

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
        ren.set_shader(m_flash_shader);
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


    // HUD
    float w = fx::screen_width() / (float) fx::screen_height() * 75;
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
    rectangle(ren, {-15, -73}, {15, -72});
    ren.set_color({0, 200, 200});
    if (m_player.is_field_active()) {
        if (m_tick % 8 < 4) ren.set_color({0, 127, 127});
    }
    else if (m_player.get_energy() == Player::MAX_ENERGY) {
        if (m_tick % 8 < 4) ren.set_color();
    }
    rectangle(ren, {-15, -73}, {-15 + m_player.get_energy(), -72});
}
