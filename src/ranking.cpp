#include "ranking.h"
#include "raylib.h"
#include <fstream>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>

void SaveScore(const std::string& name, int score, float time, float difficulty) {
    std::ofstream file("ranking.txt", std::ios::app);
    if (!file.is_open()) return;
    
    std::time_t now = std::time(nullptr);
    char dateStr[20];
    std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", std::localtime(&now));
    
    file << name << "," << dateStr << "," << score << "," << time << "," << difficulty << std::endl;
    file.close();
}

std::vector<ScoreEntry> LoadRanking() {
    std::vector<ScoreEntry> ranking;
    std::ifstream file("ranking.txt");
    if (!file.is_open()) return ranking;
    
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        ScoreEntry entry;
        std::string temp;
        
        if (std::getline(ss, entry.name, ',') &&
            std::getline(ss, entry.date, ',') &&
            std::getline(ss, temp, ',')) {
            try {
                entry.score = std::stoi(temp);
                if (std::getline(ss, temp, ',')) {
                    entry.time = std::stof(temp);
                }
                if (std::getline(ss, temp)) {
                    entry.difficulty = std::stof(temp);
                }
                ranking.push_back(entry);
            } catch (...) {
                continue;
            }
        }
    }
    
    std::sort(ranking.begin(), ranking.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
    });
    
    file.close();
    return ranking;
}

void ShowRankingScreen() {
    auto ranking = LoadRanking();
    
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) return;
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        DrawText("RANKING", 250, 30, 40, YELLOW);
        
        DrawText("POS", 50, 100, 20, WHITE);
        DrawText("NOME", 120, 100, 20, WHITE);
        DrawText("SCORE", 350, 100, 20, WHITE);
        DrawText("DATA", 450, 100, 20, WHITE);
        DrawText("DIF", 550, 100, 20, WHITE);
        
        for (int i = 0; i < 10 && i < ranking.size(); i++) {
            const auto& entry = ranking[i];
            int y = 130 + i * 30;
            
            DrawText(TextFormat("%d", i + 1), 50, y, 20, WHITE);
            DrawText(entry.name.c_str(), 120, y, 20, WHITE);
            DrawText(TextFormat("%05d", entry.score), 350, y, 20, WHITE);
            DrawText(entry.date.c_str(), 450, y, 20, WHITE);
            DrawText(TextFormat("%.1fx", entry.difficulty), 550, y, 20, WHITE);
        }
        
        if (ranking.empty()) {
            DrawText("Nenhum registro encontrado", 250, 200, 20, GRAY);
        }
        
        DrawText("Pressione ESC para voltar", 250, 500, 20, GRAY);
        
        EndDrawing();
    }
}