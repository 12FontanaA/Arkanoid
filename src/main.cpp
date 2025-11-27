#include "raylib.h"
#include "defs.h"
#include "brick.h"
#include "menu.h"
#include "ranking.h"
#include "items.h"
#include "game.h"
#include "utils.h"
#include "levels.h"
#include "bot.h"
#include "effects.h"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>
#include <algorithm>

static void CreatePaddle(Rectangle& paddle) {
    paddle.x = (SCREEN_W - PADDLE_W) / 2.0f;
    paddle.y = SCREEN_H - 40;
    paddle.width = PADDLE_W;
    paddle.height = PADDLE_H;
}

static void CreateBall(Ball& ball, float speedMultiplier) {
    ball.pos = { SCREEN_W / 2.0f, SCREEN_H / 2.0f };
    float baseSpeed = 240.0f * speedMultiplier;
    ball.vel = {
        static_cast<float>(GetRandomValue(-240, 240)) * speedMultiplier,
        -baseSpeed
    };
    ball.radius = BALL_R;
}

static void Reinit(
    Ball& ball,
    Rectangle& paddle,
    Brick bricks[ROWS][COLS],
    GameState& gameState,
    std::vector<Item>& activeItems,
    float speedMultiplier
) {
    CreateBall(ball, speedMultiplier);
    CreatePaddle(paddle);
    InitBricks(bricks, gameState.currentLevel);
    SetupLevelLayout(bricks, gameState.currentLevel, GetLevelLayout(gameState.currentLevel));
    gameState.levelCompleted = false;
    activeItems.clear();
    gameState.scoreMultiplier = 1;
    gameState.paddleWidthModifier = 1.0f;
    gameState.scoreInverted = false;
    gameState.inversionTimer = 0.0f;
    paddle.width = PADDLE_W;
}

static void DrawBorders() {
    DrawRectangle(0, 0, SCREEN_W, 4, WHITE);
    DrawRectangle(0, SCREEN_H - 4, SCREEN_W, 4, WHITE);
    DrawRectangle(0, 0, 4, SCREEN_H, WHITE);
    DrawRectangle(SCREEN_W - 4, 0, 4, SCREEN_H, WHITE);
}

static void MaybeSpawnItem(Vector2 position, std::vector<Item>& activeItems) {
    if (GetRandomValue(0, 100) < 25) {
        SpawnItem(position, activeItems);
    }
}

