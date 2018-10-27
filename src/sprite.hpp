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
    FONT,
    HEALTH,
    HEALTH_ITEM,
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
    SPEED_ITEM,
    SPIDER,
    SPIDER_BULLET,
    SQUARE,
    STARS,
    TITLE,
    TWISTER,
    WALLS,
};

SpriteRenderer::Rect const& frame(Sprite s, int frame = 0);
int frame_count(Sprite s);

