#pragma once

namespace fx {

    struct App {
        virtual bool init() { return true; }
        virtual void free() {}
        virtual void key(int code) {}
        virtual void update() {}
        virtual void resized() {}
    };

    int run(App& App);

    struct Input {
        int  x;
        int  y;
        bool a;
        bool b;
    };

    Input const& input();

    int screen_width();
    int screen_height();
}
