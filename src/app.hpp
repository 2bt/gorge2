#pragma once

namespace app {
    enum {
        WIDTH      = 360,
        MIN_HEIGHT = 590,
    };

    void init();
    void free();
    void resize(int width, int height);
    void update();
    void draw();
    void touch(int id, int action, int x, int y);
    void key(int key, int unicode);
}
