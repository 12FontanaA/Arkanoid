#ifndef RANKING_H
#define RANKING_H

#include <string>
#include <vector>

struct ScoreEntry {
    std::string name;
    std::string date;
    int score;
    float time;
    float difficulty;
};

void SaveScore(const std::string& name, int score, float time, float difficulty);
std::vector<ScoreEntry> LoadRanking();
void ShowRankingScreen();

#endif