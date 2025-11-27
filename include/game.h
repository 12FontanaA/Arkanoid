#ifndef GAME_H
#define GAME_H

#include "defs.h"
#include "brick.h"
#include <string>
#include <vector>

struct GameState {
    int score;
    int lives;
    int currentLevel;
    float gameTime;
    bool gameOver;
    bool levelCompleted;
    int specialItemsActive;
    int scoreMultiplier;
    float paddleWidthModifier;
    bool scoreInverted;  // NOVO: controle do efeito do inversor
    float inversionTimer;  // NOVO: tempo restante do efeito
};

void InitGame(GameState& state);
void NextLevel(GameState& state, Brick bricks[ROWS][COLS], Ball& ball, Rectangle& paddle);
void ResetPowerUps(GameState& state, Rectangle& paddle);
bool CheckLevelCompletion(Brick bricks[ROWS][COLS]);
void UpdateGameTime(GameState& state, float deltaTime);
void ApplyDifficultySettings(GameState& state, float speedMultiplier);
void HandleGameOver(GameState& state, const std::string& playerName, float gameTime, float difficulty);
void DebugGameState(const GameState& state);

#endif