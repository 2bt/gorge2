#include "bump.hpp"
#include "fx.hpp"
#include "wall.hpp"


void Bump::init() {
    m_bump_tex = gfx::Texture2D::create("media/bump.png", gfx::FilterMode::Linear);

    m_bump_canvas = gfx::Texture2D::create(gfx::TextureFormat::Red, fx::screen_width(), fx::screen_height());
    m_main_canvas = gfx::Texture2D::create(gfx::TextureFormat::RGB, fx::screen_width(), fx::screen_height());

    m_bump_framebuffer = gfx::Framebuffer::create();
    m_bump_framebuffer->attach_color(m_bump_canvas);

    m_main_framebuffer = gfx::Framebuffer::create();
    m_main_framebuffer->attach_color(m_main_canvas);

    m_shader = gfx::Shader::create(R"(
        #version 100
        attribute vec2 in_pos;
        attribute vec2 in_tex_coord;
        attribute vec4 in_color;
        uniform vec2 tex_scale;
        varying vec2 ex_tex_coord;
        varying vec4 ex_color;
        void main() {
            gl_Position = vec4(in_pos.x, -in_pos.y, 0.0, 1.0);
            ex_tex_coord = in_tex_coord * tex_scale;
        })", R"(
        #version 100
        precision highp float;
        uniform vec2 tex_scale;
        uniform sampler2D tex;
        uniform sampler2D bump;
        varying vec2 ex_tex_coord;
        void main() {
            vec2 d = vec2(5.0) * tex_scale;
            float h  = texture2D(bump, ex_tex_coord).r;
            float h2 = texture2D(bump, ex_tex_coord + vec2(d.x, 0.0)).r;
            float h3 = texture2D(bump, ex_tex_coord + vec2(0.0, d.y)).r;
            vec2 o = vec2(h - h2, h - h3) * 0.04;
            gl_FragColor = texture2D(tex, ex_tex_coord + o);
        })");
    m_shader->set_uniform("bump", m_bump_canvas);
}


void Bump::free() {
    delete m_bump_tex;
    delete m_bump_canvas;
    delete m_main_canvas;
    delete m_bump_framebuffer;
    delete m_main_framebuffer;
    delete m_shader;
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
    ren.set_texture(m_bump_tex);
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
    ren.scale({2.0 / fx::screen_width(), -2.0 / fx::screen_height()});

    // DEBUG
//    ren.set_texture(m_main_canvas);
//    ren.reset_color();
//    ren.draw();
//    ren.set_blendmode(SpriteRenderer::BM_ADD);
//    ren.set_texture(m_bump_canvas);
//    ren.draw();

    ren.set_texture(m_main_canvas);
    ren.set_shader(m_shader);
    ren.draw();

    ren.pop();
    ren.pop_state();
}