static void ProcessBrickCollisions(Ball& ball, Brick bricks[ROWS][COLS], GameState& gameState, 
                                  std::vector<Item>& activeItems, ParticleSystem* particleSystem) {
    bool brickHit = false;
    for (int r = 0; r < ROWS && !brickHit; ++r) {
        for (int c = 0; c < COLS && !brickHit; ++c) {
            Brick& b = bricks[r][c];
            if (!b.alive) continue;

            if (CheckCollisionCircleRec(ball.pos, ball.radius, b.rect)) {
                b.durability--;
                
                CreateExplosionEffect(particleSystem, 
                    {b.rect.x + b.rect.width/2, b.rect.y + b.rect.height/2},
                    b.color, 2);

                if (b.durability <= 0) {
                    b.alive = false;
                    
                    int points = 10 * b.maxDurability * gameState.scoreMultiplier;
                    if (gameState.scoreInverted) {
                        points = -points;
                    }
                    gameState.score += points;
                    
                    MaybeSpawnItem({b.rect.x + b.rect.width/2, b.rect.y + b.rect.height/2}, activeItems);
                } else {
                    float alpha = 0.3f + 0.7f * (b.durability / (float)b.maxDurability);
                    b.color = ColorAlpha(b.color, alpha);
                }

                brickHit = true;

                float ballLeft = ball.pos.x - ball.radius;
                float ballRight = ball.pos.x + ball.radius;
                float ballTop = ball.pos.y - ball.radius;
                float ballBottom = ball.pos.y + ball.radius;
                
                float brickLeft = b.rect.x;
                float brickRight = b.rect.x + b.rect.width;
                float brickTop = b.rect.y;
                float brickBottom = b.rect.y + b.rect.height;
                
                float overlapLeft = ballRight - brickLeft;
                float overlapRight = brickRight - ballLeft;
                float overlapTop = ballBottom - brickTop;
                float overlapBottom = brickBottom - ballTop;
                
                float minOverlap = overlapLeft;
                if (overlapRight < minOverlap) minOverlap = overlapRight;
                if (overlapTop < minOverlap) minOverlap = overlapTop;
                if (overlapBottom < minOverlap) minOverlap = overlapBottom;
                
                if (minOverlap == overlapLeft || minOverlap == overlapRight) {
                    ball.vel.x = -ball.vel.x;
                    
                    if (minOverlap == overlapLeft) {
                        ball.pos.x = brickLeft - ball.radius - 1.0f;
                    } else {
                        ball.pos.x = brickRight + ball.radius + 1.0f;
                    }
                } else {
                    ball.vel.y = -ball.vel.y;
                    
                    if (minOverlap == overlapTop) {
                        ball.pos.y = brickTop - ball.radius - 1.0f;
                    } else {
                        ball.pos.y = brickBottom + ball.radius + 1.0f;
                    }
                }
                
                ball.vel.x *= 1.02f;
                ball.vel.y *= 1.02f;
                
                float speed = sqrtf(ball.vel.x * ball.vel.x + ball.vel.y * ball.vel.y);
                float maxSpeed = 500.0f;
                if (speed > maxSpeed) {
                    ball.vel.x = (ball.vel.x / speed) * maxSpeed;
                    ball.vel.y = (ball.vel.y / speed) * maxSpeed;
                }

                if (CheckAllBricksDestroyed(bricks)) {
                    gameState.levelCompleted = true;
                    int levelBonus = 500 * gameState.scoreMultiplier;
                    if (gameState.scoreInverted) {
                        levelBonus = -levelBonus;
                    }
                    gameState.score += levelBonus;
                }
            }
        }
    }
}

static void DrawHUD(const GameState& gameState, float gameTimer, bool botMode) {
    DrawRectangle(0, 0, SCREEN_W, 100, ColorAlpha(BLACK, 0.3f));
    
    Color scoreColor = gameState.scoreInverted ? ORANGE : RAYWHITE;
    DrawText(TextFormat("SCORE: %05i", gameState.score), 10, 10, 20, scoreColor);
    
    DrawText(TextFormat("VIDAS: %d", gameState.lives), SCREEN_W - 120, 10, 20, RED);
    DrawText(TextFormat("NIVEL: %d/3", gameState.currentLevel), SCREEN_W - 120, 35, 20, BLUE);
    DrawText(TextFormat("TEMPO: %.1fs", gameTimer), SCREEN_W - 150, 60, 20, GREEN);
    
    if (gameState.scoreMultiplier > 1) {
        DrawText(TextFormat("MULTIPLICADOR: %dx", gameState.scoreMultiplier), SCREEN_W - 200, 85, 20, PURPLE);
    }
    
    if (botMode) {
        DrawText("MODO: BOT", 10, 35, 20, ORANGE);
    } else {
        DrawText("MODO: HUMANO", 10, 35, 20, GREEN);
    }
    
    if (gameState.scoreInverted) {
        DrawText(TextFormat("INVERSOR: %.1fs", gameState.inversionTimer), 10, 60, 20, ORANGE);
    }
}

static void DrawGameStateScreens(const GameState& gameState) {
    if (gameState.gameOver) {
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.7f));
        DrawText("GAME OVER", SCREEN_W / 2 - MeasureText("GAME OVER", 40) / 2, SCREEN_H / 2 - 50, 40, RED);
        DrawText("Pressione qualquer tecla para continuar...", 
                SCREEN_W / 2 - MeasureText("Pressione qualquer tecla para continuar...", 20) / 2, 
                SCREEN_H / 2 + 10, 20, WHITE);
    }
}

