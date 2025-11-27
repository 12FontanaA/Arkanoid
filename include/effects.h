#ifndef EFFECTS_H
#define EFFECTS_H

#include "raylib.h"

struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float life;
    float size;
};

// Sistema de partículas com alocação dinâmica
struct ParticleSystem {
    Particle* particles;    // Array dinâmico de partículas
    int capacity;           // Capacidade total do array
    int count;              // Número atual de partículas
    bool initialized;       // Flag para verificar se foi inicializado
};

// Funções para gerenciar o sistema de partículas
void InitParticleSystem(ParticleSystem* system, int initialCapacity);
void FreeParticleSystem(ParticleSystem* system);
void CreateExplosionEffect(ParticleSystem* system, Vector2 position, Color color, int intensity);
void UpdateParticles(ParticleSystem* system, float deltaTime);
void DrawParticles(const ParticleSystem* system);
void ResizeParticleSystem(ParticleSystem* system, int newCapacity);

#endif