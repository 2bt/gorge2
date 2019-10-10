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

void touch(int id, int action, int x, int y) {
    LOGI("touch %d %d %d %d", id, action, x, y);
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

//    m_ren.translate({0, 25});
//    m_ren.scale(0.4);

    DB_REN.transform() = m_ren.transform();

    m_world.draw(m_ren);

    m_ren.set_color({255, 255, 255, 50});
    rectangle(m_ren, {-300, -78}, {300, -75});
    rectangle(m_ren, {-300, 75}, {300, 78});

//    // title
//    m_ren.set_color();
//    m_ren.draw(frame(Sprite::TITLE));

    m_ren.flush();
    DB_REN.flush();
}


} // namespace
