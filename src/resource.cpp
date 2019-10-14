#include <array>
#include "log.hpp"
#include "resource.hpp"
#include "util.hpp"
#include "foo.hpp"

namespace resource {
namespace {


std::array<gfx::Texture2D*, TID_COUNT> s_textures;
std::array<gfx::Shader*, SID_COUNT>    s_shaders;

gfx::Texture2D* render_shockwave_texture() {
    gfx::Texture2D* result = gfx::Texture2D::create(gfx::TextureFormat::RGBA, 640, 640);

    gfx::Shader* shader = gfx::Shader::create(R"(
        #version 100
        attribute vec2 in_pos;
        attribute vec2 in_tex_coord;
        varying vec2 ex_tex_coord;
        void main() {
            gl_Position = vec4(in_pos.x, in_pos.y, 0.0, 1.0);
            ex_tex_coord = in_tex_coord;
        })", R"(
        #version 100
        precision mediump float;
        uniform sampler2D table;
        uniform float r1;
        uniform float r2;
        varying vec2 ex_tex_coord;
        void main() {
            float d = distance(ex_tex_coord.xy, vec2(40.0));
            if (d > r1) gl_FragColor = vec4(0.0);
            else if (d < r2) gl_FragColor = texture2D(table, vec2((r2 - d) * 0.1 , 0.0));
            else if (d < r1 - 1.0) gl_FragColor = vec4(0.0, 1.0, 1.0, 0.8);
            else gl_FragColor = vec4(1.0);
        })");
    static const std::array<Color, 7> buf = {
        Color(0, 0, 0, 0),
        Color(0, 255, 255, 160),
        Color(0, 255, 255, 130),
        Color(0, 0, 0, 0),
        Color(0, 0, 0, 0),
        Color(0, 255, 255, 100),
        Color(0, 0, 0, 0),
    };
    gfx::Texture2D* table = gfx::Texture2D::create(gfx::TextureFormat::RGBA, buf.size(), 1, (void const*) buf.data());
    shader->set_uniform("table", table);
    gfx::Framebuffer* framebuffer = gfx::Framebuffer::create();
    framebuffer->attach_color(result);

    SpriteRenderer ren;
    ren.init();
    ren.set_framebuffer(framebuffer);
    ren.set_shader(shader);
    ren.origin();
    ren.scale(1 / 40.0);

    for (int i = 0; i < 56; ++i) {
        float level = (i + 1) * 0.018;
        float r1 = (1 - std::pow(2, (level * -5))) * 40;
        float r2 = (1 - std::pow(2, (level * -2.25))) * 60;
        shader->set_uniform("r1", r1);
        shader->set_uniform("r2", r2);
        ren.set_viewport({i % 8 * 80, i / 8 * 80, 80, 80});
        ren.set_color();
        ren.draw({{}, {80, 80}});
    }
    ren.flush();

    ren.free();
    delete shader;
    delete framebuffer;
    delete table;

    return result;
}

gfx::Texture2D* render_praxis_texture() {
    gfx::Texture2D* result = gfx::Texture2D::create(gfx::TextureFormat::RGBA, 480, 480);

    gfx::Shader* shader = gfx::Shader::create(R"(
        #version 100
        attribute vec2 in_pos;
        attribute vec2 in_tex_coord;
        varying vec2 ex_tex_coord;
        void main() {
            gl_Position = vec4(in_pos.x, in_pos.y, 0.0, 1.0);
            ex_tex_coord = in_tex_coord;
        })", R"(
        #version 100
        precision mediump float;
        uniform float r;
        uniform float alpha;
        varying vec2 ex_tex_coord;
        void main() {
            float d = r - distance(ex_tex_coord.xy, vec2(40.0));
            if (d >= 0.0 && d < 5.0 && !(d > 2.0 && d < 4.0)) gl_FragColor = vec4(0.63, 0.63, 0.63, alpha);
            else gl_FragColor = vec4(0.0);
        })");
    gfx::Framebuffer* framebuffer = gfx::Framebuffer::create();
    framebuffer->attach_color(result);

    SpriteRenderer ren;
    ren.init();
    ren.set_framebuffer(framebuffer);
    ren.set_shader(shader);
    ren.origin();
    ren.scale(1 / 40.0);

    for (int i = 0; i < 30; ++i) {
        float r     = (1 - std::pow(2, -3 * i / 30.0)) * 40;
        float alpha = (std::pow(1 - i / 30.0, 0.3) * 300 - 100) / 255;
        shader->set_uniform("r", r);
        shader->set_uniform("alpha", alpha);
        ren.set_viewport({i % 6 * 80, i / 6 * 80, 80, 80});
        ren.set_color();
        ren.draw({{}, {80, 80}});
    }
    ren.flush();

    ren.free();
    delete shader;
    delete framebuffer;

    return result;
}


} // namespace


void init() {

    s_shaders[SID_FLASH] = gfx::Shader::create(R"(
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
    s_shaders[SID_BUMP] = gfx::Shader::create(R"(
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

    s_textures[TID_SPRITES]   = android::load_texture("atlas.png");
    s_textures[TID_BUMP]      = android::load_texture("bump.png", gfx::FilterMode::Linear);
    s_textures[TID_SHOCKWAVE] = render_shockwave_texture();
    s_textures[TID_PRAXIS]    = render_praxis_texture();
}


void free() {
    for (auto s : s_shaders)  delete s;
    for (auto t : s_textures) delete t;
}


gfx::Texture2D* texture(TextureID t) { return s_textures[t]; }
gfx::Shader*    shader(ShaderID s) { return s_shaders[s]; }


} // namespace
