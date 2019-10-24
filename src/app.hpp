#pragma once
#include <array>
#include <glm/glm.hpp>

namespace app {
    struct Touch {
        glm::vec2 pos;
        bool      pressed;
        bool      prev_pressed;
    };

    std::array<Touch, 3> const& touches();
    glm::ivec2 const& screen_size();

    void init();
    void free();
    void resize(int width, int height);
    void update();
    void draw();
    void touch(int id, bool pressed, int x, int y);
    void key(int key, int unicode);
}
