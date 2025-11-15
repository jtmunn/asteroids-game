#include <raylib.h>

#include <cmath>

#include "common.h"

/**
 * @file audio.cpp
 * @brief Audio system for Asteroids game using procedural sound generation
 *
 * Generates classic arcade-style sounds procedurally using Raylib's audio system.
 * All sounds are created at runtime for authentic retro game audio experience.
 */

// Sound objects
static Sound shootSound;
static Sound thrustSound;
static Sound smallExplosionSound;
static Sound mediumExplosionSound;
static Sound largeExplosionSound;
static Sound shipExplosionSound;
static Sound gameOverSound;

// Thrust sound management
static bool thrustSoundPlaying = false;

// Helper function to generate a wave with frequency modulation
static Wave GenerateWave(float frequency, float duration, int sampleRate, float fadeOut = 0.1f)
{
    int frameCount = (int)(duration * sampleRate);
    Wave wave = {0};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;

    short* data = (short*)malloc(frameCount * sizeof(short));

    for (int i = 0; i < frameCount; i++)
    {
        float time = (float)i / sampleRate;
        float amplitude = 1.0f;

        // Fade out at the end
        if (time > duration - fadeOut)
        {
            amplitude = (duration - time) / fadeOut;
        }

        float value = amplitude * sinf(2.0f * PI * frequency * time);
        data[i] = (short)(value * 32767);
    }

    wave.data = data;
    return wave;
}

// Generate noise wave for explosion sounds
static Wave GenerateNoiseWave(float duration, int sampleRate, float fadeOut = 0.2f)
{
    int frameCount = (int)(duration * sampleRate);
    Wave wave = {0};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;

    short* data = (short*)malloc(frameCount * sizeof(short));

    for (int i = 0; i < frameCount; i++)
    {
        float time = (float)i / sampleRate;
        float amplitude = 1.0f;

        // Fade out
        if (time > duration - fadeOut)
        {
            amplitude = (duration - time) / fadeOut;
        }

        // Create explosive sound with multiple frequency layers
        float noise = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

        // Deep rumble (40Hz) - the main explosive boom
        float deepRumble = sinf(2.0f * PI * 40.0f * time);

        // Mid rumble (80Hz) - adds body to the explosion
        float midRumble = sinf(2.0f * PI * 80.0f * time) * 0.6f;

        // Higher frequency crackle that fades quickly
        float crackle = noise * expf(-time * 6.0f);

        // Combine for explosive effect: deep boom + mid body + crackling noise
        float value = amplitude * (0.4f * deepRumble + 0.3f * midRumble + 0.3f * crackle);

        data[i] = (short)(value * 32767);
    }

    wave.data = data;
    return wave;
}

// Generate laser-style shooting sound with frequency sweep
static Wave GenerateLaserSound(float duration, int sampleRate)
{
    int frameCount = (int)(duration * sampleRate);
    Wave wave = {0};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;

    short* data = (short*)malloc(frameCount * sizeof(short));

    for (int i = 0; i < frameCount; i++)
    {
        float time = (float)i / sampleRate;
        float progress = time / duration;

        // Much more subtle frequency sweep
        float startFreq = 600.0f;  // Lower starting frequency
        float endFreq = 400.0f;    // Smaller frequency range
        float frequency = startFreq + (endFreq - startFreq) * progress;

        // Very quick, subtle envelope
        float amplitude = expf(-progress * 12.0f) * 0.4f;  // Quieter and faster decay

        // Simple sine wave, no harmonics to reduce harshness
        float value = amplitude * sinf(2.0f * PI * frequency * time);
        data[i] = (short)(value * 32767);
    }

    wave.data = data;
    return wave;
}

// Generate dramatic game over sound - descending chord sequence
static Wave GenerateGameOverSound(float duration, int sampleRate)
{
    int frameCount = (int)(duration * sampleRate);
    Wave wave = {0};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;

    short* data = (short*)malloc(frameCount * sizeof(short));

    for (int i = 0; i < frameCount; i++)
    {
        float time = (float)i / sampleRate;
        float progress = time / duration;

        // Create a somber descending sequence
        float amplitude = 1.0f - (progress * 0.3f);  // Gradual fade
        if (progress > 0.8f)
            amplitude *= (1.0f - progress) / 0.2f;  // Final fade out

        // Descending chord progression - classic game over feel
        float freq1 = 220.0f * expf(-progress * 0.8f);  // Descending fundamental
        float freq2 = 165.0f * expf(-progress * 0.6f);  // Lower harmony
        float freq3 = 110.0f * expf(-progress * 0.4f);  // Deep bass note

        float tone1 = sinf(2.0f * PI * freq1 * time) * 0.4f;
        float tone2 = sinf(2.0f * PI * freq2 * time) * 0.3f;
        float tone3 = sinf(2.0f * PI * freq3 * time) * 0.3f;

        float value = amplitude * (tone1 + tone2 + tone3);
        data[i] = (short)(value * 32767);
    }

    wave.data = data;
    return wave;
}

/**
 * @brief Initializes Raylib audio system and generates procedural sounds
 *
 * Creates all required sound effects using procedural generation.
 * Handles audio device initialization and sound volume configuration.
 * Safe to call even if audio device is not available.
 */
