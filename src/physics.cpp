#include <algorithm>
#include <cmath>

#include "common.h"

/**
 * @brief Processes ship movement input (WASD/Arrow keys) and shooting (Space)
 *
 * Handles rotation, thrust, braking, and shooting controls.
 * Creates thrust particles and plays audio effects.
 * Implements realistic physics with drag and momentum.
 */
void HandleShipInput()
{
    // Ship controls
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        ship.rotation -= 3.0f;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        ship.rotation += 3.0f;
    }

    // Ship thrust (forward) - reduced for more natural feel
    bool isThrusting = false;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
    {
        Vector2 thrustDirection = RotatePoint(0, -0.3f, ship.rotation);  // Reduced from -0.5f
        ship.velocityX += thrustDirection.x;
        ship.velocityY += thrustDirection.y;
        isThrusting = true;
        PlayThrustSound();
        CreateThrustParticles(ship.x, ship.y, ship.rotation);
    }

    // Ship reverse thrust/braking - realistic and balanced
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
    {
        // Calculate current speed
        float currentSpeed =
            sqrt(ship.velocityX * ship.velocityX + ship.velocityY * ship.velocityY);

        if (currentSpeed > 0.05f)  // Lower threshold for smoother transition
        {
            // Much gentler, realistic braking - no instant stops
            float brakeForce = 0.08f;  // Very gentle braking
            ship.velocityX -= (ship.velocityX / currentSpeed) * brakeForce;
            ship.velocityY -= (ship.velocityY / currentSpeed) * brakeForce;
        }

        // Always apply reverse thrust for immediate evasive capability
        Vector2 reverseDirection = RotatePoint(0, 0.45f, ship.rotation);  // Strong reverse
        ship.velocityX += reverseDirection.x * 0.4f;  // Very powerful reverse thrust
        ship.velocityY += reverseDirection.y * 0.4f;
    }

    // Update thrust sound state
    UpdateThrustSound(isThrusting);

    // Shooting
    if (IsKeyPressed(KEY_SPACE))
    {
        Bullet newBullet;
        Vector2 bulletDirection = RotatePoint(0, -8.0f, ship.rotation);
        newBullet.x = ship.x;
        newBullet.y = ship.y;
        newBullet.velocityX = ship.velocityX + bulletDirection.x;
        newBullet.velocityY = ship.velocityY + bulletDirection.y;
        newBullet.active = true;
        bullets.push_back(newBullet);

        // Play shoot sound
        PlayShootSound();
    }
}

/**
 * @brief Updates ship position, rotation, and applies drag/screen wrapping
 *
 * Applies physics drag, limits maximum speed, updates position,
 * and handles screen edge wrapping for seamless movement.
 */
void UpdateShipPhysics()
{
    // Apply drag
    ship.velocityX *= 0.99f;
    ship.velocityY *= 0.99f;

    // Limit speed
    float speed = sqrt(ship.velocityX * ship.velocityX + ship.velocityY * ship.velocityY);
    if (speed > 8.0f)
    {
        ship.velocityX = (ship.velocityX / speed) * 8.0f;
        ship.velocityY = (ship.velocityY / speed) * 8.0f;
    }

    // Update ship position
    ship.x += ship.velocityX;
    ship.y += ship.velocityY;

    // Screen wrapping for ship
    if (ship.x < 0)
        ship.x = gameWidth;
    if (ship.x > gameWidth)
        ship.x = 0;
    if (ship.y < 0)
        ship.y = gameHeight;
    if (ship.y > gameHeight)
        ship.y = 0;
}

/**
 * @brief Updates bullet positions, handles screen wrapping and lifetime
 *
 * Moves bullets based on their velocity and deactivates bullets
 * that travel beyond screen boundaries.
 */
void UpdateBulletPhysics()
{
    for (auto& bullet : bullets)
    {
        if (bullet.active)
        {
            bullet.x += bullet.velocityX;
            bullet.y += bullet.velocityY;

            // Remove bullets that go off screen
            if (bullet.x < 0 || bullet.x > gameWidth || bullet.y < 0 || bullet.y > gameHeight)
            {
                bullet.active = false;
            }
        }
    }
}

/**
 * @brief Updates asteroid positions, rotation, and screen wrapping
 *
 * Moves asteroids based on their velocity and handles screen edge
 * wrapping to keep asteroids in play area.
 */
void UpdateAsteroidPhysics()
{
    for (auto& asteroid : asteroids)
    {
        if (asteroid.active)
        {
            asteroid.x += asteroid.velocityX;
            asteroid.y += asteroid.velocityY;

            // Screen wrapping for asteroids
            if (asteroid.x < 0)
                asteroid.x = gameWidth;
            if (asteroid.x > gameWidth)
                asteroid.x = 0;
            if (asteroid.y < 0)
                asteroid.y = gameHeight;
            if (asteroid.y > gameHeight)
                asteroid.y = 0;
        }
    }
}

/**
 * @brief Updates all physics systems during PLAYING state
 *
 * Coordinates updates for ship, bullets, and asteroids in proper order.
 */
void UpdatePlayingPhysics()
{
    HandleShipInput();
    UpdateShipPhysics();
    UpdateBulletPhysics();
    UpdateAsteroidPhysics();
}