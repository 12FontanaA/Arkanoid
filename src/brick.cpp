#include "brick.h"
#include "defs.h"
#include "raylib.h"
#include "effects.h"
#include <cmath>
#include <vector>

// Declarações das funções externas que usamos
void SpawnItem(Vector2 position, std::vector<Item>& activeItems);

// SOBRECARGA DE FUNÇÕES - IMPLEMENTAÇÃO

// Versão 1: Apenas cor (durabilidade padrão = 1)
void CreateBrick(Brick& brick, Color color) {
    brick.color = color;
    brick.durability = 1;
    brick.maxDurability = 1;
    brick.alive = true;
}

// Versão 2: Cor + durabilidade personalizada
void CreateBrick(Brick& brick, Color color, int durability) {
    brick.color = color;
    brick.durability = durability;
    brick.maxDurability = durability;
    brick.alive = true;
}

// Versão 3: Cor + durabilidade + efeito especial
void CreateBrick(Brick& brick, Color color, int durability, bool special) {
    brick.color = color;
    brick.durability = durability;
    brick.maxDurability = durability;
    brick.alive = true;
    
    if (special) {
        // Escurecer a cor para indicar bloco especial
        brick.color = ColorAlpha(brick.color, 0.8f);
    }
}

void InitBricks(Brick bricks[ROWS][COLS], int level) {
    const int offsetX = (SCREEN_W - (COLS * (BRICK_WIDTH + BRICK_SP) - BRICK_SP)) / 2;
    const int offsetY = 60;

    Color colors[] = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, PINK, MAROON, VIOLET, SKYBLUE};

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            bricks[r][c].rect = {
                static_cast<float>(offsetX + c * (BRICK_WIDTH + BRICK_SP)),
                static_cast<float>(offsetY + r * (BRICK_HEIGHT + BRICK_SP)),
                static_cast<float>(BRICK_WIDTH),
                static_cast<float>(BRICK_HEIGHT)
            };
            
            // USANDO SOBRECARGA - diferentes versões baseadas na posição e nível
            if (level == 1) {
                // Nível 1: versão básica
                CreateBrick(bricks[r][c], colors[(r + c) % 7]);
            } else if (level == 2) {
                // Nível 2: versão com durabilidade
                int durability = 1 + ((r + c + level) % 3);
                CreateBrick(bricks[r][c], colors[(r + c) % 7], durability);
            } else {
                // Nível 3: versão especial para alguns blocos
                int durability = 1 + ((r + c + level) % 3);
                bool special = (r % 2 == 0 && c % 2 == 0); // Blocos especiais em posições pares
                CreateBrick(bricks[r][c], colors[(r + c) % 7], durability, special);
            }
        }
    }
}

