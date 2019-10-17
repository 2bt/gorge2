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
struct Button {
    int    id;
    vec2   default_pos;
    vec2   pos;
    Touch* touch;
    void reset(vec2 const& p) { pos = default_pos = p; }
    bool is_pressed() const { return !!touch; }
    vec2 get_move() {
        if (!touch) return {};
        return (touch->pos - pos) * (1.0f / 8);
    }
    void update() {
        if (touch && !touch->pressed) touch = nullptr;
        if (touch) {
            if (id == 0) pos = glm::clamp(pos, touch->pos - vec2(8), touch->pos + vec2(8));
            else pos = glm::mix(pos, touch->pos, 0.6f);
        }
        else pos = glm::mix(pos, default_pos, 0.3f);
    }
};

std::array<Touch, 3>    m_touches;
std::array<Button, 3>   m_buttons     = { Button{ 0 }, Button{ 1 }, Button{ 2 } };
Button&                 m_button_dpad = m_buttons[0];
Button&                 m_button_a    = m_buttons[1];
Button&                 m_button_b    = m_buttons[2];



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

    float aspect_ratio = (float) gfx::screen()->width() / gfx::screen()->height();
    m_button_dpad.reset({-75 * aspect_ratio + 20, 25});
    m_button_a.reset({75 * aspect_ratio - 20, 25});
    m_button_b.reset({75 * aspect_ratio - 20, -25});

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

    float aspect_ratio = (float) gfx::screen()->width() / gfx::screen()->height();
    m_button_dpad.reset({-75 * aspect_ratio + 20, 25});
    m_button_a.reset({75 * aspect_ratio - 20, 25});
    m_button_b.reset({75 * aspect_ratio - 20, -25});
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
        if (!m_button_dpad.touch && t.pos.x < 0) {
            m_button_dpad.touch = &t;
        }
        else if (!m_button_a.touch && t.pos.x > 0 && t.pos.y > 0) {
            m_button_a.touch = &t;
        }
        else if (!m_button_b.touch && t.pos.x > 0 && (t.pos.y < 0 || m_button_a.touch)) {
            m_button_b.touch = &t;
        }
    }
    for (auto& b : m_buttons) b.update();
    g_keyboard_input.a   = m_button_a.is_pressed();
    g_keyboard_input.b   = m_button_b.is_pressed();
    g_keyboard_input.mov = m_button_dpad.get_move();

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
//    if (m_world.get_player().is_alive())
    {
        for (auto const& b : m_buttons) {
            if (b.is_pressed()) m_ren.set_color({255, 255, 255, 100});
            else m_ren.set_color({255, 255, 255, 50});
            m_ren.draw(frame(Sprite::TOUCH, b.id + 1), b.pos);
        }
        if (m_button_dpad.is_pressed()) {
            m_ren.set_color({255, 255, 255, 100});
            m_ren.draw(frame(Sprite::TOUCH, 0), m_button_dpad.touch->pos);
        }
    }

    m_ren.flush();
    DB_REN.flush();
}


} // namespace
