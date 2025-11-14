#include "qtable_io.h"
#include "bot.h"
#include <stdio.h>
#include <stdbool.h>

bool save_qtable(float **Q, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) return false;
    
    // Escreve header com dimensões
    int states = N_STATES;
    int actions = N_ACTIONS;
    fwrite(&states, sizeof(int), 1, file);
    fwrite(&actions, sizeof(int), 1, file);
    
    // Escreve os dados da Q-table
    for (int i = 0; i < N_STATES; i++) {
        fwrite(Q[i], sizeof(float), N_ACTIONS, file);
    }
    
    fclose(file);
    return true;
}

bool load_qtable(float **Q, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return false;
    
    // Lê header e verifica dimensões
    int states, actions;
    if (fread(&states, sizeof(int), 1, file) != 1) {
        fclose(file);
        return false;
    }
    if (fread(&actions, sizeof(int), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    if (states != N_STATES || actions != N_ACTIONS) {
        fclose(file);
        return false; // Dimensões incompatíveis
    }
    
    // Lê os dados da Q-table
    for (int i = 0; i < N_STATES; i++) {
        if (fread(Q[i], sizeof(float), N_ACTIONS, file) != N_ACTIONS) {
            fclose(file);
            return false;
        }
    }
    
    fclose(file);
    return true;
}

bool save_qtable_text(float **Q, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) return false;
    
    fprintf(file, "Q-Table: %d states x %d actions\n", N_STATES, N_ACTIONS);
    fprintf(file, "State,Left,Stay,Right\n");
    
    for (int i = 0; i < N_STATES; i++) {
        fprintf(file, "%d", i);
        for (int a = 0; a < N_ACTIONS; a++) {
            fprintf(file, ",%.6f", Q[i][a]);
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    return true;
}