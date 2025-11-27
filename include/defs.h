#ifndef DEFS_H
#define DEFS_H

#include "raylib.h"

constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 600;
constexpr int PADDLE_W = 100;
constexpr int PADDLE_H = 20;
constexpr int BALL_R = 8;
constexpr int ROWS = 5;
constexpr int COLS = 10;
constexpr int BRICK_WIDTH = 70;
constexpr int BRICK_HEIGHT = 20;
constexpr int BRICK_SP = 4;

enum class GameScreen { Menu, GameOver, Score };

struct Ball {
    Vector2 pos;
    Vector2 vel;
    float radius;
};

#endif // DEFS_H