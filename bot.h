#ifndef BOT_H
#define BOT_H

#include "raylib.h"
#include "defs.h"

// Quantidade de segmentos para a posição do paddle no eixo X
#define N_PADDLE_X   12   

// Quantidade de segmentos para a posição da bola no eixo X
#define N_BALL_X     12   

// Quantidade de segmentos para a posição da bola no eixo Y
#define N_BALL_Y     16   

// Quantidade de bins para a velocidade da bola no eixo X (-1, 0, +1)
#define N_BALL_VX     3   

// Quantidade de bins para a velocidade da bola no eixo Y (-1, 0, +1)
#define N_BALL_VY     3   

// Quantidade de ações possíveis: Esquerda, Parado, Direita
#define N_ACTIONS     3   

// Número total de estados possíveis (produto cartesiano dos bins)
#define N_STATES (N_PADDLE_X * N_BALL_X * N_BALL_Y * N_BALL_VX * N_BALL_VY)

/**
 * Discretiza um valor contínuo em um índice de bin.
 * @param value Valor a ser discretizado.
 * @param min Valor mínimo do intervalo.
 * @param max Valor máximo do intervalo.
 * @param bins Número de bins.
 * @return Índice do bin correspondente.
 */
int discretize(float value, float min, float max, int bins);

/**
 * Classifica um valor de velocidade em três categorias: negativo, zero, positivo.
 * @param v Valor da velocidade.
 * @return 0 se negativo, 1 se próximo de zero, 2 se positivo.
 */
int sign_bin(float v);

/**
 * Codifica o estado do ambiente (paddle e bola) em um único índice de estado discreto.
 * @param paddle Estrutura Rectangle representando o paddle.
 * @param ball Estrutura Ball representando a bola.
 * @return Índice do estado discreto.
 */
int encode_state(Rectangle paddle, Ball ball);

/**
 * Inicializa a tabela Q (Q-table) com zeros.
 * @return Ponteiro para a tabela Q alocada dinamicamente.
 */
float **init_q_table(void);

/**
 * Atualiza a tabela Q usando a regra do Q-Learning.
 * @param Q Tabela Q.
 * @param state Estado atual.
 * @param action Ação tomada.
 * @param reward Recompensa recebida.
 * @param next_state Próximo estado.
 * @param alpha Taxa de aprendizado.
 * @param gamma Fator de desconto.
 * @param num_actions Número de ações possíveis.
 */
void q_learning_update(float **Q, int state, int action, float reward, int next_state, float alpha, float gamma, int num_actions);

/**
 * @brief Encontra o índice do maior valor em um vetor de floats.
 *
 * Percorre o vetor @p arr de tamanho @p len e retorna o índice
 * do elemento cuja magnitude é máxima. Em caso de empate, retorna
 * o primeiro índice encontrado.
 *
 * @param arr  Ponteiro para o primeiro elemento do vetor de valores.
 * @param len  Número de elementos no vetor @p arr.
 * @return     Índice (0 ≤ índice < @p len) do elemento com maior valor.
 */
int argmax(const float *arr, int len);

/**
 * @brief Seleciona uma ação usando a política ε-greedy.
 *
 * Com probabilidade @p epsilon escolhe uma ação aleatória
 * (exploração). Caso contrário, escolhe a ação que maximiza
 * a estimativa de recompensa futura no estado atual (exploração greedy).
 *
 * @param Q        Tabela Q, matriz de dimensões [N_STATES][N_ACTIONS],
 *                 contendo os valores de Q(s,a) para cada par estado‑ação.
 * @param state    Índice do estado atual (0 ≤ state < N_STATES).
 * @param epsilon  Probabilidade de explorar (0.0 ≤ epsilon ≤ 1.0).
 * @return         Índice da ação selecionada (0 ≤ ação < N_ACTIONS).
 */
int choose_action(float **Q, int state, float epsilon);

#endif // BOT_H