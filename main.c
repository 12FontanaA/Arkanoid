/********************************************************************
 * Arkanoid (Breakout) — versão com Q-Learning Bot
 * Compila em C99 com raylib
 ********************************************************************/

#include "raylib.h"
#include "defs.h"
#include "brick.h"
#include "sound.h"
#include "bot.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <time.h>

// Parâmetros do Q-Learning
#define ALPHA 0.1f      // Taxa de aprendizado
#define GAMMA 0.95f     // Fator de desconto
#define EPSILON 0.1f    // Taxa de exploração inicial
#define EPSILON_DECAY 0.995f  // Decaimento do epsilon
#define MIN_EPSILON 0.01f     // Epsilon mínimo

// Modos de jogo
typedef enum {
    MODE_HUMAN,     // Jogador humano
    MODE_TRAINING,  // Bot em treinamento
    MODE_AI_PLAY    // Bot jogando (sem exploração)
} GameMode;

static void CreatePaddle(Rectangle* paddle) {
    paddle->x = (SCREEN_W - PADDLE_W) / 2.0f;
    paddle->y = SCREEN_H - 40;
    paddle->width  = PADDLE_W;
    paddle->height = PADDLE_H;
}

static void CreateBall(Ball* ball) {
    ball->pos = (Vector2) { SCREEN_W / 2.0f, SCREEN_H / 2.0f };
    ball->vel = (Vector2) { GetRandomValue(-240, 240), -240 };   // px/s
    ball->radius = BALL_R;
}

static int ClampInt(int value, int min, int max)  {
    if (value < min ) return min;
    if (value > max ) return max;
    return value;
}

static void Reinit(Ball* ball, Rectangle* paddle, Brick bricks[ROWS][COLS], int *score, bool *gameOver) {
    PlaySound(restartSound);
    CreateBall(ball);
    CreatePaddle(paddle);
    InitBricks(bricks);
    *score = 0;
    *gameOver = false;
}

static void DrawBorders(void) {
    DrawRectangle(0, 0, SCREEN_W, 4, WHITE); // Topo
    DrawRectangle(0, SCREEN_H - 4, SCREEN_W, 4, WHITE); // Base
    DrawRectangle(0, 0, 4, SCREEN_H, WHITE); // Esquerda
    DrawRectangle(SCREEN_W - 4, 0, 4, SCREEN_H, WHITE); // Direita
}

// Calcula a recompensa baseada no estado atual do jogo
float CalculateReward(Ball ball, Rectangle paddle, int score, int lastScore, bool gameOver, bool hitBrick) {
    float reward = 0.0f;
    
    if (gameOver) {
        reward = -100.0f;  // Penalidade por perder
    } else if (hitBrick) {
        reward = 50.0f;    // Recompensa por quebrar tijolo
    } else if (score > lastScore) {
        reward = 10.0f;    // Recompensa por aumentar score
    } else {
        // Recompensa baseada na proximidade da bola com o paddle
        float distance = fabsf(ball.pos.x - (paddle.x + PADDLE_W/2));
        float normalized_distance = distance / (SCREEN_W/2);
        reward = 1.0f - normalized_distance;  // Quanto mais próximo, maior a recompensa
    }
    
    return reward;
}

// Executa a ação escolhida pelo bot
void ExecuteAction(Rectangle* paddle, int action, float dt) {
    float speed = 450.0f;
    
    switch(action) {
        case 0: // Mover para esquerda
            paddle->x -= speed * dt;
            break;
        case 1: // Ficar parado
            // Não faz nada
            break;
        case 2: // Mover para direita
            paddle->x += speed * dt;
            break;
    }
    
    // Manter paddle dentro da tela
    paddle->x = ClampInt(paddle->x, 0, SCREEN_W - PADDLE_W);
}

