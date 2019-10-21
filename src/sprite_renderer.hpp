#pragma once
#include "gfx.hpp"
#include <array>


class SpriteRenderer {
public:
    using vec2  = glm::vec2;
    using Color = glm::u8vec4;
    struct Rect { vec2 pos, size; };

    enum BlendMode { BM_ALPHA, BM_ADD };

    void init();
    void free();

    void push();
    void pop();

    void origin();
    void translate(vec2 const& v);
    void rotate(float r);
    void scale(vec2 const& v);
    void scale(float s) { scale({s, s}); }

    void push_state();
    void pop_state();

    void set_color() { set_color({ 255, 255, 255, 255 }); }
    void set_color(glm::u8vec3 const& c) { set_color(Color(c, 255)); }
    void set_color(Color const& c);
    void set_blendmode(BlendMode b);
    void set_viewport(gfx::Rect const& r);
    void set_texture(gfx::Texture2D* tex = nullptr);
    void set_shader(gfx::Shader* shader = nullptr);
    void set_framebuffer(gfx::RenderTarget* fb);

    void clear();
    void draw(Rect const& quad, vec2 const& pos = {});
    void draw(Rect const& quad, vec2 const& pos, float ang);
    void draw(vec2 const& pos = {});

    void flush();

    glm::mat3x2& transform() { return m_transforms[m_transform_index]; }

private:

    struct State {
        Color              color       = { 255, 255, 255, 255 };
        BlendMode          blendmode   = BM_ALPHA;
        gfx::Rect          viewport    = {0, 0, 0, 0};
        gfx::Texture2D*    tex         = nullptr;
        gfx::Shader*       shader      = nullptr;
        gfx::RenderTarget* framebuffer = nullptr;
    };


    State& state() { return m_states[m_state_index]; }

    struct Vert {
        glm::vec2   pos;
        glm::vec2   tex_coord;
        Color       color;
        Vert(vec2 const& p, vec2 const& t, Color const& c)
            : pos(p), tex_coord(t), color(c) {}
    };

    std::vector<Vert>          m_verts;

    gfx::RenderState           m_rs;
    gfx::VertexArray*          m_va;
    gfx::VertexBuffer*         m_vb;
    gfx::Shader*               m_default_shader;
    gfx::Texture2D*            m_default_tex;

    std::array<State, 4>       m_states;
    int                        m_state_index = 0;

    std::array<glm::mat3x2, 4> m_transforms;
    int                        m_transform_index = 0;
};
