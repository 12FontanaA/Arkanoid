#ifndef DEFS_H
#define DEFS_H

#include "raylib.h"

#define SCREEN_W   800
#define SCREEN_H   600
#define PADDLE_W   100
#define PADDLE_H    20
#define BALL_R      8
#define ROWS         5
#define COLS        10
#define BRICK_WIDTH    70
#define BRICK_HEIGHT    20
#define BRICK_SP    4   // espaçamento

// Game config
typedef enum GameScreen {
    Menu, 
    gameOver,
    score
} GameScreen;

// Ball
typedef struct {
    Vector2 pos, vel;
    float radius;
} Ball;

// Brick
typedef struct {
    Rectangle rect;
    bool alive;
    Color color;
} Brick;

#endif // DEFS_H 