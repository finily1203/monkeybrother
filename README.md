# Monkeybrother / Mosscape

**Monkeybrother** is a custom 2D game engine written in C++ (OpenGL), and **Mosscape** is the game built on top of it (the `Sandbox` project). The engine uses an Entity-Component-System (ECS) architecture and includes graphics, physics/collision, audio, cutscenes, serialization and an in-engine ImGui debug/editor layer.

## Features

- **ECS core**: entities, component arrays and systems (`Sandbox/ECS`, `Sandbox/Components`, `Sandbox/SystemECS`)
- **Rendering**: OpenGL (GLEW + GLFW), shader-based 2D rendering, sprite animation, 2D camera, FreeType text rendering
- **Physics & collision**: AABB/OBB colliders, forces, moving platforms
- **Gameplay behaviours**: player, enemies, collectables, exits, filters, pumps, navigation arrow
- **Audio**: FMOD Studio
- **Cutscenes & loading screen**: `Sandbox/CutsceneSystem`
- **Serialization**: JSON scenes/levels via nlohmann/json
- **Debugging**: ImGui-based debug tools, crash logging
- **Layers and messaging**: layer system and message system for decoupled communication

## Repository layout

| Path | Description |
|---|---|
| `Sandbox/` | Main engine + game project (`Sandbox.vcxproj`) |
| `Sandbox/Main/Application.cpp` | Entry point |
| `Sandbox/Engine/` | Engine loop and system registration |
| `Sandbox/ECS/`, `Components/`, `SystemECS/` | ECS framework, components and gameplay systems |
| `Sandbox/Graphics/` | Renderer, shaders, camera, fonts, animation data |
| `Sandbox/AudioSystem/` | FMOD audio wrapper |
| `Sandbox/AssetsManager/` | Asset loading and management |
| `Sandbox/Serialization/` | Level/scene save and load |
| `Sandbox/DebugSystem/`, `ImGui/` | Debug tools |
| `lib/` | Third-party libraries: FMOD, GLEW, GLFW, FreeType, Dear ImGui |
| `monkeybrother/` | Earlier engine project (legacy) |
| `monkeybrother.sln` | Visual Studio solution |

## Requirements

- Windows 10/11
- Visual Studio 2022 (MSVC toolset `v143`) with the *Desktop development with C++* workload
- A GPU with OpenGL support

All third-party dependencies are included in `lib/`; no separate install is needed.

## Build and run

1. Open `monkeybrother.sln` in Visual Studio 2022.
2. Set **Sandbox** as the startup project.
3. Choose a configuration (Debug or Release, x64) and build (`Ctrl+Shift+B`).
4. Run (`F5`). Make sure the working directory is `Sandbox/` so assets, shaders and FMOD DLLs are found.

A prebuilt `Mosscape.exe` is also in `Sandbox/`.

## Tech stack

C++ · OpenGL · GLEW · GLFW · FreeType · FMOD Studio · Dear ImGui · nlohmann/json

## Contributors

Joel-cwy, LZHOwen, Ian Loi, finily1203, lee-jingwen, javierjunliang.chua, yaotingliu and others (see `git log`).

## License

Licensed under the Apache License 2.0. See [LICENSE](LICENSE).
