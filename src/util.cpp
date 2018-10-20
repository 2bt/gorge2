#include "util.hpp"
#include <limits>


void transform(vec2* dst, vec2 const* src, int len, vec2 const& pos, float ang) {
	float si = sinf(ang);
	float co = cosf(ang);
    for (int i = 0; i < len; ++i) {
        dst[i].x = pos.x + src[i].x *  co + src[i].y * si;
        dst[i].y = pos.y + src[i].x * -si + src[i].y * co;
    }
}


CollisionInfo polygon_collision(vec2 const* poly1, int len1, vec2 const* poly2, int len2) {
    CollisionInfo info = {};
    info.distance = std::numeric_limits<float>::max();

    for (int m = 0; m < 2; ++m) {
        for (int i = 0; i < len1; ++i) {
            vec2 const& p1 = poly1[(i ?: len1) - 1];
            vec2 const& p2 = poly1[i];
 
            CollisionInfo ci = {
                0,
                {p1.y - p2.y, p2.x - p1.x},
            };

            for (int j = 0; j < len2; ++j) {
                vec2 const& w = poly2[j];
                float d = glm::dot(p1 - w, ci.normal);
                if (d > ci.distance) {
                    ci.distance = d;
                    ci.where    = w;
                }
            }
            if (ci.distance == 0) return {};

            float l = glm::length(ci.normal);
            ci.distance /= l;

            if (ci.distance < info.distance) {
                info.distance = ci.distance;
                info.where    = ci.where;
                info.normal   = ci.normal / (m == 0 ? -l : l);
            }
        }

        std::swap(poly1, poly2);
        std::swap(len1, len2);
    }

    return info;
}

