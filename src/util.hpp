#pragma once
#include <glm/glm.hpp>
#include "random.hpp"


using vec2 = glm::vec2;
using Color = glm::u8vec4;


extern Random rnd;


template<class T>
T clamp(T v, T minv, T maxv) {
    return glm::max(minv, glm::min(maxv, v));
}

void transform(vec2* dst, vec2 const* src, int len, vec2 const& pos, float ang = 0);
template<class T1, class T2>
void transform(T1& dst, T2 const& src, vec2 const& pos, float ang = 0) {
    transform(dst.data(), src.data(), dst.size(), pos, ang);
}


struct CollisionInfo {
    float distance;
    vec2  normal;
    vec2  where;
};


CollisionInfo polygon_collision(vec2 const* poly1, int len1, vec2 const* poly2, int len2);
template<class T1, class T2>
CollisionInfo polygon_collision(T1 const& poly1, T2 const& poly2) {
    return polygon_collision(poly1.data(), poly1.size(), poly2.data(), poly2.size());
}
