#pragma once
#include <raylib.h>

#include <vector>

// Game constants
extern const int gameWidth;
extern const int gameHeight;
extern const int MAX_SCALE;
extern const int MAX_LIVES;
extern const float BASE_COOLDOWN_DURATION;
extern const float WAVE_TRANSITION_DURATION;

// Game state management
enum GameState
{
    MENU,
    NEW_GAME_CONFIRM,
    WAVE_TRANSITION,
    WAITING_TO_SPAWN,
    PLAYING,
    GAME_OVER
};

// Unified menu options
enum MenuOption
{
    MENU_RESUME = 0,
    MENU_NEW_GAME = 1,
    MENU_SCALE = 2,  // Cycles through 1x-4x
    MENU_EXIT = 3
};

// Game object structures
struct Ship
{
    float x, y;                  // position
    float rotation;              // angle in degrees
    float velocityX, velocityY;  // movement speed
};

struct Bullet
{
    float x, y;
    float velocityX, velocityY;
    bool active;
};

enum AsteroidSize
{
    SMALL,
    MEDIUM,
    LARGE
};

struct Asteroid
{
    float x, y;
    float velocityX, velocityY;
    AsteroidSize size;
    float rotation;
    bool active;
};

// Particle system for visual effects
struct Particle
{
    float x, y;
    float velocityX, velocityY;
    float life;     // Time remaining (0 = dead)
    float maxLife;  // Initial life for fade calculation
    Color color;
    float size;
    bool active;
};

// Global game state (will be defined in main.cpp)
extern GameState currentState;
extern int score;
extern int lives;
extern int currentWave;
extern float waveTransitionTimer;
extern float inputCooldownTimer;
extern int currentScale;
extern RenderTexture2D gameTexture;

// Game objects
extern Ship ship;
extern std::vector<Bullet> bullets;
extern std::vector<Asteroid> asteroids;
extern std::vector<Particle> particles;

// Menu state
extern int currentMainMenuOption;
extern int currentOptionsMenuOption;
extern bool exitRequested;

// Function declarations
// Rendering utilities
void DrawTextCentered(const char* text, int y, int fontSize, Color color, int screenWidth);
void DrawEnhancedHUD();
void DrawMenuBackground();
Vector2 RotatePoint(float relativeX, float relativeY, float rotationDegrees);

// Game object management
void SpawnAsteroidWave(std::vector<Asteroid>& asteroids, int wave, int screenWidth,
                       int screenHeight);

// Unified menu system
void UpdateUnifiedMenu();
void DrawUnifiedMenu();
void StartNewGame();

// Game logic system
void HandleBulletAsteroidCollisions();
bool HandleShipAsteroidCollisions();
void CheckWaveCompletion();
void CleanupInactiveObjects();
void ResetGameState();

// Physics system
void HandleShipInput();
void UpdateShipPhysics();
void UpdateBulletPhysics();
void UpdateAsteroidPhysics();
void UpdatePlayingPhysics();

// Audio system
void InitializeAudio();
void CleanupAudio();
void PlayShootSound();
void PlayThrustSound();
void PlayAsteroidExplosionSound(AsteroidSize size);
void PlayShipExplosionSound();
void PlayGameOverSound();
void UpdateThrustSound(bool isThrusting);

// Particle system
void UpdateParticles();
void DrawParticles();
void CreateExplosionParticles(float x, float y, AsteroidSize size);
void CreateShipExplosionParticles(float x, float y);
void CreateThrustParticles(float x, float y, float rotation);
void CleanupParticles();