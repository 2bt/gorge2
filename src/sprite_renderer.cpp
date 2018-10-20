#include "sprite_renderer.hpp"
#include <cassert>


void SpriteRenderer::init() {
    m_default_shader = gfx::Shader::create(R"(
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
            gl_FragColor = ex_color * texture2D(tex, ex_tex_coord);
        })");
    state().shader = m_default_shader;

    uint32_t buf = ~0;
    m_default_tex = gfx::Texture2D::create(gfx::TextureFormat::RGBA, 1, 1, &buf);
    set_texture();

    m_rs.cull_face_enabled    = false;
    m_rs.blend_enabled        = true;

    m_rs.blend_func_src_rgb   = gfx::BlendFunc::SrcAlpha;
    m_rs.blend_func_src_alpha = gfx::BlendFunc::SrcAlpha;
    m_rs.blend_func_dst_rgb   = gfx::BlendFunc::OneMinusSrcAlpha;
    m_rs.blend_func_dst_alpha = gfx::BlendFunc::OneMinusSrcAlpha;
//    m_rs.blend_func_dst_rgb   = gfx::BlendFunc::One;
//    m_rs.blend_func_dst_alpha = gfx::BlendFunc::One;

    m_vb = gfx::VertexBuffer::create(gfx::BufferHint::StreamDraw);
    m_va = gfx::VertexArray::create();
    m_va->set_primitive_type(gfx::PrimitiveType::Triangles);
    m_va->set_attribute(0, m_vb, gfx::ComponentType::Float, 2, false, 0, sizeof(Vert));
    m_va->set_attribute(1, m_vb, gfx::ComponentType::Float, 2, false, 8, sizeof(Vert));
    m_va->set_attribute(2, m_vb, gfx::ComponentType::Uint8, 4, true, 16, sizeof(Vert));

}

void SpriteRenderer::free() {
    delete m_va;
    delete m_vb;
    delete m_default_shader;
    delete m_default_tex;
}

void SpriteRenderer::push() {
    assert(m_transform_index < (int) m_transforms.size() - 2);
    ++m_transform_index;
    transform() = m_transforms[m_transform_index - 1];
}

void SpriteRenderer::pop() {
    assert(m_transform_index > 0);
    --m_transform_index;
}

void SpriteRenderer::push_state() {
    assert(m_state_index < (int) m_states.size() - 2);
    ++m_state_index;
    state() = m_states[m_state_index - 1];
}

void SpriteRenderer::pop_state() {
    assert(m_state_index > 0);
    set_color(m_states[m_state_index - 1].color);
    set_blendmode(m_states[m_state_index - 1].blendmode);
    set_texture(m_states[m_state_index - 1].tex);
    set_shader(m_states[m_state_index - 1].shader);
    set_framebuffer(m_states[m_state_index - 1].framebuffer);
    --m_state_index;
}

void SpriteRenderer::origin() {
    transform() = { 1, 0, 0, 1, 0, 0 };
}

void SpriteRenderer::translate(vec2 const& v) {
    transform()[2][0] += v.x * transform()[0][0];
    transform()[2][1] += v.y * transform()[1][1];
}

void SpriteRenderer::rotate(float r) {
    float si = sinf(r);
    float co = cosf(r);
    auto rot = glm::mat2(transform()) * glm::mat2(co, si, -si, co);
    transform()[0] = rot[0];
    transform()[1] = rot[1];
}

void SpriteRenderer::scale(vec2 const& v) {
    transform()[0][0] *= v.x;
    transform()[1][1] *= v.y;
}

void SpriteRenderer::set_color(Color c) {
    state().color = c;
}

void SpriteRenderer::draw(vec2 const& pos) {
    draw({{}, {state().tex->get_width(), state().tex->get_height()}}, pos);
}

void SpriteRenderer::draw(Rect const& quad, vec2 const& pos) {

    vec2 const& s = quad.size;
    Color       c = state().color;

    Vert vs[] = {
        Vert(transform() * glm::vec3(pos + s * vec2(-0.5, -0.5), 1), quad.pos + vec2(  0,   0), c),
        Vert(transform() * glm::vec3(pos + s * vec2( 0.5, -0.5), 1), quad.pos + vec2(s.x,   0), c),
        Vert(transform() * glm::vec3(pos + s * vec2(-0.5,  0.5), 1), quad.pos + vec2(  0, s.y), c),
        Vert(transform() * glm::vec3(pos + s * vec2( 0.5,  0.5), 1), quad.pos + vec2(s.x, s.y), c),
    };

    m_verts.push_back(vs[0]);
    m_verts.push_back(vs[1]);
    m_verts.push_back(vs[2]);

    m_verts.push_back(vs[2]);
    m_verts.push_back(vs[1]);
    m_verts.push_back(vs[3]);
}

void SpriteRenderer::set_blendmode(BlendMode b) {
    if (state().blendmode == b) return;
    flush();
    state().blendmode = b;

    if (state().blendmode == BM_ALPHA) {
        m_rs.blend_func_dst_rgb   = gfx::BlendFunc::OneMinusSrcAlpha;
        m_rs.blend_func_dst_alpha = gfx::BlendFunc::OneMinusSrcAlpha;
    }
    else {
        m_rs.blend_func_dst_rgb   = gfx::BlendFunc::One;
        m_rs.blend_func_dst_alpha = gfx::BlendFunc::One;
    }
}

void SpriteRenderer::set_shader(gfx::Shader* shader) {
    if (!shader) shader = m_default_shader;
    if (state().shader == shader) return;
    flush();
    state().shader = shader;
}

void SpriteRenderer::set_texture(gfx::Texture2D* tex) {
    if (!tex) tex = m_default_tex;
    if (state().tex == tex) return;
    flush();
    state().tex = tex;
}

void SpriteRenderer::set_framebuffer(gfx::Framebuffer* fb) {
    if (state().framebuffer == fb) return;
    flush();
    state().framebuffer = fb;
}

void SpriteRenderer::flush() {
    if (m_verts.empty()) return;

    gfx::Texture2D* t = state().tex;
    if (state().shader->has_uniform("tex")) {
        state().shader->set_uniform("tex", t);
    }

    if (state().shader->has_uniform("tex_scale")) {
        state().shader->set_uniform("tex_scale", vec2(1.0 / t->get_width(), 1.0 / t->get_height()));
    }

    m_vb->init_data(m_verts);
    m_va->set_count(m_verts.size());
    m_verts.clear();
    gfx::draw(m_rs, state().shader, m_va, state().framebuffer);
}

