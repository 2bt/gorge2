#include "app.hpp"
#include "audio.hpp"
#include "log.hpp"
#include "gfx.hpp"
#include "resource.hpp"
#include "sprite.hpp"
#include "debug_renderer.hpp"
#include "world.hpp"

DebugRenderer DB_REN;

extern Player::Input g_keyboard_input;

namespace app {
namespace {


bool                  m_initialized = false;
glm::ivec2            m_screen_size;
std::array<Touch, 3>  m_touches;

SpriteRenderer        m_ren;
World                 m_world;

class Game {
public:
    enum State {
        MS_MENU,
        MS_PLAYING,
        MS_HIGHSCORE,
    };


    void switch_state(State s) {
        if (m_next_state == s) return;
        m_next_state = s;
        m_blend      = 0;
        m_blend_vel  = 0.05;
    }

    Rect const m_button = { {-133, -75}, {50, 50} };

    void menu_update() {
        m_world.menu_update();
        if (rnd.get_int(0, 10) < 3) {
            make_sparkle(m_world, {rnd.get_float(-55, 55), rnd.get_float(-11, 11)}, true);
        }

        bool pressed = false;
        if (m_blend == 0) {
            pressed |= g_keyboard_input.a;
            pressed |= g_keyboard_input.b;
            pressed |= m_touches[0].just_pressed() && Rect{ {-55, -11}, {110, 22} }.contains(m_touches[0].pos);
        }
        if (pressed) switch_state(MS_PLAYING);

        // check button
        if (m_touches[0].just_pressed()) {
            if (m_button.contains(m_touches[0].pos)) {
            }
        }
    }

    void menu_draw(SpriteRenderer& ren) {
        m_world.menu_draw(ren, [&ren](){
            ren.set_color();
            ren.draw(frame(Sprite::TITLE));
        });

        // draw button
        ren.set_color({255, 100, 100, 100});
        draw_rectangle(ren, m_button.pos, m_button.pos + m_button.size);
    }

    void update() {
        if (m_touches[0].just_pressed()) {
            if (Rect{ {-133, -75}, {30, 30} }.contains(m_touches[0].pos)) m_paused = !m_paused;
        }

        if (m_paused) return;

        m_blend += m_blend_vel;
        if (m_blend > 1) {
            if (m_next_state == MS_PLAYING || m_state == MS_PLAYING) {
                m_death_delay = 0;
                m_world.reset(rnd.get_int(0, 0x7fffffff));
            }
            m_blend = 1;
            m_blend_vel = -0.05;
            m_state = m_next_state;
        }
        if (m_blend < 0) {
            m_blend = 0;
            m_blend_vel = 0;
        }


        switch (m_state) {
        case MS_MENU:
            menu_update();
            break;
        case MS_PLAYING:
            m_world.update();
            if (!m_world.get_player().is_alive()) {
                if (++m_death_delay > 180) switch_state(MS_MENU);
            }
            break;
        default: assert(0);
        }

    }


    void draw(SpriteRenderer& ren) {
        switch (m_state) {
        case MS_MENU:
            menu_draw(ren);
            break;
        case MS_PLAYING:
        default:
            m_world.draw(ren);
            break;
        }
        if (m_blend > 0) {
            ren.set_color({0, 0, 0, 255 * m_blend});
            draw_rectangle(ren, {-160, -75}, {160, 75});
        }
    }

private:

    State m_state = MS_MENU;
    State m_next_state;
    float m_blend = 1;
    float m_blend_vel = -0.05;
    int   m_death_delay = 0;

    bool  m_paused = false;

} m_game;




} // namespace


std::array<Touch, 3> const& touches() { return m_touches; }
glm::ivec2 const& screen_size() { return m_screen_size; }


void init() {
    if (m_initialized) free();
    m_initialized = true;

    audio::init();
    gfx::init();

    // XXX
    {
        int width = gfx::screen()->width();
        int height = gfx::screen()->height();

        // screen size
        float aspect_ratio = std::max(height / (float) width, 75 / 160.0f);
        m_screen_size.x = int(height / aspect_ratio);
        m_screen_size.y = height;
        m_ren.set_viewport({ (width - m_screen_size.x) / 2, 0, m_screen_size.x, m_screen_size.y });

        // init transform
        m_ren.origin();
        const float s = 1.0 / 75;
        m_ren.scale({s * aspect_ratio, s});
    }


    resource::init();
    DB_REN.init();
    m_ren.init();
    m_ren.set_texture(resource::texture(resource::TID_SPRITES));

    m_world.init();

    rnd.seed();

    static int i = 0;
    if (i++ == 0) {
        m_world.reset(rnd.get_int(0, 0x7fffffff));
    }
}

void free() {
    if (!m_initialized) return;
    m_initialized = false;

    resource::free();
    m_world.free();

    m_ren.free();

    DB_REN.free();
    audio::free();
}

void resize(int width, int height) {
    if (!m_initialized) return;
    if (width == gfx::screen()->width() && height == gfx::screen()->height()) return;
    LOGI("resize %d %d", width, height);
    gfx::screen()->resize(width, height);


    // screen size
    float aspect_ratio = std::max(height / (float) width, 75 / 160.0f);
    m_screen_size.x = int(height / aspect_ratio);
    m_screen_size.y = height;
    m_ren.set_viewport({ (width - m_screen_size.x) / 2, 0, m_screen_size.x, m_screen_size.y });

    // init transform
    m_ren.origin();
    const float s = 1.0 / 75;
    m_ren.scale({s * aspect_ratio, s});


    m_world.resized();
}


void touch(int id, bool pressed, int x, int y) {
    // LOGI("touch %d %d %d %d", id, pressed, x, y);
    if (id >= (int) m_touches.size()) return;
    // transform to game coordinates
    vec2 p = vec2(x - gfx::screen()->width() * 0.5f, y - gfx::screen()->height() * 0.5f);
    p *= 150.0f / gfx::screen()->height();
    m_touches[id].pos = p;
    m_touches[id].pressed = pressed;
}


void key(int key, int unicode) {
}


void update() {
    m_game.update();

    // remember touch pressed
    for (Touch& t : m_touches) t.prev_pressed = t.pressed;
}


//#define DEBUG_ZOOM


void draw() {
    m_ren.set_color(Color{});
    m_ren.clear();

#ifdef DEBUG_ZOOM
    // debug
    m_ren.push();
    m_ren.translate({0, 20});
    m_ren.scale(0.5);
#endif
    DB_REN.transform() = m_ren.transform();

    m_game.draw(m_ren);

#ifdef DEBUG_ZOOM
    DB_REN.set_line_width(3);
    DB_REN.set_color(255, 0, 0);
    DB_REN.rect({-133.33, -75}, {133.33, 75});
    m_ren.pop();
#endif


    m_ren.flush();
    DB_REN.flush();
}


} // namespace