int main(void) {
    srand(time(NULL));
    
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Arkanoid — Q-Learning Bot");
    
    // Inicializar áudio
    InitAudioDevice();
    LoadSounds();

    // Inicializar Q-Learning
    float **Q = init_q_table();
    float epsilon = EPSILON;
    int episode = 0;
    int totalScore = 0;
    
    // Estados para Q-Learning
    int currentState, nextState;
    int currentAction;
    bool firstStep = true;
    
    // Game objects
    Rectangle paddle;
    CreatePaddle(&paddle);
    
    Ball ball;
    CreateBall(&ball);
    
    Brick bricks[ROWS][COLS];
    InitBricks(bricks);

    // States
    int score = 0, lastScore = 0;
    bool gameOver = false;
    bool hitBrick = false;
    GameMode mode = MODE_TRAINING;  // Começar em modo de treinamento
    
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        /* ---------- Controles de Modo ---------- */
        if (IsKeyPressed(KEY_ONE)) mode = MODE_HUMAN;
        if (IsKeyPressed(KEY_TWO)) mode = MODE_TRAINING;
        if (IsKeyPressed(KEY_THREE)) mode = MODE_AI_PLAY;
        
        /* ---------- Lógica ---------- */
        // Reiniciar
        if (gameOver) {
            if (mode == MODE_HUMAN && IsKeyPressed(KEY_SPACE)) {
                Reinit(&ball, &paddle, bricks, &score, &gameOver);
                firstStep = true;
            } else if (mode != MODE_HUMAN) {
                // Auto-reiniciar para treinamento/AI
                episode++;
                totalScore += score;
                
                if (episode % 100 == 0) {
                    printf("Episódio %d - Score médio: %.2f - Epsilon: %.3f\n", episode, (float)totalScore/100, epsilon);
                    totalScore = 0;
                }
                
                Reinit(&ball, &paddle, bricks, &score, &gameOver);
                firstStep = true;
                
                // Decaimento do epsilon durante treinamento
                if (mode == MODE_TRAINING && epsilon > MIN_EPSILON) {
                    epsilon *= EPSILON_DECAY;
                }
            }
        }

        if (!gameOver) {
            // Movimento do paddle baseado no modo
            if (mode == MODE_HUMAN) {
                // Controle humano
                if (IsKeyDown(KEY_LEFT))  paddle.x -= 450 * dt;
                if (IsKeyDown(KEY_RIGHT)) paddle.x += 450 * dt;
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                    paddle.x = GetMouseX() - PADDLE_W / 2.0f;
                paddle.x = ClampInt(paddle.x, 0, SCREEN_W - PADDLE_W);
            } else {
                // Controle do bot (Q-Learning)
                nextState = encode_state(paddle, ball);
                
                if (!firstStep) {
                    // Calcular recompensa
                    float reward = CalculateReward(ball, paddle, score, lastScore, gameOver, hitBrick);
                    
                    // Atualizar Q-table
                    q_learning_update(Q, currentState, currentAction, reward, 
                                    nextState, ALPHA, GAMMA, N_ACTIONS);
                }
                
                // Escolher próxima ação
                float currentEpsilon = (mode == MODE_TRAINING) ? epsilon : 0.0f; // Sem exploração no modo AI_PLAY
                currentAction = choose_action(Q, nextState, currentEpsilon);
                
                // Executar ação
                ExecuteAction(&paddle, currentAction, dt);
                
                currentState = nextState;
                lastScore = score;
                firstStep = false;
            }

            // Movimento da bola
            ball.pos.x += ball.vel.x * dt;
            ball.pos.y += ball.vel.y * dt;

            // Colisões com bordas
            if (ball.pos.x <= ball.radius || ball.pos.x >= SCREEN_W - ball.radius)
                ball.vel.x *= -1.0f;
            if (ball.pos.y <= ball.radius)
                ball.vel.y *= -1.0f;
            if (ball.pos.y >= SCREEN_H + ball.radius) {
                if (!gameOver) {
                    // PlaySound(gameOverSound);
                }
                gameOver = true;
            }

            // Colisão com paddle
            if (CheckCollisionCircleRec(ball.pos, ball.radius, paddle)) {
                PlaySound(paddleHitSound);
                ball.vel.y = -fabsf(ball.vel.y);
                float hit = (ball.pos.x - (paddle.x + PADDLE_W / 2.0f)) / (PADDLE_W / 2.0f);
                ball.vel.x = 300 * hit;
            }
            
            // Colisões com tijolos
            hitBrick = false;
            CreateBricks(bricks, &ball, &score);
            if (score > lastScore) hitBrick = true;
        }

        /* ---------- Render ---------- */
        BeginDrawing();
            ClearBackground(BLACK);

            DrawBorders();
            DrawBricks(bricks);
            
            // Paddle colorido baseado no modo
            Color paddleColor = WHITE;
            if (mode == MODE_TRAINING) paddleColor = BLUE;
            else if (mode == MODE_AI_PLAY) paddleColor = GREEN;
            
            DrawRectangleRounded(paddle, 0.6f, 10, paddleColor);
            DrawCircleV(ball.pos, ball.radius, YELLOW);

            // UI
            DrawText(TextFormat("SCORE: %05i", score), 10, 10, 20, RAYWHITE);
            
            // Informações do modo
            const char* modeText = "";
            if (mode == MODE_HUMAN) modeText = "HUMANO [1]";
            else if (mode == MODE_TRAINING) modeText = TextFormat("TREINANDO [2] - Ep:%d E:%.3f", episode, epsilon);
            else if (mode == MODE_AI_PLAY) modeText = "IA JOGANDO [3]";
            
            DrawText(modeText, 10, 35, 16, LIME);
            DrawText("1-Humano 2-Treinar 3-IA", 10, SCREEN_H - 25, 14, GRAY);
            
            if (gameOver) {
                if (mode == MODE_HUMAN) {
                    DrawText("GAME OVER — SPACE para reiniciar",
                        SCREEN_W / 2 - MeasureText("GAME OVER — SPACE para reiniciar", 20) / 2,
                        SCREEN_H / 2 - 10, 20, RED);
                } else {
                    DrawText("GAME OVER — Reiniciando...",
                        SCREEN_W / 2 - MeasureText("GAME OVER — Reiniciando...", 20) / 2,
                        SCREEN_H / 2 - 10, 20, RED);
                }
            }

            DrawFPS(SCREEN_W - 90, 10);
        EndDrawing();
    }

    // Limpeza
    for (int i = 0; i < N_STATES; i++) {
        free(Q[i]);
    }
    free(Q);
    
    UnloadSounds();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}