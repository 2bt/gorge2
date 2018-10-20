#pragma once
#include <glm/glm.hpp>

using vec2 = glm::vec2;
using Color = glm::u8vec4;

template<class T>
T clamp(T v, T minv, T maxv) {
    return std::max(minv, std::min(maxv, v));
}
