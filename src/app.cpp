#include "app.hpp"
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
std::array<Touch, 3>  m_touches;

SpriteRenderer        m_ren;
World                 m_world;

class Menu {
public:
    enum State {
        MS_MAIN,
        MS_GAME,
        MS_HIGHSCORE,
    };


    void switch_state(State s) {
        m_next_state = s;
        m_blend      = 1;
    }

    void update() {
        if (m_blend > 0) {
            m_blend = std::max<float>(0, m_blend - 0.05);
            if (m_blend == 0) {
                m_blend = -1;
                m_state = m_next_state;
            }
        }
        if (m_blend < 0) {
            m_blend = std::min<float>(0, m_blend + 0.05);
        }
        if (m_blend != 0) return;



        switch (m_state) {
        case MS_MAIN:
            if (!m_touches[0].pressed && m_touches[0].prev_pressed) {
                switch_state(MS_MAIN);
                m_world.reset(rnd.get_int(0, 0x7fffffff));
            }
            break;
        case MS_GAME:
        default:
            m_world.update();
            if (!m_world.get_player().is_alive()) {
                switch_state(MS_MAIN);
            }
            break;
        }

    }


    void draw(SpriteRenderer& ren) {
        switch (m_state) {
        case MS_MAIN:
            ren.set_color(Color());
            ren.clear();
            ren.set_color();
            ren.draw(frame(Sprite::TITLE));
            break;

        case MS_GAME:
        default:

            m_world.draw(ren);
            break;
        }

        if (m_blend != 0) {
            ren.set_color({0, 100, 0, 255 *(1 - std::abs(m_blend))});
            rectangle(ren, {-150, -75}, {150, 75});
        }
    }

private:

    State m_state = MS_MAIN;
    State m_next_state;
    float m_blend = -1;


} m_menu;




} // namespace


std::array<Touch, 3> const& get_touches() { return m_touches; }


void init() {
    if (m_initialized) free();
    m_initialized = true;


    gfx::init();
    resource::init();
    DB_REN.init();
    m_ren.init();
    m_ren.set_texture(resource::texture(resource::TID_SPRITES));

    m_world.init();

    rnd.seed();

    static int i = 0;
    if (i++ == 0) {
//        m_world.reset(rnd.get_int(0, 0x7fffffff));
    }
}

void free() {
    if (!m_initialized) return;
    m_initialized = false;

    resource::free();
    m_world.free();

    m_ren.free();

    DB_REN.free();
}

void resize(int width, int height) {
    if (!m_initialized) return;
    if (width == gfx::screen()->width() && height == gfx::screen()->height()) return;
    LOGI("resize %d %d", width, height);
    gfx::screen()->resize(width, height);

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

    m_menu.update();

    // remember touch pressed
    for (Touch& t : m_touches) t.prev_pressed = t.pressed;
}



void draw() {
    // init transform
    m_ren.origin();
    float s = 2.0 / 150;
    float r = gfx::screen()->height() / (float) gfx::screen()->width();
    m_ren.scale({s * r, s});

    // debug
//    m_ren.translate({0, 25});
//    m_ren.scale(0.4);

    DB_REN.transform() = m_ren.transform();

    m_menu.draw(m_ren);

    // debug
//    m_ren.set_color({255, 255, 255, 50});
//    rectangle(m_ren, {-300, -78}, {300, -75});
//    rectangle(m_ren, {-300, 75}, {300, 78});

    m_ren.flush();
    DB_REN.flush();
}


} // namespace
