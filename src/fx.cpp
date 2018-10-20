#include "gfx.hpp"
#include "fx.hpp"
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace fx {
namespace {


SDL_Window*   m_window;
bool          m_running       = true;
int           m_screen_width  = 800;
int           m_screen_height = 600;
SDL_GLContext m_gl_context;

Input         m_input;


void free() {
    SDL_GL_DeleteContext(m_gl_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    IMG_Quit();
}


} // namespace


int run(App& app) {
    m_screen_width  = 320 + 32;
    m_screen_height = 200 + 32,


    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // no depth buffer
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);

    m_window = SDL_CreateWindow(
            "app",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            m_screen_width, m_screen_height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    m_gl_context = SDL_GL_CreateContext(m_window);
    if (!m_gl_context) {
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

    while (m_running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                m_running = false;
                break;

            case SDL_KEYDOWN:
                if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) m_running = false;
                else app.key(e.key.keysym.scancode);
                break;

            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    m_screen_width  = e.window.data1;
                    m_screen_height = e.window.data2;
                }
                break;

            default: break;
            }
        }


        const Uint8* ks = SDL_GetKeyboardState(nullptr);
        m_input.x = !!ks[SDL_SCANCODE_RIGHT] - !!ks[SDL_SCANCODE_LEFT];
        m_input.y = !!ks[SDL_SCANCODE_DOWN] - !!ks[SDL_SCANCODE_UP];
        m_input.a = !!ks[SDL_SCANCODE_X];
        m_input.b = !!ks[SDL_SCANCODE_C];

        app.update();

        SDL_GL_SwapWindow(m_window);
    }


    app.free();
    gfx::free();
    free();

    return 0;
}

int screen_width()  { return m_screen_width; }
int screen_height() { return m_screen_height; }
Input const& input() { return m_input; }


} // namespace
