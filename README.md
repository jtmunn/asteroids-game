# Asteroids - Classic Arcade Game Recreation

A modern recreation of the classic Asteroids arcade game, built in C++ with Raylib. Features professional-quality audio, particle effects, and polished gameplay while maintaining the authentic retro feel.

## 🚀 Features

### Core Gameplay
- **Classic Asteroids Experience** - Authentic ship physics, asteroid behavior, and wave progression
- **Enhanced Controls** - WASD or Arrow keys for movement, with reverse thrust/braking support
- **Natural Physics** - Realistic momentum and inertia-based ship movement
- **Progressive Difficulty** - Waves increase in asteroid count and complexity
- **Lives System** - 4 lives with visual ship icons in HUD

### Audio & Visual Polish
- **Procedural Audio System** - No external audio files required
  - Shooting sounds, explosion effects, thrust audio, game over music
- **Particle Effects System** - Dynamic visual feedback
  - Asteroid explosions, ship destruction, thrust trails
- **Animated UI** - Starfield backgrounds, enhanced menus, wave transitions
- **Professional HUD** - Clean display with score, lives, and wave information

### Quality of Life
- **Safety Spawn System** - Visual feedback and safe respawn mechanics
- **Enhanced Menu System** - Resume functionality, scaling options, visual highlights
- **Smooth State Management** - Seamless transitions between game states
- **Bug-Free Experience** - Comprehensive edge case handling and testing

## 🎮 Controls

| Action | Keys |
|--------|------|
| **Rotate Left** | A or ← |
| **Rotate Right** | D or → |
| **Thrust Forward** | W or ↑ |
| **Reverse/Brake** | S or ↓ |
| **Shoot** | Space |
| **Menu Navigation** | W/S or ↑/↓ |
| **Select** | Enter or Space |
| **Return to Menu** | ESC (during gameplay) |
| **Exit Game** | ESC (from menu) |

## 🛠️ Technical Details

### Architecture
- **Modular Design** - Clean 8-file C++ architecture
- **Separation of Concerns** - Physics, rendering, audio, and game logic separated
- **Memory Safe** - RAII patterns, automatic cleanup, no memory leaks
- **Performance Optimized** - Smooth 60 FPS with efficient rendering

### File Structure
```
src/
├── main.cpp          # Main game loop and state management
├── physics.cpp       # Ship movement and input handling
├── game_logic.cpp    # Collision detection and game mechanics
├── menu_system.cpp   # Menu system with visual effects
├── rendering.cpp     # HUD, backgrounds, and utilities
├── audio.cpp         # Complete procedural audio system
├── particles.cpp     # Particle effects system
└── common.h          # Shared structures and declarations
```

### Dependencies
- **Raylib 5.0+** - Cross-platform game development library
- **C++14 or later** - Modern C++ features
- **No external assets** - All audio and effects generated procedurally

## 🏗️ Building

### Requirements
- C++ compiler with C++14 support
- Raylib 5.0 or later
- Make (or compatible build system)

### Build Instructions
```bash
# Debug build
make RAYLIB_PATH=/path/to/raylib PROJECT_NAME=asteroids OBJS=src/*.cpp BUILD_MODE=DEBUG

# Release build  
make RAYLIB_PATH=/path/to/raylib PROJECT_NAME=asteroids OBJS=src/*.cpp
```

### Windows (with w64devkit)
```bash
mingw32-make RAYLIB_PATH=C:/raylib/raylib PROJECT_NAME=asteroids OBJS=src/*.cpp
```

## 🎯 Roadmap

### V1.0 (Current)
- [ ] High score persistence system
- [ ] Pause overlay functionality
- [x] All core gameplay features
- [x] Complete audio system
- [x] Particle effects
- [x] Enhanced UI/UX

### V1.1+ (Future)
- Controls help screen
- Additional polish and refinements

## 🏆 Development Philosophy

This project prioritizes:
- **Authentic retro experience** with modern polish
- **Clean, maintainable code** architecture
- **Performance and stability** over feature bloat
- **Professional quality** with attention to detail

## 📝 License

[Add your chosen license here]

## 🤝 Contributing

[Add contribution guidelines if you want community involvement]

---

**Created with ❤️ using C++ and Raylib**

*Experience the classic arcade action with modern enhancements!*
- **ESC**: Close game

---
*Built as a learning project following classic Asteroids mechanics*