void InitializeAudio()
{
    InitAudioDevice();

    if (!IsAudioDeviceReady())
    {
        return;  // Audio not available, continue without sound
    }

    // Generate shoot sound - subtle blip
    Wave shootWave = GenerateLaserSound(0.08f, 44100);  // Much shorter
    shootSound = LoadSoundFromWave(shootWave);
    UnloadWave(shootWave);
    SetSoundVolume(shootSound, 0.3f);  // Much quieter

    // Generate thrust sound - very subtle low rumble
    Wave thrustWave = GenerateWave(80.0f, 0.3f, 44100, 0.1f);  // Lower pitch, shorter
    thrustSound = LoadSoundFromWave(thrustWave);
    UnloadWave(thrustWave);
    SetSoundVolume(thrustSound, 0.15f);  // Much quieter

    // Generate explosion sounds - different sizes
    Wave smallExplosionWave = GenerateNoiseWave(0.3f, 44100);
    smallExplosionSound = LoadSoundFromWave(smallExplosionWave);
    UnloadWave(smallExplosionWave);
    SetSoundVolume(smallExplosionSound, 0.4f);

    Wave mediumExplosionWave = GenerateNoiseWave(0.5f, 44100);
    mediumExplosionSound = LoadSoundFromWave(mediumExplosionWave);
    UnloadWave(mediumExplosionWave);
    SetSoundVolume(mediumExplosionSound, 0.5f);

    Wave largeExplosionWave = GenerateNoiseWave(0.8f, 44100);
    largeExplosionSound = LoadSoundFromWave(largeExplosionWave);
    UnloadWave(largeExplosionWave);
    SetSoundVolume(largeExplosionSound, 0.6f);

    // Ship explosion - longer, more dramatic
    Wave shipExplosionWave = GenerateNoiseWave(1.2f, 44100, 0.8f);
    shipExplosionSound = LoadSoundFromWave(shipExplosionWave);
    UnloadWave(shipExplosionWave);
    SetSoundVolume(shipExplosionSound, 0.7f);

    // Game over sound - dramatic descending chord
    Wave gameOverWave = GenerateGameOverSound(3.0f, 44100);
    gameOverSound = LoadSoundFromWave(gameOverWave);
    UnloadWave(gameOverWave);
    SetSoundVolume(gameOverSound, 0.8f);

    // Set master volume
    SetMasterVolume(0.6f);
}

/**
 * @brief Cleans up audio resources and closes audio system
 *
 * Unloads all sound objects and properly closes the audio device.
 * Safe to call even if audio device was not initialized.
 */
void CleanupAudio()
{
    if (!IsAudioDeviceReady())
        return;

    UnloadSound(shootSound);
    UnloadSound(thrustSound);
    UnloadSound(smallExplosionSound);
    UnloadSound(mediumExplosionSound);
    UnloadSound(largeExplosionSound);
    UnloadSound(shipExplosionSound);
    UnloadSound(gameOverSound);

    CloseAudioDevice();
}

/**
 * @brief Plays subtle laser shooting sound effect
 *
 * Plays a short, quiet laser sound when bullets are fired.
 * Only plays if audio device is ready.
 */
void PlayShootSound()
{
    if (IsAudioDeviceReady())
    {
        PlaySound(shootSound);
    }
}

/**
 * @brief Plays quiet thrust engine sound effect
 *
 * Plays engine sound when ship is thrusting.
 * Only plays if audio device is ready.
 */
void PlayThrustSound()
{
    if (IsAudioDeviceReady() && !thrustSoundPlaying)
    {
        PlaySound(thrustSound);
        thrustSoundPlaying = true;
    }
}

/**
 * @brief Plays asteroid explosion sound based on size
 * @param size Asteroid size (affects explosion sound intensity)
 *
 * Plays different explosion sounds for small, medium, and large asteroids.
 * Larger asteroids produce deeper, more dramatic explosion sounds.
 */
void PlayAsteroidExplosionSound(AsteroidSize size)
{
    if (!IsAudioDeviceReady())
        return;

    switch (size)
    {
        case SMALL:
            PlaySound(smallExplosionSound);
            break;
        case MEDIUM:
            PlaySound(mediumExplosionSound);
            break;
        case LARGE:
            PlaySound(largeExplosionSound);
            break;
    }
}

/**
 * @brief Plays dramatic ship destruction sound effect
 *
 * Plays a powerful explosion sound when the player's ship is destroyed.
 * Only plays if audio device is ready.
 */
void PlayShipExplosionSound()
{
    if (IsAudioDeviceReady())
    {
        PlaySound(shipExplosionSound);
    }
}

/**
 * @brief Plays game over music sequence
 *
 * Plays a descending tone sequence when the game ends.
 * Only plays if audio device is ready.
 */
void PlayGameOverSound()
{
    if (IsAudioDeviceReady())
    {
        PlaySound(gameOverSound);
    }
}

/**
 * @brief Updates thrust sound based on current thrusting state
 * @param isThrusting true if ship is currently thrusting
 *
 * Manages thrust sound playback to prevent overlapping audio.
 * Resets sound state when thrusting stops.
 */
void UpdateThrustSound(bool isThrusting)
{
    if (!IsAudioDeviceReady())
        return;

    if (!isThrusting)
    {
        thrustSoundPlaying = false;
    }
    else if (!IsSoundPlaying(thrustSound))
    {
        thrustSoundPlaying = false;  // Sound finished, ready to play again
    }
}