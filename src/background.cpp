#include "background.hpp"
#include "log.hpp"


void Background::reset_star(Star& s) {
	s.frame = m_random.get_int(0, 3);
	float b = m_random.get_float(0.3, 1);
	float c = (b - 0.3) * 0.4;
	float d = c * m_random.get_float(1, 2);
	s.color = { c * 255, d * 255, d * 255, 255 };
	s.vel = b * 0.127;
	s.pos.x = m_random.get_float(-135, 135);
	s.pos.y = -77;
}


void Background::init() {
    const int size = 128;
    Random rand;
    rand.seed(42);
    std::vector<uint8_t> buf(size * size * 4);
    for (uint8_t& b : buf) b = rand.get_int(0, 255);
    m_noise_tex = gfx::Texture2D::create(gfx::TextureFormat::RGBA, size, size, (void*)buf.data(),
                                         gfx::FilterMode::Linear, gfx::WrapMode::Repeat);

    m_cloud_shader = gfx::Shader::create(R"(
        #version 100
        attribute vec2 in_pos;
        void main() {
            gl_Position = vec4(in_pos.x, -in_pos.y, 0.0, 1.0);
        })", R"(
        #version 100
        precision highp float;
        uniform sampler2D tex;
        uniform float tick;
        float perlin(vec2 p) {
            float c = texture2D(tex, p * 0.0625).r
                    + texture2D(tex, p * 0.125 ).g * 0.5
                    + texture2D(tex, p * 0.25  ).b * 0.25
                    + texture2D(tex, p * 0.5   ).a * 0.125
                    + texture2D(tex, p         ).r * 0.0625;
            return c / 2.0;
        }
        void main() {
            vec2 p = (-gl_FragCoord.xy + vec2(0.0, tick)) * 0.0016;
            float f = max(0.0, pow(perlin(p), 1.0) - 0.41);
            f = floor(f * 16.0) / 16.0;
            if (f > 0.0) f += 0.1;
            f *= f * 1.2;
            vec3 c = vec3(0.4, 0.5, 0.5) * f;
            gl_FragColor = vec4(c, 1.0);
        })");

    m_canvas = gfx::Texture2D::create(gfx::TextureFormat::RGB, 8 * 36, 151);
    m_framebuffer = gfx::Framebuffer::create();
    m_framebuffer->attach_color(m_canvas);
}

void Background::free() {
    delete m_noise_tex;
    delete m_cloud_shader;
    delete m_framebuffer;
    delete m_canvas;
}

void Background::reset(uint32_t seed) {
    m_random.seed(seed);
	m_tick = m_random.get_int(0, 999999);
    for (Star& s : m_stars) {
        reset_star(s);
		s.pos.y = m_random.get_float(-77, 77);
    }
}

void Background::update() {
    ++m_tick;

    for (Star& s : m_stars) {
		s.pos.y += s.vel;
		if (s.pos.y > 77) {
			reset_star(s);
		}
	}
}

void Background::draw(SpriteRenderer& ren) {
    ren.set_color();

    if (m_tick % 10 == 0) {
        m_cloud_shader->set_uniform("tick", float(m_tick / 10));
        ren.push();
        ren.push_state();

        ren.origin();
        ren.set_shader(m_cloud_shader);
        ren.set_texture(m_noise_tex);
        ren.set_framebuffer(m_framebuffer);
        ren.draw({{}, {2, 2}});

        ren.pop();
        ren.pop_state();
    }

    ren.push_state();
    ren.set_texture(m_canvas);
    ren.draw({0, m_tick % 10 * 0.1 - 0.5});
    ren.pop_state();

    ren.push_state();
    ren.set_blendmode(SpriteRenderer::BM_ADD);
    for (Star& s : m_stars) {
		ren.set_color(s.color);
        ren.draw(frame(Sprite::STARS, s.frame), s.pos);
	}

    ren.pop_state();


    ren.flush();
}
