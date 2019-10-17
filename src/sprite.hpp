#pragma once
#include "sprite_renderer.hpp"

enum class Sprite {
    BALL,
    BALL_FIELD,
    BALL_ITEM,
    BLOCKADE,
    CANNON,
    ENERGY_ITEM,
    EXPLOSION,
    FIELD,
    FLAME_ITEM,
    FONT,
    HEALTH,
    HEART_ITEM,
    LASER,
    MONEY_ITEM,
    PLASMA_BULLET,
    PLAYER,
    RAPID_BULLET,
    RING,
    ROCKET,
    SAUCER,
    SAUCER_BULLET,
    SMALL_LASER,
    SMOKE,
    SPARK,
    SPARKLE,
    SPIDER,
    SPIDER_BULLET,
    SQUARE,
    STARS,
    TITLE,
    TOUCH,
    TWISTER,
    WALLS,
};

SpriteRenderer::Rect const& frame(Sprite s, int frame = 0);
int frame_count(Sprite s);
