#pragma once
#include "sprite_renderer.hpp"
#include "util.hpp"


class Bump {
public:

    void init();
    void free();
    void resized();
    void reset();

    void update();
    void draw_begin(SpriteRenderer& ren);
    void draw_end(SpriteRenderer& ren);

    void spawn(vec2 const& pos);

private:
    struct Wave {
        vec2  pos;
        float l;
    };
    std::vector<Wave> m_waves;
    gfx::Texture2D*   m_bump_canvas;
    gfx::Texture2D*   m_main_canvas;
    gfx::Framebuffer* m_bump_framebuffer;
    gfx::Framebuffer* m_main_framebuffer;
};
