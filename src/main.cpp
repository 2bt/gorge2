#include "fx.hpp"
#include "gfx.hpp"
#include "sprite_renderer.hpp"
#include "sprite.hpp"
#include "background.hpp"
#include "wall.hpp"
#include "player.hpp"

#include <SDL2/SDL.h>


class Game : public fx::App {
public:


    bool init() override {
        m_background.init();

        m_atlas = gfx::Texture2D::create("media/atlas.png");
        m_ren.init();
        m_ren.set_texture(m_atlas);


        // reset randomly
        m_random.seed();
        reset(m_random.get_int(0, 0x7fffffff));

        return true;
    }

    void free() override {
        m_background.free();

        delete m_atlas;
        m_ren.free();
    }

    void reset(uint32_t seed) {
        m_random.seed(seed);
        m_background.reset(m_random.get_int(0, 0x7fffffff));
        m_wall.reset(m_random.get_int(0, 0x7fffffff));
        m_player.reset();
    }


    void update() override {

        const Uint8* ks = SDL_GetKeyboardState(nullptr);
        if (ks[SDL_SCANCODE_TAB]) {
            for (int i = 0; i < 7; ++i) tick();
        }

        tick();
        draw();
    }

    void tick() {

        const Uint8* ks = SDL_GetKeyboardState(nullptr);
        Player::Input input = {
            ks[SDL_SCANCODE_RIGHT] - ks[SDL_SCANCODE_LEFT],
            ks[SDL_SCANCODE_DOWN] - ks[SDL_SCANCODE_UP],
            ks[SDL_SCANCODE_X],
            ks[SDL_SCANCODE_Y] | ks[SDL_SCANCODE_Z],
        };

        m_background.update();
        m_wall.update();
        m_player.update(input);
    }

    void draw() {
        gfx::clear({0, 0, 0, 1});

        // init transform
        m_ren.origin();
        float s = 2.0 / 150;
        float r = fx::screen_height() / (float) fx::screen_width();
        m_ren.scale({s * r, s});

//        m_ren.translate({0, 35});
//        m_ren.scale(0.25);

        // background
        m_background.draw(m_ren);

        // wall
        m_wall.draw(m_ren);

        m_player.draw(m_ren);

//        // title
//        m_ren.draw(frame(Sprite::TITLE));


        m_ren.flush();
    }

private:
    SpriteRenderer  m_ren;
    gfx::Texture2D* m_atlas;

    Random          m_random;
    Background      m_background;
    Wall            m_wall;
    Player          m_player;
};


int main(int argc, char** argv) {



    Game game;
	return fx::run(game);
}
