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

        m_atlas = gfx::Texture2D::create("media/atlas.png");
        m_ren.init();
        m_ren.set_texture(m_atlas);

        m_world.init();

        m_world.reset(0);
        return true;
    }

    void free() override {
        m_world.free();

        delete m_atlas;
        m_ren.free();

        DB_REN.free();
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
        gfx::clear({0, 0, 0, 1});

        // init transform
        m_ren.origin();
        float s = 2.0 / 150;
        float r = fx::screen_height() / (float) fx::screen_width();
        m_ren.scale({s * r, s});
        DB_REN.transform() = m_ren.transform();

//        m_ren.translate({0, 35});
//        m_ren.scale(0.25);


        m_world.draw(m_ren);

//        // title
//        m_ren.draw(frame(Sprite::TITLE));

        m_ren.flush();
        DB_REN.flush();
    }

private:
    SpriteRenderer  m_ren;
    gfx::Texture2D* m_atlas;

    World           m_world;
};


int main(int argc, char** argv) {


    Game game;
	return fx::run(game);
}
