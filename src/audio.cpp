#include "audio.hpp"
#include "log.hpp"
#include "foo.hpp"
#include <algorithm>

#define AL_LIBTYPE_STATIC
#include <AL/al.h>
#include <AL/alc.h>


namespace audio {
namespace {

struct Wave {
    char     riff[4];
    uint32_t size;
    char     format[4];
    char     fmt_id[4];
    uint32_t fmt_size;
    uint16_t fmt_format;
    uint16_t fmt_channels;
    uint32_t fmt_sample_rate;
    uint32_t fmt_byte_rate;
    uint16_t fmt_block_align;
    uint16_t fmt_bits_per_sample;
    char     data_id[4];
    uint32_t data_size;
    uint8_t  data[0];
};


ALCdevice*                     m_al_device;
ALCcontext*                    m_al_context;
std::array<ALuint, SoundCount> m_al_buffers;
std::vector<ALuint>            m_al_sources;

struct MetaSource {
    char const*      file_name;
    std::vector<int> source_indices;
    MetaSource(char const* f, size_t c) : file_name(f), source_indices(c) {}
};

int                                m_source_id_counter;
std::vector<int>                   m_source_ids;
std::array<MetaSource, SoundCount> m_meta_source = {
    MetaSource{ "back.wav",          1 },
    MetaSource{ "big_explosion.wav", 2 },
    MetaSource{ "blast.wav",         1 },
    MetaSource{ "bullet.wav",        2 },
    MetaSource{ "coin.wav",          1 },
    MetaSource{ "collect.wav",       1 },
    MetaSource{ "drop.wav",          1 },
    MetaSource{ "engine.wav",        2 },
    MetaSource{ "explosion.wav",     2 },
    MetaSource{ "field.wav",         1 },
    MetaSource{ "hit.wav",           1 },
    MetaSource{ "laser.wav",         1 },
    MetaSource{ "miss.wav",          2 },
    MetaSource{ "pause.wav",         1 },
    MetaSource{ "plasma.wav",        1 },
    MetaSource{ "saucer.wav",        1 },
    MetaSource{ "select.wav",        1 },
    MetaSource{ "spider.wav",        2 },
};



} // namespace


bool init() {
    m_al_device = alcOpenDevice(nullptr);
    if (!m_al_device) {
        LOGE("audio::init: alcOpenDevice");
        return false;
    }

    m_al_context = alcCreateContext(m_al_device, nullptr);
    alcMakeContextCurrent(m_al_context);

    alGenBuffers(m_al_buffers.size(), m_al_buffers.data());

    int source_count = 0;
    for (auto const& m : m_meta_source) source_count += m.source_indices.size();
    m_source_ids.resize(source_count);
    m_al_sources.resize(source_count);
    alGenSources(m_al_sources.size(), m_al_sources.data());

    int j = 0;
    for (int i = 0; i < (int) m_meta_source.size(); ++i) {
        MetaSource& m = m_meta_source[i];
        std::vector<uint8_t> buf;
        if (!android::load_asset(m.file_name, buf)) return false;
        Wave& w = *(Wave*) buf.data();
        int format = 0;
        if      (w.fmt_channels == 1 && w.fmt_bits_per_sample == 8)  format = AL_FORMAT_MONO8;
        else if (w.fmt_channels == 1 && w.fmt_bits_per_sample == 16) format = AL_FORMAT_MONO16;
        else if (w.fmt_channels == 2 && w.fmt_bits_per_sample == 8)  format = AL_FORMAT_STEREO8;
        else if (w.fmt_channels == 2 && w.fmt_bits_per_sample == 16) format = AL_FORMAT_STEREO16;
        else LOGE("audio::init: %s: unknown format", m.file_name);

        alBufferData(m_al_buffers[i], format, w.data, w.data_size, w.fmt_sample_rate);

        for (int& index : m.source_indices) {
            index = j++;
            alSourcei(m_al_sources[index], AL_BUFFER, m_al_buffers[i]);
        }
    }

    alListener3f(AL_POSITION, 0, 0, -200);
    alDistanceModel(AL_LINEAR_DISTANCE);
    //alDistanceModel(AL_NONE);

    return true;
}

void free() {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_al_context);
    alDeleteBuffers(m_al_buffers.size(), m_al_buffers.data());
    alcCloseDevice(m_al_device);
}

template<class Func>
void sound_do(int id, Func const& f) {
    for (size_t i = 0; i < m_source_ids.size(); ++i) {
        if (m_source_ids[i] == id) {
            f(m_al_sources[i]);
            break;
        }
    }
}

int get_sound(SoundType s) {
    MetaSource& m = m_meta_source[s];
    int index = m.source_indices.front();
    alSourceStop(m_al_sources[index]);
    std::rotate(m.source_indices.begin(), m.source_indices.begin() + 1, m.source_indices.end());

    return m_source_ids[index] = ++m_source_id_counter;
}
void set_sound_position(int id, vec2 const& pos) {
    sound_do(id, [&pos](ALuint s) { alSource3f(s, AL_POSITION, pos.x, pos.y, 0); });
}
void set_sound_pitch(int id, float pitch) {
    sound_do(id, [pitch](ALuint s) { alSourcef(s, AL_PITCH, pitch); });
}
void set_sound_playing(int id, bool playing, bool looping) {
    sound_do(id, [playing, looping](ALuint s) {
        if (!playing) {
            alSourceStop(s);
        }
        else {
            alSourcei(s, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
            alSourcePlay(s);
        }
    });
}
void play_sound(SoundType s, vec2 const& pos) {
    int id = get_sound(s);
    set_sound_position(id, pos);
    set_sound_playing(id, true);
}


} // namespace