void CreateBricks(Brick bricks[ROWS][COLS], Ball* ball, int* score, std::vector<Item>& activeItems, ParticleSystem* particleSystem, int scoreMultiplier) {
    bool brickHit = false;
    
    for (int r = 0; r < ROWS && !brickHit; ++r) {
        for (int c = 0; c < COLS && !brickHit; ++c) {
            Brick& b = bricks[r][c];
            if (!b.alive) continue;

            if (CheckCollisionCircleRec(ball->pos, ball->radius, b.rect)) {
                brickHit = true;
                b.durability--;
                
                // Efeito visual de dano
                float damageRatio = (float)b.durability / b.maxDurability;
                b.color = ColorAlpha(b.color, 0.3f + 0.7f * damageRatio);
                
                if (b.durability <= 0) {
                    b.alive = false;
                    *score += 10 * b.maxDurability * scoreMultiplier;
                    
                    // Chance de spawnar item (25% de chance)
                    if (GetRandomValue(0, 100) < 25) {
                        Vector2 itemPos = {b.rect.x + b.rect.width/2, b.rect.y + b.rect.height/2};
                        SpawnItem(itemPos, activeItems);
                    }
                    
                    // Efeito de partículas na destruição completa
                    CreateExplosionEffect(particleSystem, 
                        {b.rect.x + b.rect.width/2, b.rect.y + b.rect.height/2},
                        b.color, 15);
                } else {
                    // Efeito de partículas menor para dano parcial
                    CreateExplosionEffect(particleSystem, 
                        {b.rect.x + b.rect.width/2, b.rect.y + b.rect.height/2},
                        b.color, 8);
                }

                // **FÍSICA SUPER SIMPLES E CONFIÁVEL**
                // Calcular de qual lado a bola atingiu o bloco
                float ballLeft = ball->pos.x - ball->radius;
                float ballRight = ball->pos.x + ball->radius;
                float ballTop = ball->pos.y - ball->radius;
                float ballBottom = ball->pos.y + ball->radius;
                
                float brickLeft = b.rect.x;
                float brickRight = b.rect.x + b.rect.width;
                float brickTop = b.rect.y;
                float brickBottom = b.rect.y + b.rect.height;
                
                // Calcular overlaps
                float overlapLeft = ballRight - brickLeft;
                float overlapRight = brickRight - ballLeft;
                float overlapTop = ballBottom - brickTop;
                float overlapBottom = brickBottom - ballTop;
                
                // Encontrar o menor overlap
                float minOverlap = overlapLeft;
                if (overlapRight < minOverlap) minOverlap = overlapRight;
                if (overlapTop < minOverlap) minOverlap = overlapTop;
                if (overlapBottom < minOverlap) minOverlap = overlapBottom;
                
                // Rebater baseado na direção da menor sobreposição
                if (minOverlap == overlapLeft || minOverlap == overlapRight) {
                    // Colisão horizontal - inverter X
                    ball->vel.x = -ball->vel.x;
                    
                    // Corrigir posição
                    if (minOverlap == overlapLeft) {
                        ball->pos.x = brickLeft - ball->radius - 1.0f;
                    } else {
                        ball->pos.x = brickRight + ball->radius + 1.0f;
                    }
                } else {
                    // Colisão vertical - inverter Y
                    ball->vel.y = -ball->vel.y;
                    
                    // Corrigir posição
                    if (minOverlap == overlapTop) {
                        ball->pos.y = brickTop - ball->radius - 1.0f;
                    } else {
                        ball->pos.y = brickBottom + ball->radius + 1.0f;
                    }
                }
                
                // Pequeno aumento de velocidade
                ball->vel.x *= 1.02f;
                ball->vel.y *= 1.02f;
                
                // Limitar velocidade máxima
                float speed = sqrtf(ball->vel.x * ball->vel.x + ball->vel.y * ball->vel.y);
                float maxSpeed = 500.0f;
                if (speed > maxSpeed) {
                    ball->vel.x = (ball->vel.x / speed) * maxSpeed;
                    ball->vel.y = (ball->vel.y / speed) * maxSpeed;
                }
                
                return; // Processar apenas uma colisão por frame
            }
        }
    }
}

void DrawBricks(Brick bricks[ROWS][COLS]) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (bricks[r][c].alive) {
                // Desenhar o bloco principal
                DrawRectangleRec(bricks[r][c].rect, bricks[r][c].color);
                
                // Desenhar borda para melhor visualização
                DrawRectangleLinesEx(bricks[r][c].rect, 1.5f, ColorAlpha(BLACK, 0.3f));
                
                // Desenhar número de durabilidade se maior que 1
                if (bricks[r][c].maxDurability > 1) {
                    const char* durabilityText = TextFormat("%d", bricks[r][c].durability);
                    int textWidth = MeasureText(durabilityText, 14);
                    
                    DrawText(durabilityText,
                            bricks[r][c].rect.x + bricks[r][c].rect.width/2 - textWidth/2,
                            bricks[r][c].rect.y + bricks[r][c].rect.height/2 - 7,
                            14, WHITE);
                }
                
                // Efeito visual adicional para blocos com alta durabilidade
                if (bricks[r][c].maxDurability >= 3) {
                    // Desenhar um contorno colorido baseado na durabilidade
                    Color outlineColor = GOLD;
                    if (bricks[r][c].maxDurability >= 4) {
                        outlineColor = PURPLE;
                    }
                    if (bricks[r][c].maxDurability >= 5) {
                        outlineColor = MAROON;
                    }
                    DrawRectangleLinesEx(bricks[r][c].rect, 3.0f, outlineColor);
                }
                
                // Efeito especial para blocos com durabilidade muito alta
                if (bricks[r][c].maxDurability >= 4) {
                    // Desenhar um padrão interno para blocos super resistentes
                    Rectangle innerRect = {
                        bricks[r][c].rect.x + 2,
                        bricks[r][c].rect.y + 2,
                        bricks[r][c].rect.width - 4,
                        bricks[r][c].rect.height - 4
                    };
                    DrawRectangleLinesEx(innerRect, 1.0f, ColorAlpha(WHITE, 0.5f));
                }
            }
        }
    }
}

bool CheckAllBricksDestroyed(Brick bricks[ROWS][COLS]) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (bricks[r][c].alive) {
                return false;
            }
        }
    }
    return true;
}

void ResetBricks(Brick bricks[ROWS][COLS], int level) {
    InitBricks(bricks, level);
}

