#include "bot.h"
#include "defs.h"
#include <stdlib.h>
#include <math.h>

/**
 * Discretiza um valor contínuo em um índice de bin.
 * 
 * @param value Valor a ser discretizado.
 * @param min Valor mínimo do intervalo.
 * @param max Valor máximo do intervalo.
 * @param bins Número de bins (segmentos).
 * @return Índice do bin correspondente ao valor.
 */
int discretize(float value, float min, float max, int bins) {
    int idx = (int)(((value - min) / (max - min)) * bins);
    if (idx < 0) idx = 0;
    if (idx >= bins) idx = bins - 1;
    return idx;
}

/**
 * Classifica um valor de velocidade em três categorias: negativo, zero, positivo.
 * 
 * @param v Valor da velocidade.
 * @return 0 se negativo, 1 se próximo de zero, 2 se positivo.
 */
int sign_bin(float v) {
    if (v < -10.0f) return 0;
    if (v > 10.0f) return 2;
    return 1;
}

/**
 * Codifica o estado do ambiente (paddle e bola) em um único índice de estado discreto.
 * 
 * @param paddle Estrutura Rectangle representando o paddle.
 * @param ball Estrutura Ball representando a bola.
 * @return Índice do estado discreto.
 */
int encode_state(Rectangle paddle, Ball ball) {
    int px = discretize(paddle.x, 0, SCREEN_W - PADDLE_W, N_PADDLE_X);
    int bx = discretize(ball.pos.x, 0, SCREEN_W, N_BALL_X);
    int by = discretize(ball.pos.y, 0, SCREEN_H, N_BALL_Y);
    int bvx = sign_bin(ball.vel.x);
    int bvy = sign_bin(ball.vel.y);
    // Combina todos os índices discretizados em um único índice de estado
    return (((((px * N_BALL_X + bx) * N_BALL_Y + by) * N_BALL_VX + bvx) * N_BALL_VY) + bvy);
}

/**
 * Inicializa a tabela Q (Q-table) com zeros.
 * 
 * @return Ponteiro para a tabela Q alocada dinamicamente.
 */
float **init_q_table(void) {
    float **Q = (float**)malloc(N_STATES * sizeof(float*));
    for (int i = 0; i < N_STATES; i++) {
        Q[i] = (float*)calloc(N_ACTIONS, sizeof(float));
    }
    return Q;
}

/**
 * Atualiza a tabela Q usando a regra do Q-Learning.
 * 
 * @param Q Tabela Q.
 * @param state Estado atual.
 * @param action Ação tomada.
 * @param reward Recompensa recebida.
 * @param next_state Próximo estado.
 * @param alpha Taxa de aprendizado.
 * @param gamma Fator de desconto.
 * @param num_actions Número de ações possíveis.
 */
void q_learning_update(float **Q, int state, int action, float reward, int next_state, float alpha, float gamma, int num_actions) {
    // Encontra o maior valor Q para o próximo estado (política gulosa)
    float max_q_next = Q[next_state][0];
    for (int a = 1; a < num_actions; a++) {
        if (Q[next_state][a] > max_q_next) {
            max_q_next = Q[next_state][a];
        }
    }
    // Atualiza o valor Q para o par (estado, ação) atual
    Q[state][action] = Q[state][action] + alpha * (reward + gamma * max_q_next - Q[state][action]);
}

#include "bot.h"

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
int argmax(const float *arr, int len) {
    int best = 0;
    for (int i = 1; i < len; i++) {
        if (arr[i] > arr[best]) {
            best = i;
        }
    }
    return best;
}

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
int choose_action(float **Q, int state, float epsilon) {
    float r = (float)rand() / (float)RAND_MAX;
    if (r < epsilon) {
        /* Exploração: escolhe ação aleatória */
        return rand() % N_ACTIONS;
    } else {
        /* Exploração greedy: escolhe ação de maior valor Q */
        return argmax(Q[state], N_ACTIONS);
    }
}