#!/usr/bin/python
from PIL import Image
import os


sprite_sizes = {
    "title.png": (121, 21),
    "walls.png": (8, 8),
    "font.png": (8, 8),
}

W = 256
H = 128


atlas = Image.new("RGBA", (W, H))
heights = [0] * W


def add_to_atlas(sprite):
    found = False
    yy = H - sprite.height
    xx = 0

    for x in range(W - sprite.width):
        y = 0
        for i in range(sprite.width):
            y = max(y, heights[x + i])
        if y < yy:
            yy = y
            xx = x
            found = True

    assert found

    for i in range(sprite.width):
        heights[xx + i] = yy + sprite.height

    atlas.paste(sprite, (xx, yy))
    return (xx, yy)


def clamp(n, smallest, largest): return max(smallest, min(n, largest))


def shrink(sprite):

    flag = 0

    while sprite.height > 0:
        success = True
        for x in range(sprite.width):
            c1 = sprite.getpixel((x, 0))
            c2 = sprite.getpixel((x, sprite.height - 1))
            if c1 == c2 == (0, 0, 0, 0): continue
            success = False
            break
        if not success: break
        sprite = sprite.crop((0, 1, sprite.width, sprite.height - 1))
        flag |= 1

    while sprite.width > 0:
        success = True
        for y in range(sprite.height):
            c1 = sprite.getpixel((0, y))
            c2 = sprite.getpixel((sprite.width - 1, y))
            if c1 == c2 == (0, 0, 0, 0): continue
            success = False
            break
        if not success: break
        sprite = sprite.crop((1, 0, sprite.width - 1, sprite.height))
        flag |= 2

    if sprite.width == 0: return sprite, False

    new_sprite = Image.new("RGBA", (sprite.width + 2, sprite.height + 2))
    new_sprite.paste(sprite, (1, 1))

    if flag == 0:
        for x in range(new_sprite.width):
            new_sprite.putpixel((x, 0),
                sprite.getpixel((clamp(x - 1, 0, sprite.width - 1), 0)))
            new_sprite.putpixel((x, new_sprite.height - 1),
                sprite.getpixel((clamp(x - 1, 0, sprite.width - 1), sprite.height - 1)))

        for y in range(sprite.height):
            new_sprite.putpixel((0, y + 1),
                sprite.getpixel((0, y)))
            new_sprite.putpixel((new_sprite.height - 1, y + 1),
                sprite.getpixel((sprite.height - 1, y)))

    return new_sprite, True



info = {}


for f in os.listdir("."):
    if not f.endswith(".png") or f.startswith("_"): continue

    rects = []

    img = Image.open(f)
    w, h = sprite_sizes.get(f, (img.height, img.height))
    for y in range(0, img.height, h):
        for x in range(0, img.width, w):
            sprite = img.crop((x, y, x + w, y + h))
            sprite, border = shrink(sprite)
            pos = add_to_atlas(sprite)
            size = (sprite.width, sprite.height)
            pos  = (pos[0] + border, pos[1] + border)
            size = (size[0] - border * 2, size[1] - border * 2)

            rects.append((pos, size))

    info[f[:-4].upper()] = rects

atlas.save("../media/atlas.png")



# generate code
f = open("../src/sprite.hpp", "w")

keys = sorted(info.keys())
print >>f, """#pragma once
#include "types.hpp"

enum class Sprite {"""
for k in keys: print >>f, "    %s," % k
print >>f, "};"
print >>f, """
Rect const& frame(Sprite s, int frame = 0);
int frame_count(Sprite s);
"""

f.close()
f = open("../src/sprite.cpp", "w")

print >>f, """#include "sprite.hpp"

namespace {"""
print >>f
print >>f, "Rect sprites[] = {"
for k in keys:
    print >>f, "    // %s" % k
    for p, s in info[k]:
        print >>f, "    { { %d, %d }, { %d, %d } }," % (p + s)
print >>f, "};"
print >>f
print >>f, "int offsets[] = {"
o = 0
for k in keys:
    print >>f, "    %d," % o
    o += len(info[k])
print >>f, "    %d" % o
print >>f, "};"
print >>f
print >>f, "}"
print >>f, """
Rect const& frame(Sprite s, int frame) {
    return sprites[offsets[static_cast<int>(s)] + frame];
}

int frame_count(Sprite s) {
    return offsets[static_cast<int>(s) + 1] - offsets[static_cast<int>(s)];
}
"""
f.close()
