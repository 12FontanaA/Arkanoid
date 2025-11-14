#include <raylib.h>

// Sons do jogo
Sound paddleHitSound;
Sound brickHitSound;
Sound gameOverSound;
Sound restartSound;

// Função para carregar os sons
void LoadSounds(void) {
    paddleHitSound = LoadSound("assets/sounds/paddle_hit.wav");
    brickHitSound = LoadSound("assets/sounds/brick_hit.wav");
    gameOverSound = LoadSound("assets/sounds/game_over.wav");
    restartSound = LoadSound("assets/sounds/restart.wav");
}

// Função para descarregar os sons
void UnloadSounds(void) {
    UnloadSound(paddleHitSound);
    UnloadSound(brickHitSound);
    UnloadSound(gameOverSound);
    UnloadSound(restartSound);
}