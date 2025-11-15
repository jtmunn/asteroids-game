#include <cmath>

#include "common.h"

/**
 * @file rendering.cpp
 * @brief Core rendering utilities and visual effects for Asteroids game
 *
 * Provides utility functions for rotation calculations, text centering,
 * HUD display, and background starfield effects.
 */

/**
 * @brief Rotates a point around the origin by specified degrees
 * @param relativeX X coordinate relative to rotation center
 * @param relativeY Y coordinate relative to rotation center
 * @param rotationDegrees Rotation angle in degrees
 * @return Vector2 containing the rotated coordinates
 *
 * Used for ship movement calculations and sprite rotations.
 */
Vector2 RotatePoint(float relativeX, float relativeY, float rotationDegrees)
{
    float rotationRad = rotationDegrees * PI / 180.0f;
    float rotatedX = relativeX * cos(rotationRad) - relativeY * sin(rotationRad);
    float rotatedY = relativeX * sin(rotationRad) + relativeY * cos(rotationRad);
    return {rotatedX, rotatedY};
}

void DrawTextCentered(const char* text, int y, int fontSize, Color color, int screenWidth)
{
    int textWidth = MeasureText(text, fontSize);
    int x = (screenWidth - textWidth) / 2;
    DrawText(text, x, y, fontSize, color);
}

void DrawEnhancedHUD()
{
    // Clean HUD without background boxes - text stands out on its own
    // Positioned slightly away from border to avoid overlap
    DrawText(TextFormat("Score: %d", score), 15, 15, 20, WHITE);

    // Lives shown as "Lives:" text followed by ship icons
    DrawText("Lives:", 15, 40, 20, WHITE);
    for (int i = 0; i < lives; i++)
    {
        float shipX = 80 + (i * 18);
        float shipY = 47;

        // Draw mini ship icon
        Vector2 tip = RotatePoint(0, -4, 0);
        Vector2 leftWing = RotatePoint(-3, 3, 0);
        Vector2 rightWing = RotatePoint(3, 3, 0);

        DrawTriangle((Vector2){shipX + tip.x, shipY + tip.y},
                     (Vector2){shipX + leftWing.x, shipY + leftWing.y},
                     (Vector2){shipX + rightWing.x, shipY + rightWing.y}, WHITE);
    }

    DrawText(TextFormat("Wave: %d", currentWave), 15, 65, 20, WHITE);
}

void DrawMenuBackground()
{
    // Draw animated starfield background
    static float starTimer = 0.0f;
    starTimer += GetFrameTime();

    // Draw stars
    for (int i = 0; i < 100; i++)
    {
        float x = (i * 123.456f) - (int)(i * 123.456f);  // Pseudo-random based on index
        float y = (i * 234.567f) - (int)(i * 234.567f);
        x *= gameWidth;
        y *= gameHeight;

        // Animate star brightness
        float brightness = 0.3f + 0.4f * sinf(starTimer * 2.0f + i * 0.5f);
        DrawPixel(x, y, Fade(WHITE, brightness));
    }

    // Draw decorative asteroid outlines
    for (int i = 0; i < 8; i++)
    {
        float x = (i * 456.789f) - (int)(i * 456.789f);
        float y = (i * 567.890f) - (int)(i * 567.890f);
        x *= gameWidth;
        y *= gameHeight;

        float size = 20 + (i % 3) * 15;
        float alpha = 0.1f + 0.05f * sinf(starTimer * 0.5f + i);
        DrawCircleLines(x, y, size, Fade(WHITE, alpha));
    }

    // Draw subtle border frame
    DrawRectangleLines(10, 10, gameWidth - 20, gameHeight - 20, Fade(WHITE, 0.3f));
    DrawRectangleLines(12, 12, gameWidth - 24, gameHeight - 24, Fade(WHITE, 0.2f));
}