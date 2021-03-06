#pragma once
#include "app.hpp"
#include "bump.hpp"
#include "background.hpp"
#include "wall.hpp"
#include "populator.hpp"
#include "player.hpp"
#include "particle.hpp"
#include "enemy.hpp"
#include "item.hpp"
#include <memory>


class World {
public:

    void init();
    void free();
    void resized();

    void reset(uint32_t seed);
    void update();
    void draw(SpriteRenderer& ren);


    void menu_update();
    void menu_draw(SpriteRenderer& ren, std::function<void()> const& f);

    void shake() { m_shake += 2.5; }

    Bump& get_bump() { return m_bump; }
    Wall const& get_wall() const { return m_wall; }
    Wall& get_wall() { return m_wall; }
    Player& get_player() { return m_player; }
    Player const& get_player() const { return m_player; }
    ShockWave& get_shock_wave() { return m_shock_wave; }
    std::vector<std::unique_ptr<Enemy>> const& get_enemies() const { return m_enemies; }


    void spawn_laser(std::unique_ptr<Laser> l);
    void spawn_bullet(vec2 const& pos, vec2 const& vel, Bullet::Desc const& desc);
    void spawn_particle(std::unique_ptr<Particle> p);
    void spawn_enemy(std::unique_ptr<Enemy> e);
    void spawn_item(std::unique_ptr<Item> i);

    void maybe_spawn_spawn_powerup(vec2 const& pos, int amount = 1);
    void maybe_spawn_spawn_flame(vec2 const& pos, int amount = 1);

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

    template<class T, typename... Args>
    T* spawn_item(Args&&... args) {
        auto unique = std::make_unique<T>(*this, std::forward<Args>(args)...);
        auto raw = unique.get();
        spawn_item(std::move(unique));
        return raw;
    }


private:
    void init_buttons();
    void draw_buttons(SpriteRenderer& ren);

    struct Button {
        const int         id;
        vec2              default_pos;
        vec2              trg_pos;
        vec2              pos;
        app::Touch const* touch;
        void reset(vec2 const& p) { pos = default_pos = p; }
        bool is_pressed() const { return !!touch; }
        vec2 get_move() {
            if (!touch) return {};
            return glm::clamp((touch->pos - trg_pos) * (1.0f / 8), -1.0f, 1.0f);
        }
        void update() {
            if (touch && !touch->pressed) touch = nullptr;
            if (touch) {
                if (id == 0) trg_pos = glm::clamp(trg_pos, touch->pos - vec2(8), touch->pos + vec2(8));
                else trg_pos = touch->pos;
            }
            else trg_pos = default_pos;
            pos = glm::mix(pos, trg_pos, 0.5f);
        }
    };


    Random                                 m_random;
    Bump                                   m_bump;
    Background                             m_background;
    Wall                                   m_wall{m_populator};
    Populator                              m_populator{*this};
    int                                    m_tick;
    float                                  m_shake;
    int                                    m_powerup_balance;
    int                                    m_flame_balance;

    ShockWave                              m_shock_wave;
    Player                                 m_player{*this};
    std::vector<std::unique_ptr<Enemy>>    m_enemies;
    std::vector<std::unique_ptr<Laser>>    m_lasers;
    std::vector<std::unique_ptr<Bullet>>   m_bullets;
    std::vector<std::unique_ptr<Particle>> m_particles;
    std::vector<std::unique_ptr<Item>>     m_items;

    std::array<Button, 3> m_buttons     = { Button{ 0 }, Button{ 1 }, Button{ 2 } };
    Button&               m_button_dpad = m_buttons[0];
    Button&               m_button_a    = m_buttons[1];
    Button&               m_button_b    = m_buttons[2];
};
