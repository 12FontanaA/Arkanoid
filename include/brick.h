#ifndef BRICK_H
#define BRICK_H

#include "defs.h"
#include "raylib.h"
#include <vector>

// Forward declarations
struct Ball;
struct Item;
struct ParticleSystem;

struct Brick {
    Rectangle rect;
    bool alive;
    Color color;
    int durability;
    int maxDurability;
};

// SOBRECARGA DE FUNÇÕES - ADICIONADO
void CreateBrick(Brick& brick, Color color);  // Versão básica
void CreateBrick(Brick& brick, Color color, int durability);  // Versão com durabilidade
void CreateBrick(Brick& brick, Color color, int durability, bool special);  // Versão especial

void InitBricks(Brick bricks[ROWS][COLS], int level);
void CreateBricks(Brick bricks[ROWS][COLS], Ball* ball, int* score, std::vector<Item>& activeItems, ParticleSystem* particleSystem, int scoreMultiplier);
void DrawBricks(Brick bricks[ROWS][COLS]);
bool CheckAllBricksDestroyed(Brick bricks[ROWS][COLS]);
void ResetBricks(Brick bricks[ROWS][COLS], int level);
int GetRemainingBricks(Brick bricks[ROWS][COLS]);
void CreateSpecialBrickPattern(Brick bricks[ROWS][COLS], int patternType, int level);
void UpdateBrickAnimations(Brick bricks[ROWS][COLS], float deltaTime);
void HighlightVulnerableBricks(Brick bricks[ROWS][COLS]);
int CalculateBrickScore(const Brick& brick);
void DebugBrickInfo(Brick bricks[ROWS][COLS]);

#endif // BRICK_H