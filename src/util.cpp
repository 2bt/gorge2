#include "util.hpp"

void transform(vec2* dst, vec2 const* src, int len, vec2 const& pos, float ang) {
	float si = sinf(ang);
	float co = cosf(ang);
    for (int i = 0; i < len; ++i) {
        dst[i].x = pos.x + src[i].x *  co + src[i].y * si;
        dst[i].y = pos.y + src[i].x * -si + src[i].y * co;
    }
}
