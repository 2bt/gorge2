#pragma once
#include <glm/glm.hpp>

using vec2 = glm::vec2;
using Color = glm::u8vec4;

template<class T>
T clamp(T v, T minv, T maxv) {
    return std::max(minv, std::min(maxv, v));
}

void transform(vec2* dst, vec2 const* src, int len, vec2 const& pos, float ang = 0);

template<class T1, class T2>
void transform(T1& dst, T2 const& src, vec2 const& pos, float ang = 0) {
    transform(dst.data(), src.data(), dst.size(), pos, ang);
}
