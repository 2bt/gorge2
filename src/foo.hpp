#pragma once
#include "gfx.hpp"

namespace android {

bool load_asset(std::string const& name, std::vector<uint8_t>& buf);
gfx::Texture2D* load_texture(char const*     name,
                             gfx::FilterMode filter = gfx::FilterMode::Nearest,
                             gfx::WrapMode   wrap   = gfx::WrapMode::Clamp);

//std::string get_storage_dir();
//void show_keyboard();
//void hide_keyboard();

//bool load_pref(char const* name, bool value);
//int  load_pref(char const* name, int value);
//void store_pref(char const* name, bool value);
//void store_pref(char const* name, int value);
//void store_pref_apply();

} // namespace
