#include "bot.h"
#include <algorithm>
#include <random>

namespace bot {

int discretize(float value, float min, float max, int bins) {
    if (value <= min) return 0;
    if (value >= max) return bins - 1;
    float ratio = (value - min) / (max - min);
    return static_cast<int>(ratio * bins);
}

int sign_bin(float v) {
    if (v < 0) return 0;
    if (v > 0) return 2;
    return 1;
}

int encode_state(Rectangle paddle, Ball ball) {
    int px = bot::discretize(paddle.x, 0, SCREEN_W - PADDLE_W, bot::N_PADDLE_X);
    int bx = bot::discretize(ball.pos.x, 0, SCREEN_W, bot::N_BALL_X);
    int by = bot::discretize(ball.pos.y, 0, SCREEN_H, bot::N_BALL_Y);
    int bvx = bot::sign_bin(ball.vel.x);
    int bvy = bot::sign_bin(ball.vel.y);
    
    return (((((px * bot::N_BALL_X + bx) * bot::N_BALL_Y + by) * bot::N_BALL_VX + bvx) * bot::N_BALL_VY) + bvy);
}

std::vector<std::vector<float>> init_q_table() {
    return std::vector<std::vector<float>>(
        bot::N_STATES,
        std::vector<float>(bot::N_ACTIONS, 0.0f)
    );
}

void q_learning_update(std::vector<std::vector<float>>& Q, int state, int action, float reward, int next_state, float alpha, float gamma) {
    float max_next = *std::max_element(Q[next_state].begin(), Q[next_state].end());
    Q[state][action] = Q[state][action] + alpha * (reward + gamma * max_next - Q[state][action]);
}

int argmax(const std::vector<float>& arr) {
    return static_cast<int>(
        std::distance(arr.begin(), std::max_element(arr.begin(), arr.end()))
    );
}

int choose_action(const std::vector<std::vector<float>>& Q, int state, float epsilon) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    static std::uniform_int_distribution<int> action_dis(0, bot::N_ACTIONS - 1);
    
    if (dis(gen) < epsilon) {
        return action_dis(gen);
    }
    return bot::argmax(Q[state]);
}

} // namespace bot