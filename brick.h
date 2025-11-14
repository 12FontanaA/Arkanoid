#ifndef BRICK_H
#define BRICK_H

#include "defs.h"

void InitBricks(Brick bricks[ROWS][COLS]);

void CreateBricks(Brick bricks[ROWS][COLS], Ball *ball, int *score);

void DrawBricks(Brick bricks[ROWS][COLS]);

#endif // BRICK_H