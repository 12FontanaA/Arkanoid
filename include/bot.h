#ifndef BOT_H
#define BOT_H

#include "raylib.h"
#include "defs.h"
#include <vector>

namespace bot {
    constexpr int N_PADDLE_X = 12;
    constexpr int N_BALL_X = 12;
    constexpr int N_BALL_Y = 16;
    constexpr int N_BALL_VX = 3;
    constexpr int N_BALL_VY = 3;
    constexpr int N_ACTIONS = 3;
    constexpr int N_STATES = N_PADDLE_X * N_BALL_X * N_BALL_Y * N_BALL_VX * N_BALL_VY;

    int discretize(float value, float min, float max, int bins);
    int sign_bin(float v);
    int encode_state(Rectangle paddle, Ball ball);
    std::vector<std::vector<float>> init_q_table();
    void q_learning_update(std::vector<std::vector<float>>& Q, int state, int action, float reward, int next_state, float alpha, float gamma);
    int argmax(const std::vector<float>& arr);
    int choose_action(const std::vector<std::vector<float>>& Q, int state, float epsilon);
} // namespace bot

#endif