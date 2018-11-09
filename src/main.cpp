#include "fx.hpp"
#include "gfx.hpp"
#include "sprite.hpp"
#include "debug_renderer.hpp"
#include "world.hpp"
#include <SDL2/SDL.h>

DebugRenderer DB_REN;


class Game : public fx::App {
public:


    bool init() override {
        DB_REN.init();

        m_atlas_tex = gfx::Texture2D::create("media/atlas.png");
        m_ren.init();
        m_ren.set_texture(m_atlas_tex);

        m_world.init();

        rnd.seed();
        m_world.reset(rnd.get_int(0, 0x7fffffff));
        return true;
    }

    void free() override {
        m_world.free();

        delete m_atlas_tex;
        m_ren.free();

        DB_REN.free();
    }

    void resized() override {
        m_world.resized();
    }

    void update() override {

        const Uint8* ks = SDL_GetKeyboardState(nullptr);
        if (ks[SDL_SCANCODE_TAB]) {
            for (int i = 0; i < 7; ++i) m_world.update();
        }

        m_world.update();


        draw();
    }

    void draw() {
        // init transform
        m_ren.origin();
        float s = 2.0 / 150;
        float r = fx::screen_height() / (float) fx::screen_width();
        m_ren.scale({s * r, s});

//        m_ren.translate({0, 35});
//        m_ren.scale(0.25);
//        m_ren.scale(0.8);
        DB_REN.transform() = m_ren.transform();


        m_world.draw(m_ren);

//        // title
//        m_ren.draw(frame(Sprite::TITLE));

        m_ren.flush();
        DB_REN.flush();
    }

private:
    SpriteRenderer  m_ren;
    gfx::Texture2D* m_atlas_tex;

    World           m_world;
};


int main(int argc, char** argv) {


    Game game;
    return fx::run(game);
}
