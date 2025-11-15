#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "common.h"

// Particle system for visual effects in Asteroids

void UpdateParticles()
{
    for (auto& particle : particles)
    {
        if (!particle.active)
            continue;

        // Update position
        particle.x += particle.velocityX;
        particle.y += particle.velocityY;

        // Apply drag for more natural movement
        particle.velocityX *= 0.98f;
        particle.velocityY *= 0.98f;

        // Update life
        particle.life -= GetFrameTime();
        if (particle.life <= 0.0f)
        {
            particle.active = false;
        }

        // Screen wrapping for particles
        if (particle.x < 0)
            particle.x = gameWidth;
        if (particle.x > gameWidth)
            particle.x = 0;
        if (particle.y < 0)
            particle.y = gameHeight;
        if (particle.y > gameHeight)
            particle.y = 0;
    }
}

void DrawParticles()
{
    for (const auto& particle : particles)
    {
        if (!particle.active)
            continue;

        // Calculate fade based on remaining life
        float fadeRatio = particle.life / particle.maxLife;
        Color fadedColor = particle.color;
        fadedColor.a = (unsigned char)(255 * fadeRatio);

        // Draw particle as a small circle or square
        float currentSize = particle.size * fadeRatio;  // Shrink as it fades
        DrawCircleV({particle.x, particle.y}, currentSize, fadedColor);
    }
}

void CreateExplosionParticles(float x, float y, AsteroidSize size)
{
    int particleCount;
    float particleSpeed;
    float particleLife;
    Color baseColor;

    // Different explosion characteristics based on asteroid size
    switch (size)
    {
        case SMALL:
            particleCount = 6;
            particleSpeed = 3.0f;
            particleLife = 0.8f;
            baseColor = WHITE;
            break;
        case MEDIUM:
            particleCount = 8;
            particleSpeed = 4.0f;
            particleLife = 1.2f;
            baseColor = WHITE;
            break;
        case LARGE:
            particleCount = 12;
            particleSpeed = 5.0f;
            particleLife = 1.5f;
            baseColor = WHITE;
            break;
    }

    // Create particles in all directions
    for (int i = 0; i < particleCount; i++)
    {
        Particle particle;
        particle.x = x;
        particle.y = y;

        // Random direction
        float angle = (float)i / particleCount * 2.0f * PI + ((float)rand() / RAND_MAX) * 0.5f;
        float speed = particleSpeed * (0.5f + (float)rand() / RAND_MAX * 0.5f);

        particle.velocityX = cosf(angle) * speed;
        particle.velocityY = sinf(angle) * speed;

        particle.life = particleLife * (0.7f + (float)rand() / RAND_MAX * 0.3f);
        particle.maxLife = particle.life;

        // Simple retro colors
        particle.color = WHITE;
        if (rand() % 3 == 0)
            particle.color = LIGHTGRAY;  // Some gray particles

        particle.size = 2.0f + (float)rand() / RAND_MAX * 2.0f;
        particle.active = true;

        particles.push_back(particle);
    }
}

void CreateShipExplosionParticles(float x, float y)
{
    // Ship explosion with moderate debris
    int particleCount = 15;

    for (int i = 0; i < particleCount; i++)
    {
        Particle particle;
        particle.x = x + ((float)rand() / RAND_MAX - 0.5f) * 10.0f;  // Spread around ship
        particle.y = y + ((float)rand() / RAND_MAX - 0.5f) * 10.0f;

        // Random direction with more spread
        float angle = (float)rand() / RAND_MAX * 2.0f * PI;
        float speed = 2.0f + (float)rand() / RAND_MAX * 6.0f;

        particle.velocityX = cosf(angle) * speed;
        particle.velocityY = sinf(angle) * speed;

        particle.life = 2.0f + (float)rand() / RAND_MAX * 1.0f;
        particle.maxLife = particle.life;

        // Simple retro colors
        if (i < 8)
            particle.color = WHITE;  // Hot fragments
        else
            particle.color = LIGHTGRAY;  // Debris

        particle.size = 1.5f + (float)rand() / RAND_MAX * 3.0f;
        particle.active = true;

        particles.push_back(particle);
    }
}

void CreateThrustParticles(float x, float y, float rotation)
{
    // Create a few small thrust particles behind the ship
    int particleCount = 3;

    for (int i = 0; i < particleCount; i++)
    {
        Particle particle;

        // Position particles behind the ship
        float thrustOffset = 15.0f + i * 5.0f;  // Stagger particles
        Vector2 thrustPos = RotatePoint(0, thrustOffset, rotation);
        particle.x = x + thrustPos.x + ((float)rand() / RAND_MAX - 0.5f) * 4.0f;
        particle.y = y + thrustPos.y + ((float)rand() / RAND_MAX - 0.5f) * 4.0f;

        // Particles move in thrust direction with some spread
        Vector2 thrustDir = RotatePoint(0, 1.0f, rotation);  // Opposite of ship direction
        particle.velocityX = thrustDir.x * (1.0f + (float)rand() / RAND_MAX) +
                             ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
        particle.velocityY = thrustDir.y * (1.0f + (float)rand() / RAND_MAX) +
                             ((float)rand() / RAND_MAX - 0.5f) * 2.0f;

        particle.life = 0.3f + (float)rand() / RAND_MAX * 0.2f;  // Short-lived
        particle.maxLife = particle.life;

        // Thrust colors - blue/white exhaust
        particle.color = (i == 0) ? WHITE : SKYBLUE;

        particle.size = 1.0f + (float)rand() / RAND_MAX * 1.5f;
        particle.active = true;

        particles.push_back(particle);
    }
}

void CleanupParticles()
{
    // Remove inactive particles to prevent memory bloat
    particles.erase(std::remove_if(particles.begin(), particles.end(),
                                   [](const Particle& p) { return !p.active; }),
                    particles.end());
}