#include "log.hpp"
#include "foo.hpp"
#include <array>
#include <AL/al.h>
#include <AL/alc.h>

namespace audio {
namespace {

ALCdevice*            m_device;
ALCcontext*           m_context;
std::array<ALuint, 1> m_buffers;

} // namespace


void init() {
    m_device = alcOpenDevice(nullptr);
    if (!m_device) {
        LOGE("audio::init: alcOpenDevice");
        return;
    }

    m_context = alcCreateContext(m_device, nullptr);
    alcMakeContextCurrent(m_context);

    ALenum err;

    alGenBuffers(m_buffers.size(), m_buffers.data());
    if ((err = alGetError()) != AL_NO_ERROR) {
        LOGE("audio::init: alGenBuffers: %d", err);
        return;
    }

    std::array<char const*, 1> names = {
        "laser.wav",
    };


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
        uint16_t data_size;
        uint8_t  data[0];
    };

    for (int i = 0; i < (int) names.size(); ++i) {
        std::vector<uint8_t> buf;
        android::load_asset(names[i], buf);
        Wave& w = *(Wave*) buf.data();
        int format = 0;
        if      (w.fmt_channels == 1 && w.fmt_bits_per_sample == 8)  format = AL_FORMAT_MONO8;
        else if (w.fmt_channels == 1 && w.fmt_bits_per_sample == 16) format = AL_FORMAT_MONO16;
        else if (w.fmt_channels == 2 && w.fmt_bits_per_sample == 8)  format = AL_FORMAT_STEREO8;
        else if (w.fmt_channels == 2 && w.fmt_bits_per_sample == 16) format = AL_FORMAT_STEREO16;
        else  LOGE("audio::init: %s: unknown format", names[i]);
        alBufferData(m_buffers[i], format, w.data, w.data_size, w.fmt_sample_rate);
    }


//    // Generate Sources
//    std::array<ALuint, 1> sources;
//    alGenSources(sources.size(), sources.data());
//    alSourcei(sources[0], AL_BUFFER, m_buffers[0]);
//    alSourcePlay(sources[0]);
}

void free() {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_context);
    alDeleteBuffers(m_buffers.size(), m_buffers.data());
    alcCloseDevice(m_device);
}




} // namespace
