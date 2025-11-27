#include "qtable_io.h"
#include <fstream>
#include <cstdio>

bool save_qtable(const std::vector<std::vector<float>>& Q, const std::string& filename) {
    FILE* file = std::fopen(filename.c_str(), "wb");
    if (!file) return false;
    
    int states = bot::N_STATES;
    int actions = bot::N_ACTIONS;
    std::fwrite(&states, sizeof(int), 1, file);
    std::fwrite(&actions, sizeof(int), 1, file);
    
    for (int i = 0; i < bot::N_STATES; ++i) {
        std::fwrite(Q[i].data(), sizeof(float), bot::N_ACTIONS, file);
    }
    
    std::fclose(file);
    return true;
}

bool load_qtable(std::vector<std::vector<float>>& Q, const std::string& filename) {
    FILE* file = std::fopen(filename.c_str(), "rb");
    if (!file) return false;
    
    int states, actions;
    if (std::fread(&states, sizeof(int), 1, file) != 1) {
        std::fclose(file);
        return false;
    }
    if (std::fread(&actions, sizeof(int), 1, file) != 1) {
        std::fclose(file);
        return false;
    }
    
    if (states != bot::N_STATES || actions != bot::N_ACTIONS) {
        std::fclose(file);
        return false;
    }
    
    for (int i = 0; i < bot::N_STATES; ++i) {
        if (std::fread(Q[i].data(), sizeof(float), bot::N_ACTIONS, file) != bot::N_ACTIONS) {
            std::fclose(file);
            return false;
        }
    }
    
    std::fclose(file);
    return true;
}

bool save_qtable_text(const std::vector<std::vector<float>>& Q, const std::string& filename) {
    FILE* file = std::fopen(filename.c_str(), "w");
    if (!file) return false;
    
    std::fprintf(file, "Q-Table: %d states x %d actions\n", bot::N_STATES, bot::N_ACTIONS);
    std::fprintf(file, "State,Left,Stay,Right\n");
    
    for (int i = 0; i < bot::N_STATES; ++i) {
        std::fprintf(file, "%d", i);
        for (int a = 0; a < bot::N_ACTIONS; ++a) {
            std::fprintf(file, ",%.6f", Q[i][a]);
        }
        std::fprintf(file, "\n");
    }
    
    std::fclose(file);
    return true;
}