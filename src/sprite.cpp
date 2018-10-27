#include "sprite.hpp"

namespace {

SpriteRenderer::Rect sprites[] = {
    // BALL
    { { 96, 93 }, { 4, 4 } },
    { { 127, 94 }, { 4, 4 } },
    { { 167, 94 }, { 4, 4 } },
    { { 173, 94 }, { 4, 4 } },
    { { 179, 94 }, { 4, 4 } },
    { { 1, 95 }, { 4, 4 } },
    { { 7, 95 }, { 4, 4 } },
    { { 45, 95 }, { 4, 4 } },
    { { 51, 95 }, { 4, 4 } },
    { { 102, 96 }, { 4, 4 } },
    // BALL_FIELD
    { { 93, 27 }, { 6, 6 } },
    { { 101, 27 }, { 8, 8 } },
    { { 111, 27 }, { 10, 10 } },
    { { 123, 27 }, { 8, 8 } },
    // BALL_ITEM
    { { 221, 27 }, { 4, 4 } },
    { { 227, 27 }, { 6, 6 } },
    { { 235, 27 }, { 6, 6 } },
    { { 243, 27 }, { 6, 6 } },
    // BLOCKADE
    { { 41, 11 }, { 8, 6 } },
    // CANNON
    { { 179, 78 }, { 8, 8 } },
    { { 81, 77 }, { 6, 10 } },
    // ENERGY_ITEM
    { { 161, 25 }, { 4, 4 } },
    { { 167, 25 }, { 4, 4 } },
    { { 173, 25 }, { 4, 4 } },
    { { 179, 25 }, { 4, 4 } },
    { { 185, 25 }, { 4, 4 } },
    { { 191, 25 }, { 4, 4 } },
    { { 197, 25 }, { 4, 4 } },
    { { 203, 25 }, { 4, 4 } },
    { { 209, 25 }, { 4, 4 } },
    { { 215, 25 }, { 4, 4 } },
    { { 21, 27 }, { 4, 4 } },
    { { 27, 27 }, { 4, 4 } },
    { { 33, 27 }, { 4, 4 } },
    { { 39, 27 }, { 4, 4 } },
    { { 45, 27 }, { 4, 4 } },
    { { 67, 27 }, { 4, 4 } },
    // EXPLOSION
    { { 189, 78 }, { 12, 12 } },
    { { 203, 78 }, { 12, 12 } },
    { { 217, 78 }, { 12, 12 } },
    { { 89, 79 }, { 12, 12 } },
    { { 1, 81 }, { 12, 12 } },
    { { 31, 85 }, { 12, 12 } },
    // FIELD
    { { 1, 29 }, { 10, 10 } },
    { { 51, 31 }, { 12, 12 } },
    { { 13, 33 }, { 14, 14 } },
    { { 29, 33 }, { 12, 12 } },
    // FONT
    { { 51, 11 }, { 8, 8 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 61, 11 }, { 6, 2 } },
    { { 69, 11 }, { 2, 6 } },
    { { 0, 0 }, { 0, 0 } },
    { { 73, 11 }, { 6, 6 } },
    { { 81, 11 }, { 6, 6 } },
    { { 89, 11 }, { 6, 6 } },
    { { 97, 11 }, { 6, 6 } },
    { { 105, 11 }, { 6, 6 } },
    { { 113, 11 }, { 6, 6 } },
    { { 121, 11 }, { 6, 6 } },
    { { 223, 11 }, { 6, 6 } },
    { { 243, 11 }, { 6, 6 } },
    { { 61, 15 }, { 6, 6 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 129, 11 }, { 4, 6 } },
    { { 0, 0 }, { 0, 0 } },
    { { 135, 17 }, { 4, 6 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 141, 17 }, { 6, 6 } },
    { { 149, 17 }, { 6, 6 } },
    { { 157, 17 }, { 6, 6 } },
    { { 165, 17 }, { 6, 6 } },
    { { 173, 17 }, { 6, 6 } },
    { { 181, 17 }, { 6, 6 } },
    { { 189, 17 }, { 6, 6 } },
    { { 197, 17 }, { 6, 6 } },
    { { 205, 17 }, { 6, 6 } },
    { { 213, 17 }, { 6, 6 } },
    { { 21, 19 }, { 6, 6 } },
    { { 29, 19 }, { 6, 6 } },
    { { 37, 19 }, { 6, 6 } },
    { { 69, 19 }, { 6, 6 } },
    { { 77, 19 }, { 6, 6 } },
    { { 85, 19 }, { 6, 6 } },
    { { 93, 19 }, { 6, 6 } },
    { { 101, 19 }, { 6, 6 } },
    { { 109, 19 }, { 6, 6 } },
    { { 117, 19 }, { 6, 6 } },
    { { 125, 19 }, { 6, 6 } },
    { { 221, 19 }, { 6, 6 } },
    { { 229, 19 }, { 6, 6 } },
    { { 237, 19 }, { 6, 6 } },
    { { 245, 19 }, { 6, 6 } },
    { { 1, 21 }, { 6, 6 } },
    { { 45, 19 }, { 4, 6 } },
    { { 0, 0 }, { 0, 0 } },
    { { 9, 21 }, { 4, 6 } },
    { { 0, 0 }, { 0, 0 } },
    { { 51, 21 }, { 6, 8 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 59, 23 }, { 6, 6 } },
    // HEALTH
    { { 21, 11 }, { 8, 6 } },
    { { 31, 11 }, { 8, 6 } },
    // HEALTH_ITEM
    { { 79, 89 }, { 7, 7 } },
    { { 185, 92 }, { 7, 7 } },
    { { 194, 92 }, { 7, 7 } },
    { { 203, 92 }, { 5, 5 } },
    // LASER
    { { 253, 67 }, { 1, 4 } },
    // MONEY_ITEM
    { { 108, 96 }, { 5, 7 } },
    { { 115, 96 }, { 5, 7 } },
    { { 133, 96 }, { 5, 7 } },
    { { 140, 96 }, { 5, 7 } },
    // PLASMA_BULLET
    { { 251, 27 }, { 3, 3 } },
    { { 251, 32 }, { 3, 3 } },
    { { 43, 33 }, { 3, 3 } },
    { { 65, 33 }, { 3, 3 } },
    // PLAYER
    { { 73, 27 }, { 8, 8 } },
    { { 83, 27 }, { 8, 10 } },
    // RAPID_BULLET
    { { 253, 1 }, { 1, 3 } },
    // RING
    { { 210, 92 }, { 8, 8 } },
    { { 220, 92 }, { 6, 8 } },
    { { 25, 91 }, { 4, 8 } },
    { { 251, 87 }, { 2, 8 } },
    { { 61, 93 }, { 4, 8 } },
    { { 88, 93 }, { 6, 8 } },
    // ROCKET
    { { 141, 25 }, { 8, 10 } },
    { { 151, 25 }, { 8, 10 } },
    // SAUCER
    { { 135, 1 }, { 20, 14 } },
    { { 157, 1 }, { 20, 14 } },
    { { 179, 1 }, { 20, 14 } },
    { { 201, 1 }, { 20, 14 } },
    // SAUCER_BULLET
    { { 253, 9 }, { 1, 5 } },
    // SMALL_LASER
    { { 253, 73 }, { 1, 2 } },
    // SMOKE
    { { 69, 87 }, { 8, 8 } },
    { { 53, 87 }, { 6, 6 } },
    { { 103, 79 }, { 4, 4 } },
    { { 103, 85 }, { 2, 2 } },
    // SPARK
    { { 253, 6 }, { 1, 1 } },
    // SPARKLE
    { { 15, 79 }, { 2, 2 } },
    { { 61, 79 }, { 4, 4 } },
    { { 61, 85 }, { 6, 6 } },
    { { 15, 87 }, { 8, 8 } },
    { { 45, 87 }, { 6, 6 } },
    // SPEED_ITEM
    { { 1, 1 }, { 4, 6 } },
    { { 7, 1 }, { 4, 6 } },
    { { 13, 1 }, { 6, 8 } },
    { { 21, 1 }, { 6, 8 } },
    { { 29, 1 }, { 4, 6 } },
    // SPIDER
    { { 35, 1 }, { 10, 8 } },
    { { 47, 1 }, { 12, 8 } },
    { { 61, 1 }, { 10, 8 } },
    { { 73, 1 }, { 12, 8 } },
    { { 87, 1 }, { 10, 8 } },
    { { 99, 1 }, { 10, 8 } },
    { { 111, 1 }, { 10, 8 } },
    { { 123, 1 }, { 10, 8 } },
    // SPIDER_BULLET
    { { 25, 87 }, { 2, 2 } },
    { { 167, 88 }, { 4, 4 } },
    { { 173, 88 }, { 4, 4 } },
    { { 179, 88 }, { 4, 4 } },
    // SQUARE
    { { 223, 1 }, { 8, 8 } },
    { { 233, 1 }, { 8, 6 } },
    { { 243, 1 }, { 8, 8 } },
    { { 1, 9 }, { 6, 10 } },
    { { 233, 9 }, { 8, 8 } },
    { { 9, 11 }, { 10, 8 } },
    // STARS
    { { 15, 21 }, { 2, 2 } },
    { { 15, 25 }, { 4, 4 } },
    { { 133, 25 }, { 2, 2 } },
    { { 137, 25 }, { 2, 2 } },
    // TITLE
    { { 123, 37 }, { 119, 19 } },
    // TWISTER
    { { 231, 87 }, { 8, 10 } },
    { { 241, 87 }, { 8, 8 } },
    { { 107, 88 }, { 8, 6 } },
    { { 117, 88 }, { 8, 6 } },
    { { 127, 88 }, { 8, 4 } },
    { { 137, 88 }, { 8, 6 } },
    { { 147, 88 }, { 8, 6 } },
    { { 157, 88 }, { 8, 8 } },
    // WALLS
    { { 70, 37 }, { 8, 8 } },
    { { 93, 37 }, { 8, 8 } },
    { { 244, 37 }, { 8, 8 } },
    { { 80, 39 }, { 8, 8 } },
    { { 103, 39 }, { 8, 8 } },
    { { 113, 39 }, { 8, 8 } },
    { { 1, 41 }, { 8, 8 } },
    { { 43, 45 }, { 8, 8 } },
    { { 53, 45 }, { 8, 8 } },
    { { 29, 47 }, { 8, 8 } },
    { { 63, 47 }, { 8, 8 } },
    { { 90, 47 }, { 8, 8 } },
    { { 244, 47 }, { 8, 8 } },
    { { 11, 49 }, { 8, 8 } },
    { { 73, 49 }, { 8, 8 } },
    { { 100, 49 }, { 8, 8 } },
    { { 110, 49 }, { 8, 8 } },
    { { 1, 51 }, { 8, 8 } },
    { { 39, 55 }, { 8, 8 } },
    { { 49, 55 }, { 8, 8 } },
    { { 21, 57 }, { 8, 8 } },
    { { 59, 57 }, { 8, 8 } },
    { { 83, 57 }, { 8, 8 } },
    { { 244, 57 }, { 8, 8 } },
    { { 0, 0 }, { 0, 0 } },
    { { 120, 58 }, { 8, 8 } },
    { { 130, 58 }, { 8, 8 } },
    { { 140, 58 }, { 8, 8 } },
    { { 150, 58 }, { 8, 8 } },
    { { 160, 58 }, { 8, 8 } },
    { { 170, 58 }, { 8, 8 } },
    { { 180, 58 }, { 8, 8 } },
    { { 190, 58 }, { 8, 8 } },
    { { 200, 58 }, { 8, 8 } },
    { { 210, 58 }, { 8, 8 } },
    { { 220, 58 }, { 8, 8 } },
    { { 230, 58 }, { 8, 8 } },
    { { 11, 59 }, { 8, 8 } },
    { { 69, 59 }, { 8, 8 } },
    { { 93, 59 }, { 8, 8 } },
    { { 0, 0 }, { 0, 0 } },
    { { 103, 59 }, { 8, 8 } },
    { { 1, 61 }, { 8, 8 } },
    { { 31, 65 }, { 8, 8 } },
    { { 41, 65 }, { 8, 8 } },
    { { 21, 67 }, { 8, 8 } },
    { { 51, 67 }, { 8, 8 } },
    { { 79, 67 }, { 8, 8 } },
    { { 240, 67 }, { 8, 8 } },
    { { 113, 68 }, { 8, 8 } },
    { { 123, 68 }, { 8, 8 } },
    { { 133, 68 }, { 8, 8 } },
    { { 143, 68 }, { 8, 8 } },
    { { 153, 68 }, { 8, 8 } },
    { { 163, 68 }, { 8, 8 } },
    { { 173, 68 }, { 8, 8 } },
    { { 183, 68 }, { 8, 8 } },
    { { 193, 68 }, { 8, 8 } },
    { { 203, 68 }, { 8, 8 } },
    { { 213, 68 }, { 8, 8 } },
    { { 223, 68 }, { 8, 8 } },
    { { 11, 69 }, { 8, 8 } },
    { { 61, 69 }, { 8, 8 } },
    { { 89, 69 }, { 8, 8 } },
    { { 99, 69 }, { 8, 8 } },
    { { 1, 71 }, { 8, 8 } },
    { { 31, 75 }, { 8, 8 } },
    { { 41, 75 }, { 8, 8 } },
    { { 21, 77 }, { 8, 8 } },
    { { 51, 77 }, { 8, 8 } },
    { { 71, 77 }, { 8, 8 } },
    { { 233, 77 }, { 8, 8 } },
    { { 243, 77 }, { 8, 8 } },
    { { 109, 78 }, { 8, 8 } },
    { { 119, 78 }, { 8, 8 } },
    { { 129, 78 }, { 8, 8 } },
    { { 139, 78 }, { 8, 8 } },
    { { 149, 78 }, { 8, 8 } },
    { { 159, 78 }, { 8, 8 } },
    { { 169, 78 }, { 8, 8 } },
};

int offsets[] = {
    0,
    10,
    14,
    18,
    19,
    21,
    37,
    43,
    47,
    175,
    177,
    181,
    182,
    186,
    190,
    192,
    193,
    199,
    201,
    205,
    206,
    207,
    211,
    212,
    217,
    222,
    230,
    234,
    240,
    244,
    245,
    253,
    333
};

}

SpriteRenderer::Rect const& frame(Sprite s, int frame) {
    return sprites[offsets[static_cast<int>(s)] + frame];
}

int frame_count(Sprite s) {
    return offsets[static_cast<int>(s) + 1] - offsets[static_cast<int>(s)];
}

