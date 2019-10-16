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


struct Touch {
    vec2 pos;
    bool pressed;
    bool prev_pressed;
};
std::array<Touch, 3> m_touches;
Touch*               m_touch_move;
Touch*               m_touch_a;
Touch*               m_touch_b;
vec2                 m_touch_move_prev_pos;


bool            m_initialized = false;
SpriteRenderer  m_ren;
World           m_world;

} // namespace



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
    if (i++ == 0) m_world.reset(rnd.get_int(0, 0x7fffffff));
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

    Touch& t = m_touches[id];
    t.pos = p;
    t.pressed = pressed;

}


void key(int key, int unicode) {
}


void update() {

    // input
    for (Touch& t : m_touches) {
        if (!(t.pressed && !t.prev_pressed)) continue;

        // move
        if (!m_touch_move && t.pos.x < 0) {
            m_touch_move = &t;
            m_touch_move_prev_pos = t.pos;
            continue;
        }

        // a
        if (!m_touch_a && t.pos.x > 0 && t.pos.y > 0) {
            m_touch_a = &t;
            continue;
        }

        // b
        if (!m_touch_b && t.pos.x > 0 && (t.pos.y < 0 || m_touch_a)) {
            m_touch_b = &t;
            continue;
        }
    }

    if (m_touch_move && !m_touch_move->pressed) m_touch_move = nullptr;
    if (m_touch_a && !m_touch_a->pressed) m_touch_a = nullptr;
    if (m_touch_b && !m_touch_b->pressed) m_touch_b = nullptr;

    // move
    if (m_touch_move) {
        vec2 dist(8);
        m_touch_move_prev_pos = glm::clamp(m_touch_move_prev_pos, m_touch_move->pos - dist, m_touch_move->pos + dist);
        g_keyboard_input.mov = (m_touch_move->pos - m_touch_move_prev_pos) * (1.0f / 8);
    }
    else {
        g_keyboard_input.mov = {};
    }
    g_keyboard_input.a = !!m_touch_a;
    g_keyboard_input.b = !!m_touch_b;

    m_world.update();

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

    m_world.draw(m_ren);

    // debug
//    m_ren.set_color({255, 255, 255, 50});
//    rectangle(m_ren, {-300, -78}, {300, -75});
//    rectangle(m_ren, {-300, 75}, {300, 78});

//    // title
//    m_ren.set_color();
//    m_ren.draw(frame(Sprite::TITLE));

    // touch
    m_ren.set_color({255, 255, 255, 100});
    if (m_touch_move) {
        m_ren.draw(frame(Sprite::TOUCH), m_touch_move->pos);
        m_ren.draw(frame(Sprite::TOUCH, 1), m_touch_move_prev_pos);
    }
    if (m_touch_a) m_ren.draw(frame(Sprite::TOUCH), m_touch_a->pos);
    if (m_touch_b) m_ren.draw(frame(Sprite::TOUCH), m_touch_b->pos);

    m_ren.flush();
    DB_REN.flush();
}


} // namespace
