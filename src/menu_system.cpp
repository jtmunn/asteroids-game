#include <raylib.h>

#include "common.h"

/**
 * @file menu_system.cpp
 * @brief Dynamic menu system with context-aware options and visual effects
 *
 * Provides a flexible menu interface using vector-based menu items that
 * adapt to different contexts (main menu, pause menu, etc.) automatically.
 */

// Dynamic menu system globals
std::vector<MenuItem> currentMenuItems;
int selectedMenuIndex = 0;

/**
 * @brief Helper function to check if we have an active game
 * @return true if game is in progress (lives > 0), false otherwise
 */
bool HasActiveGame()
{
    return lives > 0;
}

/**
 * @brief Resets menu state when context changes
 */
void ResetMenuState()
{
    selectedMenuIndex = 0;
}

/**
 * @brief Builds menu items list based on current context
 * @param context Menu context (MENU, PAUSED, etc.)
 */
void BuildMenuItems(GameState context)
{
    currentMenuItems.clear();
    selectedMenuIndex = 0;

    switch (context)
    {
        case MENU:
            // Main menu - show Resume if active game exists
            if (HasActiveGame())
            {
                currentMenuItems.push_back({"Resume Game", ACTION_RESUME, false});
            }
            currentMenuItems.push_back({"New Game", ACTION_NEW_GAME, false});
            currentMenuItems.push_back({"High Scores", ACTION_HIGH_SCORES, false});
            currentMenuItems.push_back(
                {"Scale: " + std::to_string(currentScale) + "x", ACTION_SCALE, true});
            currentMenuItems.push_back({"Exit", ACTION_EXIT, false});
            break;

        case PAUSED:
            // Pause menu - simple options
            currentMenuItems.push_back({"Continue", ACTION_RESUME, false});
            currentMenuItems.push_back(
                {"Main Menu", ACTION_EXIT, false});  // EXIT will go to menu in pause context
            break;

        default:
            // Fallback - basic menu
            currentMenuItems.push_back({"Continue", ACTION_RESUME, false});
            break;
    }
}

/**
 * @brief Handles menu navigation and selection input
 */
void UpdateDynamicMenu()
{
    if (currentMenuItems.empty())
        return;

    int optionsCount = currentMenuItems.size();

    // Menu navigation
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
    {
        selectedMenuIndex = (selectedMenuIndex - 1 + optionsCount) % optionsCount;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
    {
        selectedMenuIndex = (selectedMenuIndex + 1) % optionsCount;
    }

    // Menu selection
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        if (selectedMenuIndex >= 0 && selectedMenuIndex < static_cast<int>(currentMenuItems.size()))
        {
            ExecuteMenuAction(currentMenuItems[selectedMenuIndex].action);
        }
    }
}

/**
 * @brief Executes the selected menu action
 * @param action The menu action to perform
 */
void ExecuteMenuAction(MenuActionType action)
{
    switch (action)
    {
        case ACTION_RESUME:
            if (currentState == PAUSED)
            {
                currentState = PLAYING;
            }
            else if (HasActiveGame())
            {
                currentState = PLAYING;
            }
            break;

        case ACTION_NEW_GAME:
            if (HasActiveGame())
            {
                // Show confirmation dialog
                currentState = NEW_GAME_CONFIRM;
            }
            else
            {
                // No active game, start new game directly
                StartNewGame();
            }
            break;

        case ACTION_HIGH_SCORES:
            currentState = HIGH_SCORES;
            break;

        case ACTION_SCALE:
            // Cycle through scale options
            currentScale = (currentScale % 4) + 1;
            SetWindowSize(gameWidth * currentScale, gameHeight * currentScale);
            // Update the label for dynamic display
            BuildMenuItems(currentState == PAUSED ? PAUSED : MENU);
            break;

        case ACTION_EXIT:
            if (currentState == PAUSED)
            {
                // In pause context, EXIT means go to main menu
                currentState = MENU;
            }
            else
            {
                // In main menu context, EXIT means quit
                exitRequested = true;
            }
            break;
    }
}

void StartNewGame()
{
    currentState = WAVE_TRANSITION;

    // Initialize game state
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

/**
 * @brief Renders the dynamic menu with proper spacing
 */
void DrawDynamicMenu()
{
    if (currentMenuItems.empty())
        return;

    // Draw decorative background elements
    DrawMenuBackground();

    // Enhanced title with outline effect (only for main menu)
    if (currentState == MENU)
    {
        DrawTextCentered("ASTEROIDS", gameHeight / 2 - 119, 48, BLACK, gameWidth);  // Shadow
        DrawTextCentered("ASTEROIDS", gameHeight / 2 - 120, 48, WHITE, gameWidth);
    }

    // Calculate menu positioning
    int menuItemCount = currentMenuItems.size();
    int lineHeight = 35;
    int totalMenuHeight = menuItemCount * lineHeight;
    int yStart = gameHeight / 2 - (totalMenuHeight / 2) + (currentState == MENU ? 20 : 0);

    // Draw menu items
    for (int i = 0; i < menuItemCount; i++)
    {
        bool isSelected = (i == selectedMenuIndex);
        int yPos = yStart + (i * lineHeight);

        // Highlight background for selected item
        if (isSelected)
        {
            DrawRectangle((gameWidth - 250) / 2, yPos - 5, 250, 30, Fade(YELLOW, 0.2f));
            DrawRectangleLines((gameWidth - 250) / 2, yPos - 5, 250, 30, YELLOW);
        }

        // Draw menu item text
        Color textColor = isSelected ? YELLOW : WHITE;
        const char* prefix = isSelected ? "> " : "  ";
        std::string displayText = prefix + currentMenuItems[i].label;

        DrawTextCentered(displayText.c_str(), yPos, 24, textColor, gameWidth);
    }

    // Add footer information (only for main menu)
    if (currentState == MENU)
    {
        // High Score display
        DrawTextCentered(TextFormat("High Score: %d", highScore), gameHeight / 2 + 100, 20, YELLOW,
                         gameWidth);

        // Instructions
        DrawTextCentered("Use W/S or UP/DOWN to navigate", gameHeight / 2 + 125, 16, GRAY,
                         gameWidth);
        DrawTextCentered("Press ENTER to select", gameHeight / 2 + 145, 16, GRAY, gameWidth);

        // Show additional context
        if (HasActiveGame())
        {
            DrawTextCentered(
                TextFormat("Current: Wave %d, Lives %d, Score %d", currentWave, lives, score),
                gameHeight / 2 + 165, 16, DARKGRAY, gameWidth);
        }
    }
}

// Menu initialization tracking
static bool menuInitialized = false;
static GameState lastMenuContext = MENU;
static int lastLivesCount = -1;  // Track lives changes

void UpdateUnifiedMenu()
{
    // Rebuild menu if context changed, not initialized, or lives changed
    if (!menuInitialized || lastMenuContext != MENU || lastLivesCount != lives)
    {
        BuildMenuItems(MENU);
        menuInitialized = true;
        lastMenuContext = MENU;
        lastLivesCount = lives;
    }
    UpdateDynamicMenu();
}

void DrawUnifiedMenu()
{
    // Ensure menu is built before drawing
    if (!menuInitialized || lastMenuContext != MENU || lastLivesCount != lives)
    {
        BuildMenuItems(MENU);
        menuInitialized = true;
        lastMenuContext = MENU;
        lastLivesCount = lives;
    }
    DrawDynamicMenu();
}