static void UpdateBotControl(std::vector<std::vector<float>>& Q, Ball& ball, Rectangle& paddle, 
                           float epsilon, bool botMode, int& currentState, int& lastState, int& lastAction,
                           float alpha, float gamma, GameState& gameState) {
    if (!botMode) return;
    
    Vector2 predictedPosition = ball.pos;
    Vector2 predictedVelocity = ball.vel;
    
    for (int i = 0; i < 60; i++) {
        predictedPosition.x += predictedVelocity.x * GetFrameTime();
        predictedPosition.y += predictedVelocity.y * GetFrameTime();
        
        if (predictedPosition.x <= ball.radius || predictedPosition.x >= SCREEN_W - ball.radius) {
            predictedVelocity.x *= -1;
        }
        
        if (predictedPosition.y <= ball.radius) {
            predictedVelocity.y *= -1;
        }
        
        if (predictedVelocity.y > 0 && predictedPosition.y > SCREEN_H - 150) {
            break;
        }
        
        if (predictedPosition.y > SCREEN_H) {
            break;
        }
    }
    
    float paddleCenter = paddle.x + paddle.width / 2;
    float targetX = predictedPosition.x - paddle.width / 2;
    targetX = Clamp(targetX, 0.0f, SCREEN_W - paddle.width);
    
    float direction = 0;
    if (paddleCenter < predictedPosition.x - 10) {
        direction = 1;
    } else if (paddleCenter > predictedPosition.x + 10) {
        direction = -1;
    }
    
    float paddleSpeed = 400.0f * GetFrameTime();
    paddle.x += direction * paddleSpeed;
    
    int action = 1;
    if (direction > 0) action = 2;
    if (direction < 0) action = 0;
    
    currentState = bot::encode_state(paddle, ball);
    
    if (lastState != -1 && lastAction != -1) {
        float reward = 0.0f;
        
        float distanceToBall = fabs(paddleCenter - predictedPosition.x);
        reward += (1.0f - (distanceToBall / SCREEN_W)) * 0.3f;
        
        if (CheckCollisionCircleRec(ball.pos, ball.radius, paddle)) {
            reward += 3.0f;
        }
        
        if (ball.pos.y > SCREEN_H) {
            reward -= 10.0f;
        }
        
        bot::q_learning_update(Q, lastState, lastAction, reward, currentState, alpha, gamma);
    }
    
    lastState = currentState;
    lastAction = action;
}

static void SaveQTable(const std::vector<std::vector<float>>& Q) {
    FILE* file = fopen("qtable.txt", "w");
    if (file) {
        for (int i = 0; i < bot::N_STATES; ++i) {
            for (int j = 0; j < bot::N_ACTIONS; ++j) {
                fprintf(file, "%.6f ", Q[i][j]);
            }
            fprintf(file, "\n");
        }
        fclose(file);
        TraceLog(LOG_INFO, "Q-table salva com sucesso!");
    }
}

