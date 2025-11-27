#include "effects.h"
#include <cmath>
#include <cstdlib>

void InitParticleSystem(ParticleSystem* system, int initialCapacity) {
    system->particles = new Particle[initialCapacity];
    system->capacity = initialCapacity;
    system->count = 0;
    system->initialized = true;
}

void FreeParticleSystem(ParticleSystem* system) {
    if (system->particles != nullptr) {
        delete[] system->particles;
        system->particles = nullptr;
    }
    system->capacity = 0;
    system->count = 0;
    system->initialized = false;
}

void ResizeParticleSystem(ParticleSystem* system, int newCapacity) {
    if (newCapacity <= system->capacity) return;
    
    Particle* newParticles = new Particle[newCapacity];
    
    for (int i = 0; i < system->count; i++) {
        newParticles[i] = system->particles[i];
    }
    
    // CORREÇÃO: Liberar memória antiga antes de atribuir nova
    if (system->particles != nullptr) {
        delete[] system->particles;
    }
    
    system->particles = newParticles;
    system->capacity = newCapacity;
}

void CreateExplosionEffect(ParticleSystem* system, Vector2 position, Color color, int intensity) {
    int particlesToCreate = intensity * 10;
    
    if (system->count + particlesToCreate > system->capacity) {
        int newCapacity = system->capacity * 2 + particlesToCreate;
        ResizeParticleSystem(system, newCapacity);
    }
    
    for (int i = 0; i < particlesToCreate && system->count < system->capacity; i++) {
        Particle* p = &system->particles[system->count];
        
        p->position = position;
        float angle = GetRandomValue(0, 360) * 3.14159f / 180.0f;
        float speed = GetRandomValue(50, 200) / 10.0f;
        p->velocity = { cosf(angle) * speed, sinf(angle) * speed };
        p->color = color;
        p->life = 1.0f;
        p->size = (float)GetRandomValue(2, 6);
        
        system->count++;
    }
}

void UpdateParticles(ParticleSystem* system, float deltaTime) {
    Particle* particles = system->particles;
    int writeIndex = 0;
    
    for (int readIndex = 0; readIndex < system->count; readIndex++) {
        Particle* p = &particles[readIndex];
        
        p->position.x += p->velocity.x * deltaTime;
        p->position.y += p->velocity.y * deltaTime;
        p->life -= deltaTime;
        
        if (p->life > 0.0f) {
            if (writeIndex != readIndex) {
                particles[writeIndex] = particles[readIndex];
            }
            writeIndex++;
        }
    }
    
    system->count = writeIndex;
}

void DrawParticles(const ParticleSystem* system) {
    const Particle* particles = system->particles;
    
    for (int i = 0; i < system->count; i++) {
        const Particle* p = &particles[i];
        DrawCircleV(p->position, p->size, ColorAlpha(p->color, p->life));
    }
}