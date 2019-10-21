#pragma once
#include "gfx.hpp"
#include "sprite.hpp"
#include "util.hpp"
#include <array>

class Background {
public:
    void init();
    void free();
    void reset(uint32_t seed);
    void update();
    void draw(SpriteRenderer& ren);

private:
    struct Star {
        vec2  pos;
        float vel;
        Color color;
        int   frame;
    };

    void reset_star(Star& s);

    Random                m_random;
    int                   m_tick;
    std::array<Star, 200> m_stars;
    gfx::Texture2D*       m_noise_tex;
    gfx::Shader*          m_cloud_shader;
    gfx::Framebuffer*     m_framebuffer;
    gfx::Texture2D*       m_canvas;
    bool                  m_first_frame;
};

