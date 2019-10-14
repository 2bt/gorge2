#include "app.hpp"
#include "log.hpp"
#include "gfx.hpp"
#include "resource.hpp"
#include "sprite.hpp"
#include "debug_renderer.hpp"
#include "world.hpp"

DebugRenderer DB_REN;

namespace app {
namespace {

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


struct Touch {
    SpriteRenderer::Color color;
    vec2                  pos;
    bool                  pressed;
};

std::array<Touch, 3> m_touches = {
    Touch{
        { 255, 0, 0, 255 },
    },
    Touch{
        { 0, 255, 0, 255 },
    },
    Touch{
        { 0, 0, 255, 255 },
    },
};


void touch(int id, bool pressed, int x, int y) {
    LOGI("touch %d %d %d %d", id, pressed, x, y);

    if (id >= (int) m_touches.size()) return;
    Touch& t = m_touches[id];
    t.pos = { x, y };
    t.pressed = pressed;
}


void key(int key, int unicode) {
}

void update() {
    m_world.update();
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

    // touches
    for (Touch const& t : m_touches) {
        if (!t.pressed) continue;
        vec2 p = t.pos - vec2(gfx::screen()->width(), gfx::screen()->height()) * 0.5f;
        p *= 150.0f / gfx::screen()->height();

        m_ren.set_color(t.color);
        m_ren.draw(frame(Sprite::EXPLOSION), p);
    }


    m_ren.flush();
    DB_REN.flush();
}


} // namespace
