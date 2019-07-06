#include "gfx.hpp"
#include "fx.hpp"
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace fx {
namespace {


SDL_Window*   s_window;
bool          s_running       = true;
int           s_screen_width  = 800;
int           s_screen_height = 600;
SDL_GLContext s_gl_context;

Input         s_input;


void free() {
    SDL_GL_DeleteContext(s_gl_context);
    SDL_DestroyWindow(s_window);
    SDL_Quit();
    IMG_Quit();
}


} // namespace


int run(App& app) {
    s_screen_width  = 320 + 32;
    s_screen_height = 200 + 32,


    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // no depth buffer
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);

    s_window = SDL_CreateWindow(
            "app",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            s_screen_width, s_screen_height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    s_gl_context = SDL_GL_CreateContext(s_window);
    if (!s_gl_context) {
        fprintf(stderr, "error: SDL_GL_CreateContext() failed\n");
        free();
        return 1;
    }

    if (!gfx::init()) {
        fprintf(stderr, "error: gfx::init() failed\n");
        free();
        return 1;
    }

    SDL_GL_SetSwapInterval(1); // v-sync


    if (!app.init()) {
        fprintf(stderr, "error: app.init() failed\n");
        gfx::free();
        free();
        return 1;
    }

    while (s_running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                s_running = false;
                break;

            case SDL_KEYDOWN:
                if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) s_running = false;
                else app.key(e.key.keysym.scancode);
                break;

            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    s_screen_width  = e.window.data1;
                    s_screen_height = e.window.data2;
                    app.resized();
                }
                break;

            default: break;
            }
        }


        const Uint8* ks = SDL_GetKeyboardState(nullptr);
        s_input.x = !!ks[SDL_SCANCODE_RIGHT] - !!ks[SDL_SCANCODE_LEFT];
        s_input.y = !!ks[SDL_SCANCODE_DOWN] - !!ks[SDL_SCANCODE_UP];
        s_input.a = !!ks[SDL_SCANCODE_X];
        s_input.b = !!ks[SDL_SCANCODE_Y] | !!ks[SDL_SCANCODE_Z];

        app.update();

        SDL_GL_SwapWindow(s_window);
    }


    app.free();
    gfx::free();
    free();

    return 0;
}

int screen_width()  { return s_screen_width; }
int screen_height() { return s_screen_height; }
Input const& input() { return s_input; }


} // namespace
