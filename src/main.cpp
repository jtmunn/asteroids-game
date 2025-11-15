#include <algorithm>
#include <cmath>

#include "common.h"

// Game constants (definitions for header declarations)
const int gameWidth = 800;
const int gameHeight = 600;
const int MAX_SCALE = 4;

// Game state variables
GameState currentState = MENU;
int score = 0;
int lives = 0;  // No active game at startup
int currentWave = 1;
float waveTransitionTimer = 0.0f;
float inputCooldownTimer = 0.0f;
int currentScale = 2;
RenderTexture2D gameTexture;

// Game objects
Ship ship = {gameWidth / 2.0f, gameHeight / 2.0f, 0.0f, 0.0f, 0.0f};
std::vector<Bullet> bullets;
std::vector<Asteroid> asteroids;
std::vector<Particle> particles;

// Menu state
int currentMainMenuOption = 0;
int currentOptionsMenuOption = 0;
bool exitRequested = false;

// Game constants (definitions for header declarations)
const int MAX_LIVES = 4;
const float BASE_COOLDOWN_DURATION = 1.0f;
const float PENALTY_PER_DEATH = 1.5f;  // More severe death penalty
const float WAVE_TRANSITION_DURATION = 2.0f;

int main()
{
    // Initialize window with initial scale
    InitWindow(gameWidth * currentScale, gameHeight * currentScale, "Asteroids Game");

    // Create render texture for pixel-perfect scaling
    gameTexture = LoadRenderTexture(gameWidth, gameHeight);

    // Initialize audio system
    InitializeAudio();

    SetTargetFPS(60);
    SetExitKey(0);  // Disable default ESC to exit behavior

    // Game loop
    bool shouldExit = false;
    while (!WindowShouldClose() && !shouldExit)
    {
        // ESC key handling - only works during active gameplay states
        if (IsKeyPressed(KEY_ESCAPE))
        {
            if (currentState == PLAYING || currentState == WAITING_TO_SPAWN)
            {
                // During gameplay, ESC goes to menu
                currentState = MENU;
            }
            // ESC does nothing in MENU, GAME_OVER, or WAVE_TRANSITION states
        }

        // UPDATE - Handle different game states
        switch (currentState)
        {
            case MENU:
                UpdateUnifiedMenu();
                // Check if exit was requested from menu
                if (exitRequested)
                {
                    shouldExit = true;
                }
                break;

            case NEW_GAME_CONFIRM:
            {
                // Handle Y/N confirmation for new game
                if (IsKeyPressed(KEY_Y))
                {
                    StartNewGame();
                }
                else if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE))
                {
                    currentState = MENU;
                }
                // Any other key does nothing
            }
            break;

            case GAME_OVER:
            {
                // Game over input handling
                if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER))
                {
                    currentState = WAVE_TRANSITION;

                    // Reset game state
                    ResetGameState();
                    // Simple cooldown to prevent accidental respawn
                    inputCooldownTimer = BASE_COOLDOWN_DURATION;
                }
                else if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE))
                {
                    currentState = MENU;
                }
            }
            break;

            case WAVE_TRANSITION:
            {
                // Wave transition timing
                waveTransitionTimer -= GetFrameTime();
                if (waveTransitionTimer <= 0.0f)
                {
                    // Start the wave
                    currentState = WAITING_TO_SPAWN;
                    SpawnAsteroidWave(asteroids, currentWave, gameWidth, gameHeight);
                }
            }
            break;

            case WAITING_TO_SPAWN:
            {
                // Handle input cooldown countdown
                if (inputCooldownTimer > 0.0f)
                {
                    inputCooldownTimer -= GetFrameTime();
                }

                // Handle safe spawn waiting (only after cooldown expires)
                if (inputCooldownTimer <= 0.0f &&
                    (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_W) ||
                     IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT) ||
                     IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_D) ||
                     IsKeyPressed(KEY_RIGHT)))
                {
                    currentState = PLAYING;
                    // Reset ship position and velocity for safe spawn
                    ship.x = gameWidth / 2.0f;
                    ship.y = gameHeight / 2.0f;
                    ship.rotation = 0.0f;
                    ship.velocityX = 0.0f;
                    ship.velocityY = 0.0f;
                }

                // Update asteroids so they keep moving
                UpdateAsteroidPhysics();
            }
            break;

            case PLAYING:
            {
                // Update all physics systems
                UpdatePlayingPhysics();

                // Collision detection: bullets vs asteroids
                HandleBulletAsteroidCollisions();

                // Collision detection: ship vs asteroids
                if (HandleShipAsteroidCollisions())
                {
                    break;  // Ship was destroyed, exit collision checking
                }

                // Check if wave is complete
                CheckWaveCompletion();

                // Clean up inactive objects
                CleanupInactiveObjects();
            }
            break;

                // PAUSED state removed - now handled by unified menu
        }

        // Update particle system in all states
        UpdateParticles();
        CleanupParticles();

        // DRAW - Render to texture first (game resolution)
        BeginTextureMode(gameTexture);
        ClearBackground(BLACK);

        switch (currentState)
        {
            case MENU:
                DrawUnifiedMenu();
                break;

            case NEW_GAME_CONFIRM:
            {
                // Draw current game state in background (dimmed)
                DrawRectangle(0, 0, gameWidth, gameHeight, Fade(BLACK, 0.8f));

                // Animated confirmation dialog
                static float confirmTimer = 0.0f;
                confirmTimer += GetFrameTime() * 4.0f;
                float confirmPulse = 0.7f + 0.3f * sinf(confirmTimer);

                DrawRectangle(gameWidth / 2 - 180, gameHeight / 2 - 80, 360, 120,
                              Fade(BLACK, 0.9f));
                DrawRectangleLines(gameWidth / 2 - 180, gameHeight / 2 - 80, 360, 120,
                                   Fade(YELLOW, confirmPulse));

                // Confirmation dialog with pulse effect
                DrawTextCentered("START NEW GAME?", gameHeight / 2 - 61, 32, BLACK,
                                 gameWidth);  // Shadow
                DrawTextCentered("START NEW GAME?", gameHeight / 2 - 60, 32,
                                 Fade(YELLOW, confirmPulse), gameWidth);
                DrawTextCentered("Current game will be lost!", gameHeight / 2 - 20, 18, RED,
                                 gameWidth);
                DrawTextCentered("Press Y to confirm, N to cancel", gameHeight / 2 + 20, 20, WHITE,
                                 gameWidth);
            }
            break;

            case GAME_OVER:
            {
                // Enhanced game over screen
                DrawMenuBackground();

                // Main game over panel
                DrawRectangle(gameWidth / 2 - 200, gameHeight / 2 - 120, 400, 240,
                              Fade(BLACK, 0.8f));
                DrawRectangleLines(gameWidth / 2 - 200, gameHeight / 2 - 120, 400, 240, RED);
                DrawRectangleLines(gameWidth / 2 - 198, gameHeight / 2 - 118, 396, 236,
                                   Fade(RED, 0.5f));

                // Game over title with effect
                DrawTextCentered("GAME OVER", gameHeight / 2 - 81, 40, BLACK, gameWidth);  // Shadow
                DrawTextCentered("GAME OVER", gameHeight / 2 - 80, 40, RED, gameWidth);

                // Stats panel
                DrawRectangle(gameWidth / 2 - 180, gameHeight / 2 - 40, 360, 80,
                              Fade(DARKGRAY, 0.3f));
                DrawRectangleLines(gameWidth / 2 - 180, gameHeight / 2 - 40, 360, 80, WHITE);

                DrawTextCentered("FINAL STATISTICS", gameHeight / 2 - 30, 16, YELLOW, gameWidth);
                DrawTextCentered(TextFormat("Score: %d", score), gameHeight / 2 - 10, 20, WHITE,
                                 gameWidth);
                DrawTextCentered(TextFormat("Wave Reached: %d", currentWave), gameHeight / 2 + 15,
                                 20, WHITE, gameWidth);

                // Controls
                DrawTextCentered("Press R or ENTER to Restart", gameHeight / 2 + 50, 18, LIGHTGRAY,
                                 gameWidth);
                DrawTextCentered("Press M or ESC for Menu", gameHeight / 2 + 75, 18, LIGHTGRAY,
                                 gameWidth);
            }
            break;

            case WAVE_TRANSITION:
            {
                // Enhanced wave transition screen
                DrawMenuBackground();
                DrawRectangle(0, 0, gameWidth, gameHeight, Fade(BLACK, 0.7f));

                // Animated wave panel
                static float pulseTimer = 0.0f;
                pulseTimer += GetFrameTime() * 3.0f;
                float pulse = 0.8f + 0.2f * sinf(pulseTimer);

                DrawRectangle(gameWidth / 2 - 200, gameHeight / 2 - 80, 400, 160,
                              Fade(BLACK, 0.9f));
                DrawRectangleLines(gameWidth / 2 - 200, gameHeight / 2 - 80, 400, 160,
                                   Fade(YELLOW, pulse));
                DrawRectangleLines(gameWidth / 2 - 198, gameHeight / 2 - 78, 396, 156,
                                   Fade(YELLOW, pulse * 0.5f));

                // Wave title with glow effect
                DrawTextCentered(TextFormat("WAVE %d", currentWave), gameHeight / 2 - 41, 40, BLACK,
                                 gameWidth);  // Shadow
                DrawTextCentered(TextFormat("WAVE %d", currentWave), gameHeight / 2 - 40, 40,
                                 Fade(YELLOW, pulse), gameWidth);

                DrawTextCentered("Incoming Asteroids!", gameHeight / 2 + 10, 20, WHITE, gameWidth);

                // Enhanced progress bar with glow
                float progress = 1.0f - (waveTransitionTimer / WAVE_TRANSITION_DURATION);
                int barWidth = (int)(300 * progress);

                // Progress bar background
                DrawRectangle(gameWidth / 2 - 152, gameHeight / 2 + 48, 304, 12, BLACK);
                DrawRectangleLines(gameWidth / 2 - 152, gameHeight / 2 + 48, 304, 12, WHITE);

                // Progress fill with gradient effect
                DrawRectangle(gameWidth / 2 - 150, gameHeight / 2 + 50, barWidth, 8, YELLOW);
                if (barWidth > 0)
                {
                    DrawRectangle(gameWidth / 2 - 150, gameHeight / 2 + 50, barWidth, 4,
                                  Fade(WHITE, 0.8f));
                }

                DrawTextCentered(TextFormat("%.1f", waveTransitionTimer), gameHeight / 2 + 75, 16,
                                 LIGHTGRAY, gameWidth);
            }
            break;

            case WAITING_TO_SPAWN:
            {
                // Draw starfield background
                DrawMenuBackground();

                // Draw all bullets
                for (const auto& bullet : bullets)
                {
                    if (bullet.active)
                    {
                        DrawCircle(bullet.x, bullet.y, 2, WHITE);
                    }
                }

                // Draw asteroids
                for (const auto& asteroid : asteroids)
                {
                    if (asteroid.active)
                    {
                        float radius = (asteroid.size == LARGE)    ? 30
                                       : (asteroid.size == MEDIUM) ? 20
                                                                   : 10;
                        DrawCircleLines(asteroid.x, asteroid.y, radius, WHITE);
                    }
                }

                // Draw safe spawn overlay
                DrawRectangle(0, 0, gameWidth, gameHeight, Fade(BLACK, 0.4f));

                if (inputCooldownTimer > 0)
                {
                    DrawTextCentered("READY TO SPAWN", gameHeight / 2 - 40, 30, GRAY, gameWidth);
                    DrawTextCentered(TextFormat("Wait %.1f seconds...", inputCooldownTimer),
                                     gameHeight / 2 + 10, 20, RED, gameWidth);
                    DrawTextCentered("Input blocked to prevent accidental spawning",
                                     gameHeight / 2 + 35, 16, ORANGE, gameWidth);
                }
                else
                {
                    DrawTextCentered("READY TO SPAWN", gameHeight / 2 - 40, 30, GREEN, gameWidth);
                    DrawTextCentered("Press SPACE, ENTER, or any movement key when ready",
                                     gameHeight / 2 + 10, 18, WHITE, gameWidth);
                    DrawTextCentered("Choose a safe spot away from asteroids!", gameHeight / 2 + 35,
                                     16, YELLOW, gameWidth);
                }

                // Draw HUD on top of everything
                DrawEnhancedHUD();
            }
            break;

            case PLAYING:
            {
                // Draw starfield background
                DrawMenuBackground();

                // Draw ship (only in normal playing state)
                if (currentState == PLAYING)
                {
                    // Rotate all three points using RotatePoint function
                    Vector2 tip = RotatePoint(0, -10, ship.rotation);
                    Vector2 leftWing = RotatePoint(-8, 8, ship.rotation);
                    Vector2 rightWing = RotatePoint(8, 8, ship.rotation);

                    // Draw ship triangle
                    DrawTriangle((Vector2){ship.x + tip.x, ship.y + tip.y},
                                 (Vector2){ship.x + leftWing.x, ship.y + leftWing.y},
                                 (Vector2){ship.x + rightWing.x, ship.y + rightWing.y}, WHITE);
                }

                // Draw bullets
                for (const auto& bullet : bullets)
                {
                    if (bullet.active)
                    {
                        DrawCircle(bullet.x, bullet.y, 2, WHITE);
                    }
                }

                // Draw asteroids
                for (const auto& asteroid : asteroids)
                {
                    if (asteroid.active)
                    {
                        float radius = (asteroid.size == LARGE)    ? 30
                                       : (asteroid.size == MEDIUM) ? 20
                                                                   : 10;
                        DrawCircleLines(asteroid.x, asteroid.y, radius, WHITE);
                    }
                }

                // Draw particle effects
                DrawParticles();

                // Draw HUD on top of everything
                DrawEnhancedHUD();

                // No pause overlay needed - ESC goes directly to menu
            }
            break;
        }

        EndTextureMode();

        // Now draw the texture to the actual window (scaled)
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw the game texture scaled to fill the window
        DrawTexturePro(gameTexture.texture,
                       (Rectangle){0, 0, (float)gameWidth, -(float)gameHeight},  // Source (flip Y)
                       (Rectangle){0, 0, (float)(gameWidth * currentScale),
                                   (float)(gameHeight * currentScale)},  // Dest
                       (Vector2){0, 0},                                  // Origin
                       0.0f,                                             // Rotation
                       WHITE);                                           // Tint

        EndDrawing();
    }

    // Cleanup
    CleanupAudio();
    UnloadRenderTexture(gameTexture);
    CloseWindow();
    return 0;
}
