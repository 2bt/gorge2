#include "bump.hpp"
#include "wall.hpp"
#include "resource.hpp"
#include "log.hpp"


void Bump::init() {
//    m_bump_canvas = gfx::Texture2D::create(gfx::TextureFormat::Alpha, gfx::screen()->width(), gfx::screen()->height());
    m_bump_canvas = gfx::Texture2D::create(gfx::TextureFormat::RGB, gfx::screen()->width(), gfx::screen()->height());
    m_main_canvas = gfx::Texture2D::create(gfx::TextureFormat::RGB, gfx::screen()->width(), gfx::screen()->height());

    m_bump_framebuffer = gfx::Framebuffer::create();
    m_bump_framebuffer->attach_color(m_bump_canvas);

    m_main_framebuffer = gfx::Framebuffer::create();
    m_main_framebuffer->attach_color(m_main_canvas);

    resource::shader(resource::SID_BUMP)->set_uniform("bump", m_bump_canvas);
}


void Bump::free() {
    delete m_bump_canvas;
    delete m_main_canvas;
    delete m_bump_framebuffer;
    delete m_main_framebuffer;
}


void Bump::resized() {
    free();
    init();
}


void Bump::reset() {
    m_waves.clear();
}


void Bump::spawn(vec2 const& pos) {
    m_waves.push_back(Wave{pos, 0.05});
}


void Bump::update() {
    for (auto it = m_waves.begin(); it != m_waves.end();) {
        it->l += + 0.015;
        it->pos.y += Wall::SPEED;
        if (it->l >= 1) it = m_waves.erase(it);
        else ++it;
    }
}


void Bump::draw_begin(SpriteRenderer& ren) {
    ren.push_state();
    ren.set_framebuffer(m_bump_framebuffer);
    ren.clear({});
    ren.set_texture(resource::texture(resource::TID_BUMP));
    ren.set_blendmode(SpriteRenderer::BM_ADD);
    for (auto const& w : m_waves) {
        ren.set_color({255, 255, 255, std::pow(1 - w.l, 4) * 255});
        ren.push();
        ren.translate(w.pos);
        ren.scale(std::pow(w.l, 0.7) * 3);
        ren.draw();
        ren.pop();
    }
    ren.pop_state();
    ren.push_state();
    ren.set_framebuffer(m_main_framebuffer);
    ren.clear({});
}


void Bump::draw_end(SpriteRenderer& ren) {
    ren.pop_state();
    ren.push_state();
    ren.push();
    ren.origin();
    ren.scale({2.0 / gfx::screen()->width(), -2.0 / gfx::screen()->height()});
    // DEBUG
//    ren.set_texture(m_main_canvas);
//    ren.reset_color();
//    ren.draw();
//    ren.set_blendmode(SpriteRenderer::BM_ADD);
//    ren.set_texture(m_bump_canvas);
//    ren.draw();

    ren.set_texture(m_main_canvas);
    ren.set_shader(resource::shader(resource::SID_BUMP));
    ren.draw();

    ren.pop();
    ren.pop_state();
}
