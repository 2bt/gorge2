#pragma once
#include "gfx.hpp"


namespace resource {

    void init();
    void free();

    enum ShaderID {
        SID_FLASH,
        SID_BUMP,
        SID_COUNT
    };

    enum TextureID {
        TID_SPRITES,
        TID_BUMP,
        TID_SHOCKWAVE,
        TID_PRAXIS,
        TID_COUNT
    };

    gfx::Shader*    shader(ShaderID s);
    gfx::Texture2D* texture(TextureID t);
}
