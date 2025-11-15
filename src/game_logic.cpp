#include <algorithm>
#include <cmath>

#include "common.h"

/**
 * @brief Handles collision detection between bullets and asteroids, creates explosions
 *
 * Detects collisions using distance calculation, splits asteroids into smaller pieces,
 * updates score, plays explosion effects, and creates particle effects.
 */
void HandleBulletAsteroidCollisions()
{
    for (auto& bullet : bullets)
    {
        if (!bullet.active)
            continue;

        for (auto& asteroid : asteroids)
        {
            if (!asteroid.active)
                continue;

            float radius = (asteroid.size == LARGE) ? 30 : (asteroid.size == MEDIUM) ? 20 : 10;
            float distance = sqrt(pow(bullet.x - asteroid.x, 2) + pow(bullet.y - asteroid.y, 2));

            if (distance < radius + 2)  // 2 is bullet radius
            {
                // Bullet hit asteroid
                bullet.active = false;
                asteroid.active = false;

                // Create explosion effects
                PlayAsteroidExplosionSound(asteroid.size);
                CreateExplosionParticles(asteroid.x, asteroid.y, asteroid.size);

                // Award points
                switch (asteroid.size)
                {
                    case LARGE:
                        score += 20;
                        break;
                    case MEDIUM:
                        score += 50;
                        break;
                    case SMALL:
                        score += 100;
                        break;
                }

                // Split asteroid if not small
                if (asteroid.size != SMALL)
                {
                    for (int i = 0; i < 2; i++)
                    {
                        Asteroid fragment;
                        fragment.x = asteroid.x;
                        fragment.y = asteroid.y;
                        fragment.velocityX = asteroid.velocityX + GetRandomValue(-2, 2);
                        fragment.velocityY = asteroid.velocityY + GetRandomValue(-2, 2);
                        fragment.size = (asteroid.size == LARGE) ? MEDIUM : SMALL;
                        fragment.rotation = 0.0f;
                        fragment.active = true;
                        asteroids.push_back(fragment);
                    }
                }
                break;
            }
        }
    }
}

/**
 * @brief Handles collision detection between ship and asteroids
 * @return true if ship was destroyed, false otherwise
 *
 * Detects ship-asteroid collisions using distance calculation.
 * Creates explosion effects and decreases lives when ship is destroyed.
 */
bool HandleShipAsteroidCollisions()
{
    for (auto& asteroid : asteroids)
    {
        if (!asteroid.active)
            continue;

        float asteroidRadius = (asteroid.size == LARGE) ? 30 : (asteroid.size == MEDIUM) ? 20 : 10;
        float shipRadius = 8.0f;
        float distance = sqrt(pow(ship.x - asteroid.x, 2) + pow(ship.y - asteroid.y, 2));

        if (distance < asteroidRadius + shipRadius)
        {
            // Ship destroyed!
            PlayShipExplosionSound();
            CreateShipExplosionParticles(ship.x, ship.y);
            lives--;
            if (lives > 0)
            {
                // Use safe spawn system for respawn
                currentState = WAITING_TO_SPAWN;
                // Simple cooldown to prevent accidental respawn
                inputCooldownTimer = BASE_COOLDOWN_DURATION;
            }
            else
            {
                // Game over - check and update high score
                CheckAndUpdateHighScore();
                currentState = GAME_OVER;
                PlayGameOverSound();
            }
            return true;  // Collision occurred
        }
    }
    return false;  // No collision
}

/**
 * @brief Checks if all asteroids are destroyed and advances to next wave
 *
 * Scans all asteroids to determine if wave is complete.
 * Increments wave counter and starts transition when all asteroids destroyed.
 */
void CheckWaveCompletion()
{
    bool allAsteroidsDestroyed = true;
    for (const auto& asteroid : asteroids)
    {
        if (asteroid.active)
        {
            allAsteroidsDestroyed = false;
            break;
        }
    }

    if (allAsteroidsDestroyed)
    {
        // Start next wave
        currentWave++;
        currentState = WAVE_TRANSITION;
        waveTransitionTimer = WAVE_TRANSITION_DURATION;
    }
}

/**
 * @brief Removes inactive bullets and asteroids from game vectors
 *
 * Uses STL remove_if to efficiently clean up deactivated game objects
 * and maintain optimal performance.
 */
void CleanupInactiveObjects()
{
    // Clean up inactive bullets
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }),
        bullets.end());

    // Clean up inactive asteroids
    asteroids.erase(std::remove_if(asteroids.begin(), asteroids.end(),
                                   [](const Asteroid& a) { return !a.active; }),
                    asteroids.end());
}

// Spawn asteroids for a new wave
void SpawnAsteroidWave(std::vector<Asteroid>& asteroids, int wave, int screenWidth,
                       int screenHeight)
{
    // Clear existing asteroids
    asteroids.clear();

    // Calculate number of asteroids for this wave (starts with 4, increases by 2 each wave)
    int asteroidCount = 3 + wave;  // Wave 1 = 4 asteroids, Wave 2 = 5, etc.

    for (int i = 0; i < asteroidCount; i++)
    {
        Asteroid newAsteroid;

        // Spawn asteroids away from center to avoid immediate collision
        do
        {
            newAsteroid.x = GetRandomValue(0, screenWidth);
            newAsteroid.y = GetRandomValue(0, screenHeight);
        } while (abs(newAsteroid.x - screenWidth / 2) < 100 &&
                 abs(newAsteroid.y - screenHeight / 2) < 100);

        // Velocity increases slightly with wave number
        float speedMultiplier = 1.0f + (wave - 1) * 0.2f;
        newAsteroid.velocityX = (float)GetRandomValue(-2, 2) * speedMultiplier;
        newAsteroid.velocityY = (float)GetRandomValue(-2, 2) * speedMultiplier;
        newAsteroid.size = LARGE;
        newAsteroid.rotation = 0.0f;
        newAsteroid.active = true;
        asteroids.push_back(newAsteroid);
    }
}

/**
 * @brief Resets all game variables to initial state
 *
 * Resets score, lives, wave counter, ship position/velocity,
 * clears all game objects, and prepares for new game.
 */
void ResetGameState()
{
    score = 0;
    lives = MAX_LIVES;
    currentWave = 1;
    waveTransitionTimer = WAVE_TRANSITION_DURATION;

    // Reset ship
    ship.x = gameWidth / 2.0f;
    ship.y = gameHeight / 2.0f;
    ship.rotation = 0.0f;
    ship.velocityX = 0.0f;
    ship.velocityY = 0.0f;

    // Clear bullets and asteroids
    bullets.clear();
    asteroids.clear();
}