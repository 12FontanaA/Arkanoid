#include "menu.h"
#include "raylib.h"
#include "ranking.h"
#include <cmath>

static void WaitRelease(int key) {
    while (IsKeyDown(key) && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
        WaitTime(0.02);
    }
}

static int MenuNavigate(int max, int current) {
    if (IsKeyPressed(KEY_UP)) {
        current--;
        if (current < 0) current = max - 1;
    }
    if (IsKeyPressed(KEY_DOWN)) {
        current++;
        if (current >= max) current = 0;
    }
    return current;
}

void ShowRanking() {
    ShowRankingScreen();
}

MenuAction ShowMainMenu(float &speedMultiplier, bool &botMode) {
    const char* options[] = {
        "Iniciar Jogo (Humano)",
        "Iniciar Jogo (Bot)",
        "Dificuldade", 
        "Ranking",
        "Sair"
    };
    int selected = 0;
    const int count = 5;
    speedMultiplier = 1.0f;
    botMode = false;
    
    while (!WindowShouldClose()) {
        selected = MenuNavigate(count, selected);
        
        if (IsKeyPressed(KEY_ENTER)) {
            WaitRelease(KEY_ENTER);
            switch (selected) {
                case 0: 
                    botMode = false;
                    return MenuAction::Start;
                case 1:
                    botMode = true;
                    return MenuAction::StartBot;
                case 2: {
                    const char* diffs[] = {
                        "Fácil (1.0x)",
                        "Médio (1.4x)", 
                        "Difícil (1.8x)"
                    };
                    int diffSel = 0;
                    while (!WindowShouldClose()) {
                        diffSel = MenuNavigate(3, diffSel);
                        if (IsKeyPressed(KEY_ENTER)) {
                            if (diffSel == 0) speedMultiplier = 1.0f;
                            else if (diffSel == 1) speedMultiplier = 1.4f;
                            else if (diffSel == 2) speedMultiplier = 1.8f;
                            WaitRelease(KEY_ENTER);
                            break;
                        }
                        if (IsKeyPressed(KEY_ESCAPE)) break;
                        
                        BeginDrawing();
                        ClearBackground(BLACK);
                        DrawText("Selecione a dificuldade", 80, 40, 30, YELLOW);
                        for (int i = 0; i < 3; i++) {
                            Color col = (i == diffSel ? GREEN : RAYWHITE);
                            DrawText(diffs[i], 80, 140 + i * 40, 30, col);
                        }
                        DrawText("Pressione ESC para voltar", 80, 300, 20, GRAY);
                        EndDrawing();
                    }
                    break;
                }
                case 3:
                    ShowRanking();
                    break;
                case 4:
                    return MenuAction::Quit;
            }
        }
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        float time = (float)GetTime();
        Color titleColor = SKYBLUE;
        if (((int)(time * 2) % 2) == 0) {
            titleColor = BLUE;
        }
        
        DrawText("ARKANOID", 800/2 - MeasureText("ARKANOID", 60)/2, 40, 60, titleColor);
        
        for (int i = 0; i < count; i++) {
            Color col = (i == selected ? GREEN : RAYWHITE);
            if (i == selected) {
                DrawText(">", 50, 150 + i * 40, 35, GREEN);
            }
            DrawText(options[i], 80, 150 + i * 40, 35, col);
        }
        
        DrawText(TextFormat("Dificuldade: %.1fx", speedMultiplier), 80, 350, 20, YELLOW);
        DrawText("Use ↑ ↓ e ENTER", 80, 420, 18, GRAY);
        
        EndDrawing();
    }
    
    return MenuAction::Quit;
}

GameOverAction ShowGameOverMenu() {
    const char* options[] = {
        "Voltar ao Menu",
        "Reiniciar",
        "Sair"
    };
    int selected = 0;
    
    while (!WindowShouldClose()) {
        selected = MenuNavigate(3, selected);
        
        if (IsKeyPressed(KEY_ENTER)) {
            WaitRelease(KEY_ENTER);
            if (selected == 0) return GameOverAction::ReturnToMenu;
            if (selected == 1) return GameOverAction::Restart;
            if (selected == 2) return GameOverAction::Quit;
        }
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        DrawText("GAME OVER", 800/2 - MeasureText("GAME OVER", 50)/2, 80, 50, RED);
        
        for (int i = 0; i < 3; i++) {
            Color col = (i == selected ? YELLOW : RAYWHITE);
            DrawText(options[i], 800/2 - MeasureText(options[i], 30)/2, 200 + i * 40, 30, col);
        }
        
        DrawText("Pressione ENTER para selecionar", 800/2 - MeasureText("Pressione ENTER para selecionar", 20)/2, 350, 20, GRAY);
        
        EndDrawing();
    }
    
    return GameOverAction::Quit;
}