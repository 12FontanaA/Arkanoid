#ifndef QTABLE_IO_H
#define QTABLE_IO_H

#include "bot.h"
#include <stdio.h>
#include <stdbool.h>

/**
 * Salva a Q-table em um arquivo binário.
 * @param Q Ponteiro para a Q-table.
 * @param filename Nome do arquivo para salvar.
 * @return true se salvou com sucesso, false caso contrário.
 */
bool save_qtable(float **Q, const char *filename);

/**
 * Carrega a Q-table de um arquivo binário.
 * @param Q Ponteiro para a Q-table (deve estar já alocada).
 * @param filename Nome do arquivo para carregar.
 * @return true se carregou com sucesso, false caso contrário.
 */
bool load_qtable(float **Q, const char *filename);

/**
 * Salva a Q-table em formato texto (para debug/análise).
 * @param Q Ponteiro para a Q-table.
 * @param filename Nome do arquivo para salvar.
 * @return true se salvou com sucesso, false caso contrário.
 */
bool save_qtable_text(float **Q, const char *filename);

#endif // QTABLE_IO_H