static void LoadQTable(std::vector<std::vector<float>>& Q) {
    FILE* file = fopen("qtable.txt", "r");
    if (file) {
        for (int i = 0; i < bot::N_STATES; ++i) {
            for (int j = 0; j < bot::N_ACTIONS; ++j) {
                if (fscanf(file, "%f", &Q[i][j]) != 1) {
                    fclose(file);
                    return;
                }
            }
        }
        fclose(file);
        TraceLog(LOG_INFO, "Q-table carregada com sucesso!");
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    InitWindow(SCREEN_W, SCREEN_H, "Arkanoid — Modo Completo");
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetTargetFPS(60);

    std::vector<std::vector<float>> Q = bot::init_q_table();
    LoadQTable(Q);
    bool botMode = false;
    float epsilon = 0.1f;
    float alpha = 0.1f;
    float gamma = 0.9f;
    
    int currentState = -1;
    int lastState = -1;
    int lastAction = -1;

    ParticleSystem particleSystem;
    InitParticleSystem(&particleSystem, 1000);

    float speedMultiplier = 1.0f;
    MenuAction action = ShowMainMenu(speedMultiplier, botMode);
    if (action == MenuAction::Quit) {
        FreeParticleSystem(&particleSystem);
        CloseWindow();
        return 0;
    }

    if (action == MenuAction::Start) {
        botMode = false;
    } else if (action == MenuAction::StartBot) {
        botMode = true;
    }

    Rectangle paddle;
    CreatePaddle(paddle);

    Ball ball;
    CreateBall(ball, speedMultiplier);

    Brick bricks[ROWS][COLS];
    InitBricks(bricks, 1);
    SetupLevelLayout(bricks, 1, GetLevelLayout(1));

    GameState gameState;
    InitGame(gameState);
    ApplyDifficultySettings(gameState, speedMultiplier);
    
    std::vector<Item> activeItems;
    float gameTimer = 0.0f;
    bool gameRunning = true;

    while (!WindowShouldClose() && gameRunning) {
        float dt = GetFrameTime();
        gameTimer += dt;

        UpdateParticles(&particleSystem, dt);
        UpdateGameTime(gameState, dt);

        if (gameState.gameOver) {
            SaveScore("PLAYER", gameState.score, gameTimer, speedMultiplier);
            GameOverAction go = ShowGameOverMenu();

            if (go == GameOverAction::ReturnToMenu) {
                MenuAction newAction = ShowMainMenu(speedMultiplier, botMode);
                if (newAction == MenuAction::Quit)
                    break;

                if (newAction == MenuAction::Start) {
                    botMode = false;
                } else if (newAction == MenuAction::StartBot) {
                    botMode = true;
                }

                Reinit(ball, paddle, bricks, gameState, activeItems, speedMultiplier);
                InitGame(gameState);
                ApplyDifficultySettings(gameState, speedMultiplier);
                gameTimer = 0.0f;
                
                currentState = -1;
                lastState = -1;
                lastAction = -1;
                particleSystem.count = 0;
            }
            else if (go == GameOverAction::Restart) {
                Reinit(ball, paddle, bricks, gameState, activeItems, speedMultiplier);
                InitGame(gameState);
                ApplyDifficultySettings(gameState, speedMultiplier);
                gameTimer = 0.0f;
                
                currentState = -1;
                lastState = -1;
                lastAction = -1;
                particleSystem.count = 0;
            }
            else if (go == GameOverAction::Quit) {
                break;
            }
        }
        else if (gameState.levelCompleted) {
            if (gameState.currentLevel >= 3) {
                int victoryBonus = 1000 * gameState.scoreMultiplier;
                if (gameState.scoreInverted) {
                    victoryBonus = -victoryBonus;
                }
                gameState.score += victoryBonus;
                
                SaveScore("VENCEDOR", gameState.score, gameTimer, speedMultiplier);
                
                bool victoryScreen = true;
                while (victoryScreen && !WindowShouldClose()) {
                    BeginDrawing();
                    ClearBackground(BLACK);
                    
                    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(GREEN, 0.7f));
                    DrawText("PARABÉNS!", SCREEN_W / 2 - MeasureText("PARABÉNS!", 60) / 2, SCREEN_H / 2 - 100, 60, GOLD);
                    DrawText("VOCÊ COMPLETOU TODOS OS NÍVEIS!", SCREEN_W / 2 - MeasureText("VOCÊ COMPLETOU TODOS OS NÍVEIS!", 30) / 2, SCREEN_H / 2 - 20, 30, WHITE);
                    DrawText(TextFormat("Score Final: %d", gameState.score), SCREEN_W / 2 - MeasureText("Score Final: 00000", 25) / 2, SCREEN_H / 2 + 30, 25, YELLOW);
                    DrawText("Pressione ENTER para voltar ao menu", SCREEN_W / 2 - MeasureText("Pressione ENTER para voltar ao menu", 20) / 2, SCREEN_H / 2 + 80, 20, LIGHTGRAY);
                    
                    EndDrawing();

                    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        victoryScreen = false;
                    }
                }
                
                break;
            } else {
                NextLevel(gameState, bricks, ball, paddle);
                ApplyDifficultySettings(gameState, speedMultiplier);
                activeItems.clear();
                particleSystem.count = 0;
                currentState = -1;
                lastState = -1;
                lastAction = -1;
            }
        }
        else {
            if (botMode) {
                UpdateBotControl(Q, ball, paddle, epsilon, botMode, currentState, lastState, lastAction, alpha, gamma, gameState);
            } else {
                if (IsKeyDown(KEY_LEFT)) paddle.x -= 450 * dt * speedMultiplier;
                if (IsKeyDown(KEY_RIGHT)) paddle.x += 450 * dt * speedMultiplier;
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    paddle.x = GetMouseX() - paddle.width / 2.0f;
                }
                paddle.x = Clamp(paddle.x, 0.0f, SCREEN_W - paddle.width);
            }

            ball.pos.x += ball.vel.x * dt;
            ball.pos.y += ball.vel.y * dt;

            if (ball.pos.x <= ball.radius) {
                ball.pos.x = ball.radius;
                ball.vel.x = fabs(ball.vel.x);
            }
            if (ball.pos.x >= SCREEN_W - ball.radius) {
                ball.pos.x = SCREEN_W - ball.radius;
                ball.vel.x = -fabs(ball.vel.x);
            }
            if (ball.pos.y <= ball.radius) {
                ball.pos.y = ball.radius;
                ball.vel.y = fabs(ball.vel.y);
            }

            if (ball.pos.y >= SCREEN_H + ball.radius) {
                gameState.lives--;
                if (gameState.lives <= 0) {
                    gameState.gameOver = true;
                } else {
                    CreateBall(ball, speedMultiplier);
                    CreatePaddle(paddle);
                    int penalty = 100;
                    if (gameState.scoreInverted) {
                        penalty = -penalty;
                    }
                    gameState.score = std::max(0, gameState.score - penalty);
                    
                    currentState = -1;
                    lastState = -1;
                    lastAction = -1;
                    particleSystem.count = 0;
                }
            }

            if (CheckCollisionCircleRec(ball.pos, ball.radius, paddle)) {
                ball.pos.y = paddle.y - ball.radius - 1.0f;
                
                float hit = (ball.pos.x - (paddle.x + paddle.width / 2.0f)) / (paddle.width / 2.0f);
                float currentSpeed = sqrtf(ball.vel.x * ball.vel.x + ball.vel.y * ball.vel.y);
                ball.vel.x = hit * currentSpeed * 0.8f;
                ball.vel.y = -fabs(ball.vel.y) * 1.1f;
            }

            ProcessBrickCollisions(ball, bricks, gameState, activeItems, &particleSystem);

            UpdateItems(activeItems, paddle, gameState);

            static float scoreTimer = 0.0f;
            scoreTimer += dt;
            if (scoreTimer >= 1.0f) {
                int timePoints = 1 * gameState.scoreMultiplier;
                if (gameState.scoreInverted) {
                    timePoints = -timePoints;
                }
                gameState.score += timePoints;
                scoreTimer = 0.0f;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawBorders();
        DrawBricks(bricks);
        DrawItems(activeItems);
        DrawParticles(&particleSystem);

        DrawRectangleRounded(paddle, 0.6f, 10, WHITE);
        DrawRectangleLinesEx(paddle, 2.0f, DARKGRAY);
        DrawCircleV(ball.pos, ball.radius, YELLOW);

        DrawHUD(gameState, gameTimer, botMode);
        DrawGameStateScreens(gameState);

        if (!gameState.gameOver && !gameState.levelCompleted) {
            if (botMode) {
                DrawText("MODO BOT ATIVO - Observando o aprendizado", 10, SCREEN_H - 50, 14, ORANGE);
            } else {
                DrawText("Teclas: SETAS/MOUSE para mover", 10, SCREEN_H - 50, 14, LIGHTGRAY);
            }
        }

        DrawFPS(SCREEN_W - 90, SCREEN_H - 30);
        EndDrawing();
    }

    if (botMode) {
        SaveQTable(Q);
    }

    FreeParticleSystem(&particleSystem);
    CloseWindow();
    return 0;
}