int GetRemainingBricks(Brick bricks[ROWS][COLS]) {
    int count = 0;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (bricks[r][c].alive) {
                count++;
            }
        }
    }
    return count;
}

void CreateSpecialBrickPattern(Brick bricks[ROWS][COLS], int patternType, int level) {
    // Padrões especiais para fases avançadas
    switch (patternType) {
        case 0: // Padrão X
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (r == c || r + c == ROWS - 1) {
                        // USANDO SOBRECARGA - versão com durabilidade aumentada
                        CreateBrick(bricks[r][c], PURPLE, 2 + level);
                    } else {
                        bricks[r][c].alive = false;
                    }
                }
            }
            break;
            
        case 1: // Padrão bordas
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (r == 0 || r == ROWS - 1 || c == 0 || c == COLS - 1) {
                        // USANDO SOBRECARGA - versão com durabilidade
                        CreateBrick(bricks[r][c], ORANGE, 1 + level);
                    } else {
                        bricks[r][c].alive = false;
                    }
                }
            }
            break;
            
        case 2: // Padrão aleatório
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (GetRandomValue(0, 100) < 70) { // 70% de chance de ter bloco
                        // USANDO SOBRECARGA - versão com durabilidade aleatória
                        int durability = 1 + GetRandomValue(0, level);
                        CreateBrick(bricks[r][c], ColorFromHSV(GetRandomValue(0, 360), 0.8f, 0.9f), durability);
                    } else {
                        bricks[r][c].alive = false;
                    }
                }
            }
            break;
            
        case 3: // Padrão fortificado (novo)
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (r == 0 || r == ROWS - 1 || c == 0 || c == COLS - 1) {
                        CreateBrick(bricks[r][c], MAROON, 5); // Bordas muito resistentes
                    } else if (r == 1 || r == ROWS - 2 || c == 1 || c == COLS - 2) {
                        CreateBrick(bricks[r][c], ORANGE, 3); // Segunda camada
                    } else {
                        CreateBrick(bricks[r][c], ColorFromHSV((r * 30 + c * 15) % 360, 0.8f, 0.9f), 2);
                    }
                }
            }
            break;
            
        default: // Padrão padrão - todos os blocos
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    // USANDO SOBRECARGA - versão básica
                    CreateBrick(bricks[r][c], RED);
                }
            }
            break;
    }
}

void UpdateBrickAnimations(Brick bricks[ROWS][COLS], float deltaTime) {
    // Animação sutil de pulsação para blocos com durabilidade > 1
    static float pulseTime = 0.0f;
    pulseTime += deltaTime;
    
    float pulse = (sinf(pulseTime * 2.0f) + 1.0f) * 0.1f; // Pulsação entre 0.9 e 1.1
    
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (bricks[r][c].alive && bricks[r][c].maxDurability > 1) {
                // Aplicar efeito de pulsação leve apenas se não estiver danificado
                if (bricks[r][c].durability == bricks[r][c].maxDurability) {
                    Color original = bricks[r][c].color;
                    bricks[r][c].color = ColorAlpha(original, 0.8f + pulse * 0.2f);
                }
            }
        }
    }
}

void HighlightVulnerableBricks(Brick bricks[ROWS][COLS]) {
    // Destacar blocos com 1 de durabilidade restante
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (bricks[r][c].alive && bricks[r][c].durability == 1) {
                // Piscar em vermelho para indicar vulnerabilidade
                if (((int)(GetTime() * 4) % 2) == 0) {
                    DrawRectangleRec(bricks[r][c].rect, ColorAlpha(RED, 0.3f));
                }
            }
        }
    }
}

int CalculateBrickScore(const Brick& brick) {
    // Sistema de pontuação baseado na durabilidade do bloco
    switch (brick.maxDurability) {
        case 1: return 10;
        case 2: return 25;
        case 3: return 50;
        case 4: return 75;
        case 5: return 100;
        default: return 10 * brick.maxDurability;
    }
}

void DebugBrickInfo(Brick bricks[ROWS][COLS]) {
    int aliveCount = 0;
    int totalDurability = 0;
    int maxDurabilityBricks = 0;
    
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (bricks[r][c].alive) {
                aliveCount++;
                totalDurability += bricks[r][c].durability;
                if (bricks[r][c].maxDurability >= 3) {
                    maxDurabilityBricks++;
                }
            }
        }
    }
    
    TraceLog(LOG_DEBUG, TextFormat("Bricks: %d alive, Total durability: %d, Strong bricks: %d", 
                                   aliveCount, totalDurability, maxDurabilityBricks));
}