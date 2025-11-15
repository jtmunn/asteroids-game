#pragma once
#include <raylib.h>

#include <string>
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
    HIGH_SCORES,
    WAVE_TRANSITION,
    WAITING_TO_SPAWN,
    PLAYING,
    PAUSED,
    GAME_OVER
};

// Dynamic menu system structures
enum MenuActionType
{
    ACTION_RESUME,
    ACTION_NEW_GAME,
    ACTION_HIGH_SCORES,
    ACTION_SCALE,
    ACTION_EXIT
};

struct MenuItem
{
    std::string label;      // Display text
    MenuActionType action;  // What this menu item does
    bool dynamicLabel;      // If true, label may change (like Scale: 2x)
};

// Menu system globals
extern std::vector<MenuItem> currentMenuItems;
extern int selectedMenuIndex;

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
extern int highScore;
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
/**
 * @brief Draws text centered horizontally on screen
 * @param text The text string to display
 * @param y Vertical position in pixels from top of screen
 * @param fontSize Size of the font in pixels
 * @param color Text color (Raylib Color struct)
 * @param screenWidth Total screen width in pixels for centering calculation
 */
void DrawTextCentered(const char* text, int y, int fontSize, Color color, int screenWidth);

/**
 * @brief Draws the enhanced HUD showing score, lives as ship icons, and wave number
 */
void DrawEnhancedHUD();

/**
 * @brief Draws animated starfield background with twinkling stars and floating asteroids
 */
void DrawMenuBackground();

// High score system
/**
 * @brief Loads high score from highscore.dat file, sets to 0 if file doesn't exist
 */
void LoadHighScore();

/**
 * @brief Saves current high score to highscore.dat file
 */
void SaveHighScore();

/**
 * @brief Checks if current score is higher than high score and updates if so
 */
void CheckAndUpdateHighScore();

/**
 * @brief Rotates a 2D point around origin by specified degrees
 * @param relativeX X coordinate relative to rotation origin
 * @param relativeY Y coordinate relative to rotation origin
 * @param rotationDegrees Rotation angle in degrees (positive = clockwise)
 * @return Vector2 containing the rotated coordinates
 */
Vector2 RotatePoint(float relativeX, float relativeY, float rotationDegrees);

// Game object management
/**
 * @brief Spawns a wave of asteroids with count based on wave number
 * @param asteroids Reference to asteroid vector to populate
 * @param wave Current wave number (affects asteroid count)
 * @param screenWidth Screen width in pixels for positioning
 * @param screenHeight Screen height in pixels for positioning
 */
void SpawnAsteroidWave(std::vector<Asteroid>& asteroids, int wave, int screenWidth,
                       int screenHeight);

// Unified menu system
/**
 * @brief Updates menu input handling and state transitions
 */
void UpdateUnifiedMenu();

/**
 * @brief Draws the main menu with animated backgrounds and highlighted options
 */
void DrawUnifiedMenu();

/**
 * @brief Initializes a new game by resetting all game state variables
 */
void StartNewGame();

// Game logic system
/**
 * @brief Handles collision detection between bullets and asteroids, creates explosions
 */
void HandleBulletAsteroidCollisions();

/**
 * @brief Handles collision detection between ship and asteroids
 * @return true if ship was destroyed, false otherwise
 */
bool HandleShipAsteroidCollisions();

/**
 * @brief Checks if all asteroids are destroyed and advances to next wave
 */
void CheckWaveCompletion();

/**
 * @brief Removes inactive bullets and asteroids from game vectors
 */
void CleanupInactiveObjects();

/**
 * @brief Resets all game variables to initial state
 */
void ResetGameState();

// Physics system
/**
 * @brief Processes ship movement input (WASD/Arrow keys) and shooting (Space)
 */
void HandleShipInput();

/**
 * @brief Updates ship position, rotation, and applies drag/screen wrapping
 */
void UpdateShipPhysics();

/**
 * @brief Updates bullet positions, handles screen wrapping and lifetime
 */
void UpdateBulletPhysics();

/**
 * @brief Updates asteroid positions, rotation, and screen wrapping
 */
void UpdateAsteroidPhysics();

/**
 * @brief Updates all physics systems during PLAYING state
 */
void UpdatePlayingPhysics();

// Audio system
/**
 * @brief Initializes Raylib audio system and generates procedural sounds
 */
void InitializeAudio();

/**
 * @brief Cleans up audio resources and closes audio system
 */
void CleanupAudio();

/**
 * @brief Plays subtle laser shooting sound effect
 */
void PlayShootSound();

/**
 * @brief Plays quiet thrust engine sound effect
 */
void PlayThrustSound();

/**
 * @brief Plays asteroid explosion sound based on size
 * @param size Asteroid size (affects explosion sound intensity)
 */
void PlayAsteroidExplosionSound(AsteroidSize size);

/**
 * @brief Plays dramatic ship destruction sound effect
 */
void PlayShipExplosionSound();

/**
 * @brief Plays game over music sequence
 */
void PlayGameOverSound();

/**
 * @brief Updates thrust sound based on current thrusting state
 * @param isThrusting true if ship is currently thrusting
 */
void UpdateThrustSound(bool isThrusting);

// Dynamic menu system
/**
 * @brief Resets menu state when context changes
 */
void ResetMenuState();

/**
 * @brief Builds menu items list based on current context
 * @param context Menu context (MENU, PAUSED, etc.)
 */
void BuildMenuItems(GameState context);

/**
 * @brief Handles menu navigation and selection input
 */
void UpdateDynamicMenu();

/**
 * @brief Renders the dynamic menu with proper spacing
 */
void DrawDynamicMenu();

/**
 * @brief Executes the selected menu action
 * @param action The menu action to perform
 */
void ExecuteMenuAction(MenuActionType action);

// Particle system
/**
 * @brief Updates all active particles (position, fade, lifetime)
 */
void UpdateParticles();

/**
 * @brief Renders all active particles to screen
 */
void DrawParticles();

/**
 * @brief Creates explosion particles at specified location
 * @param x X coordinate of explosion center
 * @param y Y coordinate of explosion center
 * @param size Asteroid size (affects particle count and behavior)
 */
void CreateExplosionParticles(float x, float y, AsteroidSize size);

/**
 * @brief Creates dramatic ship explosion particles
 * @param x X coordinate of ship center
 * @param y Y coordinate of ship center
 */
void CreateShipExplosionParticles(float x, float y);

/**
 * @brief Creates thrust trail particles behind ship
 * @param x Ship X coordinate
 * @param y Ship Y coordinate
 * @param rotation Ship rotation angle in degrees
 */
void CreateThrustParticles(float x, float y, float rotation);

/**
 * @brief Removes expired particles from particle vector
 */
void CleanupParticles();