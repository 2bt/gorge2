#pragma once

#include "util.hpp"

namespace audio {

    enum SoundType {
        ST_BACK,
        ST_BIG_EXPLOSION,
        ST_BLAST,
        ST_BULLET,
        ST_COIN,
        ST_COLLECT,
        ST_DROP,
        ST_ENGINE,
        ST_EXPLOSION,
        ST_FIELD,
        ST_HIT,
        ST_LASER,
        ST_MISS,
        ST_PAUSE,
        ST_PLASMA,
        ST_SAUCER,
        ST_SELECT,
        ST_SPIDER,

        SoundCount,
    };

    bool init();
    void free();

    void play_sound(SoundType s, vec2 const& pos = {});
    int get_sound(SoundType s);
    void set_sound_position(int id, vec2 const& pos);
    void set_sound_pitch(int id, float pitch);
    void set_sound_playing(int id, bool playing, bool looping = false);


} // namespace
