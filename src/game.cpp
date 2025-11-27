#include "game.h"
#include "brick.h"
#include "raylib.h"
#include "items.h"
#include "effects.h"
#include "ranking.h"
#include "levels.h"
#include <cmath>
#include <algorithm>

extern LevelLayout GetLevelLayout(int level);
extern void SetupLevelLayout(Brick bricks[ROWS][COLS], int level, LevelLayout layout);
extern void SaveScore(const std::string& playerName, int score, float gameTime, float difficulty);

void InitGame(GameState& state) {
    state.score = 0;
    state.lives = 3;
    state.currentLevel = 1;
    state.gameTime = 0.0f;
    state.gameOver = false;
    state.levelCompleted = false;
    state.specialItemsActive = 0;
    state.scoreMultiplier = 1;
    state.paddleWidthModifier = 1.0f;
    state.scoreInverted = false;
    state.inversionTimer = 0.0f;
}

void NextLevel(GameState& state, Brick bricks[ROWS][COLS], Ball& ball, Rectangle& paddle) {
    state.currentLevel++;
    state.levelCompleted = false;
    
    ball.pos = { (float)SCREEN_W / 2.0f, (float)SCREEN_H / 2.0f };
    paddle.x = ((float)SCREEN_W - paddle.width) / 2.0f;
    
    InitBricks(bricks, state.currentLevel);
    LevelLayout layout = GetLevelLayout(state.currentLevel);
    SetupLevelLayout(bricks, state.currentLevel, layout);
    
    float speedIncrease = 1.0f + (state.currentLevel * 0.15f);
    ball.vel.x *= speedIncrease;
    ball.vel.y *= speedIncrease;
    
    int levelBonus = 500 * state.currentLevel;
    state.score += levelBonus;
    
    // CORREÇÃO: Resetar inversor ao mudar de nível
    state.scoreInverted = false;
    state.inversionTimer = 0.0f;
}

void ResetPowerUps(GameState& state, Rectangle& paddle) {
    state.paddleWidthModifier = 1.0f;
    paddle.width = PADDLE_W;
    paddle.x = ((float)SCREEN_W - paddle.width) / 2.0f;
    state.scoreMultiplier = 1;
    state.specialItemsActive = 0;
    state.scoreInverted = false;
    state.inversionTimer = 0.0f;
}

bool CheckLevelCompletion(Brick bricks[ROWS][COLS]) {
    return CheckAllBricksDestroyed(bricks);
}

void UpdateGameTime(GameState& state, float deltaTime) {
    state.gameTime += deltaTime;
    
    // CORREÇÃO: Sistema de inversor melhorado
    if (state.scoreInverted) {
        state.inversionTimer -= deltaTime;
        if (state.inversionTimer <= 0.0f) {
            state.scoreInverted = false;
            // Garantir que o score não fique negativo
            if (state.score < 0) {
                state.score = 0;
            }
        }
    }
    
    static float timeBonusAccumulator = 0.0f;
    timeBonusAccumulator += deltaTime;
    
    if (timeBonusAccumulator >= 30.0f) {
        state.score += 100 * state.scoreMultiplier;
        timeBonusAccumulator = 0.0f;
    }
}

void ApplyDifficultySettings(GameState& state, float speedMultiplier) {
    if (speedMultiplier >= 1.4f) {
        state.scoreMultiplier = 2;
    } else if (speedMultiplier >= 1.8f) {
        state.scoreMultiplier = 3;
    } else {
        state.scoreMultiplier = 1;
    }
}

void HandleGameOver(GameState& state, const std::string& playerName, float gameTime, float difficulty) {
    if (state.gameOver) {
        // CORREÇÃO: Garantir score não negativo no final
        if (state.score < 0) {
            state.score = 0;
        }
        
        int finalScore = state.score;
        
        if (state.lives > 0) {
            finalScore += state.lives * 500;
        }
        
        if (gameTime < 180.0f) {
            finalScore += (int)((180.0f - gameTime) * 10);
        }
        
        state.score = finalScore;
        
        SaveScore(playerName, finalScore, gameTime, difficulty);
    }
}

void DebugGameState(const GameState& state) {
    TraceLog(LOG_INFO, TextFormat(
        "Game State - Level: %d, Score: %d, Lives: %d, Time: %.1f, Multiplier: %dx, Inverted: %s",
        state.currentLevel, state.score, state.lives, state.gameTime, state.scoreMultiplier,
        state.scoreInverted ? "YES" : "NO"
    ));
}