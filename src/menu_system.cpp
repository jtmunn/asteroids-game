#include <raylib.h>

#include "common.h"

// Menu constants (using shared constants from common.h)

// Helper function to check if we have an active game
bool HasActiveGame()
{
    // Active game exists when we have lives remaining (game in progress)
    // This works regardless of current state (menu, playing, etc.)
    return lives > 0;
}

// Get number of menu options based on whether we have an active game
int GetUnifiedMenuOptionsCount()
{
    return HasActiveGame() ? 4 : 3;  // Resume, New Game, Scale, Exit : New Game, Scale, Exit
}

void UpdateUnifiedMenu()
{
    int optionsCount = GetUnifiedMenuOptionsCount();

    // Menu navigation
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
    {
        currentMainMenuOption = (currentMainMenuOption - 1 + optionsCount) % optionsCount;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
    {
        currentMainMenuOption = (currentMainMenuOption + 1) % optionsCount;
    }

    // Menu selection
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        bool hasActiveGame = HasActiveGame();
        int adjustedOption = currentMainMenuOption;

        // If no active game, shift options up (no Resume option)
        if (!hasActiveGame)
        {
            adjustedOption = currentMainMenuOption + 1;  // Map to actual enum values
        }

        switch (adjustedOption)
        {
            case MENU_RESUME:
                if (hasActiveGame)
                {
                    currentState = PLAYING;
                }
                break;

            case MENU_NEW_GAME:
                if (hasActiveGame)
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

            case MENU_SCALE:
                // Cycle through scale options
                currentScale = (currentScale % 4) + 1;
                SetWindowSize(gameWidth * currentScale, gameHeight * currentScale);
                break;

            case MENU_EXIT:
                // Request exit - will be handled by main loop
                exitRequested = true;
                break;
        }
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

void DrawUnifiedMenu()
{
    // Draw decorative background elements
    DrawMenuBackground();

    // Enhanced title with outline effect
    DrawTextCentered("ASTEROIDS", gameHeight / 2 - 119, 48, BLACK, gameWidth);  // Shadow
    DrawTextCentered("ASTEROIDS", gameHeight / 2 - 120, 48, WHITE, gameWidth);

    bool hasActiveGame = HasActiveGame();
    int yStart = gameHeight / 2 - 60;
    int lineHeight = 35;
    int currentLine = 0;

    // Resume Game option (only if active game)
    if (hasActiveGame)
    {
        if (currentMainMenuOption == currentLine)
        {
            DrawRectangle((gameWidth - 200) / 2, yStart + (currentLine * lineHeight) - 5, 200, 30,
                          Fade(YELLOW, 0.2f));
            DrawRectangleLines((gameWidth - 200) / 2, yStart + (currentLine * lineHeight) - 5, 200,
                               30, YELLOW);
        }
        Color resumeColor = (currentMainMenuOption == currentLine) ? YELLOW : WHITE;
        const char* resumePrefix = (currentMainMenuOption == currentLine) ? "> " : "  ";
        DrawTextCentered(TextFormat("%sResume Game", resumePrefix),
                         yStart + (currentLine * lineHeight), 24, resumeColor, gameWidth);
        currentLine++;
    }

    // New Game option
    if (currentMainMenuOption == currentLine)
    {
        DrawRectangle((gameWidth - 200) / 2, yStart + (currentLine * lineHeight) - 5, 200, 30,
                      Fade(YELLOW, 0.2f));
        DrawRectangleLines((gameWidth - 200) / 2, yStart + (currentLine * lineHeight) - 5, 200, 30,
                           YELLOW);
    }
    Color newGameColor = (currentMainMenuOption == currentLine) ? YELLOW : WHITE;
    const char* newGamePrefix = (currentMainMenuOption == currentLine) ? "> " : "  ";
    DrawTextCentered(TextFormat("%sNew Game", newGamePrefix), yStart + (currentLine * lineHeight),
                     24, newGameColor, gameWidth);
    currentLine++;

    // Scale option
    if (currentMainMenuOption == currentLine)
    {
        DrawRectangle((gameWidth - 200) / 2, yStart + (currentLine * lineHeight) - 5, 200, 30,
                      Fade(YELLOW, 0.2f));
        DrawRectangleLines((gameWidth - 200) / 2, yStart + (currentLine * lineHeight) - 5, 200, 30,
                           YELLOW);
    }
    Color scaleColor = (currentMainMenuOption == currentLine) ? YELLOW : WHITE;
    const char* scalePrefix = (currentMainMenuOption == currentLine) ? "> " : "  ";
    DrawTextCentered(TextFormat("%sScale: %dx", scalePrefix, currentScale),
                     yStart + (currentLine * lineHeight), 24, scaleColor, gameWidth);
    currentLine++;

    // Exit option
    if (currentMainMenuOption == currentLine)
    {
        DrawRectangle((gameWidth - 200) / 2, yStart + (currentLine * lineHeight) - 5, 200, 30,
                      Fade(YELLOW, 0.2f));
        DrawRectangleLines((gameWidth - 200) / 2, yStart + (currentLine * lineHeight) - 5, 200, 30,
                           YELLOW);
    }
    Color exitColor = (currentMainMenuOption == currentLine) ? YELLOW : WHITE;
    const char* exitPrefix = (currentMainMenuOption == currentLine) ? "> " : "  ";
    DrawTextCentered(TextFormat("%sExit", exitPrefix), yStart + (currentLine * lineHeight), 24,
                     exitColor, gameWidth);

    // Instructions
    DrawTextCentered("Use W/S or UP/DOWN to navigate", gameHeight / 2 + 80, 16, GRAY, gameWidth);
    DrawTextCentered("Press ENTER to select", gameHeight / 2 + 100, 16, GRAY, gameWidth);

    // Show additional context
    if (hasActiveGame)
    {
        DrawTextCentered(
            TextFormat("Current: Wave %d, Lives %d, Score %d", currentWave, lives, score),
            gameHeight / 2 + 130, 16, DARKGRAY, gameWidth);
    }
}
