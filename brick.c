#include "defs.h"
#include "sound.h"

#include <math.h>

void InitBricks(Brick bricks[ROWS][COLS]) {
    const int offsetX = (SCREEN_W - (COLS * (BRICK_WIDTH + BRICK_SP) - BRICK_SP)) / 2;
    const int offsetY = 60;

    for (int r = 0; r < ROWS; ++r)
    for (int c = 0; c < COLS; ++c) {
        bricks[r][c].rect = (Rectangle){
            offsetX + c * (BRICK_WIDTH + BRICK_SP),
            offsetY + r * (BRICK_HEIGHT + BRICK_SP),
            BRICK_WIDTH, BRICK_HEIGHT };
        bricks[r][c].alive = true;
        bricks[r][c].color = ColorFromHSV(r * 36.0f, 0.7f, 0.9f);
    }
}

void CreateBricks(Brick bricks[ROWS][COLS], Ball *ball, int *score) {
    // tijolos
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c) {
            Brick *b = &bricks[r][c];
            if (!b->alive) continue;

            if (CheckCollisionCircleRec(ball->pos, ball->radius, b->rect)) {
                PlaySound(brickHitSound);
                b->alive = false;
                *score += 10;

                // Calcula as distâncias das bordas da bola em relação ao bloco
                float dist_top    = fabsf((ball->pos.y + ball->radius) - b->rect.y);
                float dist_bottom = fabsf((ball->pos.y - ball->radius) - (b->rect.y + b->rect.height));
                float dist_left   = fabsf((ball->pos.x + ball->radius) - b->rect.x);
                float dist_right  = fabsf((ball->pos.x - ball->radius) - (b->rect.x + b->rect.width));

                // Verifica se a colisão é mais próxima dos lados vertical ou horizontal
                if (fminf(dist_top, dist_bottom) < fminf(dist_left, dist_right)) {
                    // Colisão no topo ou na base do bloco, inverte o eixo Y
                    ball->vel.y *= -1.0f;
                } else {
                    // Colisão nos lados esquerdo ou direito do bloco, inverte o eixo X
                    ball->vel.x *= -1.0f;
                }

                goto skipRemaining; // evita multi-colisão no mesmo frame
            }
        }
    skipRemaining: ;
}

void DrawBricks(Brick bricks[ROWS][COLS]) {
    for (int r = 0; r < ROWS; ++r)
    for (int c = 0; c < COLS; ++c)
    if (bricks[r][c].alive)
        DrawRectangleRec(bricks[r][c].rect, bricks[r][c].color);
}