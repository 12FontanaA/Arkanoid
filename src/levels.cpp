#include "levels.h"
#include "brick.h"
#include "raylib.h"

LevelLayout GetLevelLayout(int level) {
    switch(level) {
        case 1: return LevelLayout::STANDARD;
        case 2: return LevelLayout::PYRAMID;
        case 3: return LevelLayout::FORTIFIED;  // NOVO LAYOUT FORTIFICADO
        case 4: return LevelLayout::DIAMOND;
        case 5: return LevelLayout::WALL;
        default: return LevelLayout::STANDARD;
    }
}

void SetupLevelLayout(Brick bricks[ROWS][COLS], int level, LevelLayout layout) {
    // Reset todos os blocos
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            bricks[r][c].alive = false;
        }
    }
    
    Color colors[] = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, PINK, MAROON, VIOLET, SKYBLUE};
    
    switch(layout) {
        case LevelLayout::STANDARD:
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    CreateBrick(bricks[r][c], colors[r % 7]);
                }
            }
            break;
            
        case LevelLayout::PYRAMID:
            for (int r = 0; r < ROWS; r++) {
                int bricksInRow = r + 1;
                int start = (COLS - bricksInRow) / 2;
                for (int c = start; c < start + bricksInRow; c++) {
                    if (c >= 0 && c < COLS) {
                        CreateBrick(bricks[r][c], colors[(r + c) % 7], 2);
                    }
                }
            }
            break;
            
        case LevelLayout::DIAMOND:
            for (int r = 0; r < ROWS; r++) {
                int bricksInRow = (r < 3) ? (r * 2 + 1) : ((ROWS - r - 1) * 2 + 1);
                int start = (COLS - bricksInRow) / 2;
                for (int c = start; c < start + bricksInRow; c++) {
                    if (c >= 0 && c < COLS) {
                        CreateBrick(bricks[r][c], colors[(r + c) % 7], 2);
                    }
                }
            }
            break;

        case LevelLayout::FORTIFIED:  // NOVO LAYOUT FORTIFICADO PARA NÍVEL 3
            {
                // Camada externa - blocos mais resistentes
                for (int r = 0; r < ROWS; r++) {
                    for (int c = 0; c < COLS; c++) {
                        if (r == 0 || r == ROWS - 1 || c == 0 || c == COLS - 1) {
                            // Bordas - blocos muito resistentes (5 hits)
                            CreateBrick(bricks[r][c], MAROON, 5);
                        } else if (r == 1 || r == ROWS - 2 || c == 1 || c == COLS - 2) {
                            // Segunda camada - blocos resistentes (3 hits)
                            CreateBrick(bricks[r][c], ORANGE, 3);
                        } else {
                            // Centro - blocos normais (2 hits)
                            CreateBrick(bricks[r][c], colors[(r * c) % 7], 2);
                        }
                    }
                }
                
                // Adicionar blocos especiais super resistentes em posições estratégicas
                bricks[2][2].durability = 4;
                bricks[2][2].maxDurability = 4;
                bricks[2][2].color = PURPLE;
                
                bricks[2][COLS-3].durability = 4;
                bricks[2][COLS-3].maxDurability = 4;
                bricks[2][COLS-3].color = PURPLE;
                
                bricks[ROWS-3][2].durability = 4;
                bricks[ROWS-3][2].maxDurability = 4;
                bricks[ROWS-3][2].color = PURPLE;
                
                bricks[ROWS-3][COLS-3].durability = 4;
                bricks[ROWS-3][COLS-3].maxDurability = 4;
                bricks[ROWS-3][COLS-3].color = PURPLE;
            }
            break;

        case LevelLayout::WALL:
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    int durability = 1 + ((r + c) % 3);
                    CreateBrick(bricks[r][c], colors[(r * 2 + c) % 7], durability);
                }
            }
            break;

        case LevelLayout::CHECKERBOARD:
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if ((r + c) % 2 == 0) {
                        CreateBrick(bricks[r][c], colors[r % 7], 1 + (r % 2));
                    }
                }
            }
            break;

        case LevelLayout::RECURSIVE:
            CreateRecursivePattern(bricks, 0, 0, 8, 3);
            break;
    }
}

// FUNÇÃO RECURSIVA - ATUALIZADA PARA BLOCO MAIS RESISTENTES
void CreateRecursivePattern(Brick bricks[ROWS][COLS], int startRow, int startCol, int size, int depth) {
    if (depth <= 0 || size < 2) {
        return;
    }
    
    if (startRow >= ROWS || startCol >= COLS) {
        return;
    }
    
    Color depthColors[] = {ORANGE, YELLOW, GOLD, PINK, SKYBLUE, LIME, PURPLE, MAROON};
    Color currentColor = depthColors[depth % 8];
    
    // Aumentar durabilidade baseada na profundidade
    int durability = depth + 1; // 2, 3, ou 4 hits
    
    for (int r = startRow; r < startRow + size && r < ROWS; r++) {
        for (int c = startCol; c < startCol + size && c < COLS; c++) {
            CreateBrick(bricks[r][c], currentColor, durability, true);
        }
    }
    
    int newSize = size / 2;
    
    CreateRecursivePattern(bricks, startRow, startCol, newSize, depth - 1);
    CreateRecursivePattern(bricks, startRow, startCol + newSize, newSize, depth - 1);
    CreateRecursivePattern(bricks, startRow + newSize, startCol, newSize, depth - 1);
    CreateRecursivePattern(bricks, startRow + newSize, startCol + newSize, newSize, depth - 1);
}

void CreatePattern(Brick bricks[ROWS][COLS], int startRow, int startCol, int width, int height, int patternType, int depth) {
    if (depth > 2 || width < 2 || height < 2) return;
    
    for (int r = startRow; r < startRow + height; r++) {
        for (int c = startCol; c < startCol + width; c++) {
            if (r < ROWS && c < COLS) {
                if (r == startRow || r == startRow + height - 1 || c == startCol || c == startCol + width - 1) {
                    bricks[r][c].alive = true;
                    Color depthColors[] = {RED, ORANGE, YELLOW, GREEN};
                    bricks[r][c].color = depthColors[depth % 4];
                    bricks[r][c].durability = depth + 1;
                    bricks[r][c].maxDurability = depth + 1;
                }
            }
        }
    }
    
    int subWidth = width / 2;
    int subHeight = height / 2;
    CreatePattern(bricks, startRow + 1, startCol + 1, subWidth, subHeight, patternType, depth + 1);
    CreatePattern(bricks, startRow + 1, startCol + subWidth + 1, subWidth, subHeight, patternType, depth + 1);
}