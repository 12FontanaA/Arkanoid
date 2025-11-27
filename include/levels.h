#ifndef LEVELS_H
#define LEVELS_H

#include "defs.h"
#include "brick.h"

enum class LevelLayout {
    STANDARD,
    PYRAMID,
    DIAMOND,
    WALL,
    CHECKERBOARD,
    RECURSIVE,
    FORTIFIED  // NOVO LAYOUT ADICIONADO
};

LevelLayout GetLevelLayout(int level);
void SetupLevelLayout(Brick bricks[ROWS][COLS], int level, LevelLayout layout);
void CreatePattern(Brick bricks[ROWS][COLS], int startRow, int startCol, int width, int height, int patternType, int depth);

// FUNÇÃO RECURSIVA
void CreateRecursivePattern(Brick bricks[ROWS][COLS], int startRow, int startCol, int size, int depth);

#endif