# Mosscape (Monkeybrother Engine)

**Mosscape** is a 2D physics-puzzle game built on **Monkeybrother**, a custom C++/OpenGL game engine. It was made by **Team MonkeHood** for DigiPen Institute of Technology Singapore (course CSD2401).

You play as a moss ball. You don't move it directly. You **rotate the world** and let physics roll the ball through each level. Along the way you collect items, pass through filters that shrink the ball, avoid enemies, ride the push of pumps, and reach the exit.

## Gameplay

| Element | What it does |
|---|---|
| Player (moss ball) | Rolls under physics. It can grow, and plays an idle animation after a period of no input |
| Collectables | Items to pick up. Each one plays a collection sound effect |
| Filters | One-use objects that shrink the ball. They show whether they can still be used or are clogged |
| Pumps | Push the player. They switch on and off on a timer and have an animation |
| Enemies | Patrol the level. Only the patrol state is implemented (the enum has 3 states) |
| Moving platforms | Supported by the engine, but not used in the shipped levels |
| Exit | Completes the level |
| Navigation arrow | Points the player at the next collectable, then at the exit |

The game also has a main menu, options menu (audio and gameplay settings), pause menu, tutorial page, quit-level menu, level-completed and game-over screens, plus intro and end cutscenes and an animated loading screen.

### Controls

| Input | Action |
|---|---|
| `A` / `D` | Rotate the world left / right. Holding the key accelerates the rotation |
| Mouse (sideways) | Rotate the world, when mouse rotation is on |
| `T` | Toggle between mouse and keyboard rotation |
| `Z` / `X` | Camera zoom in / out |
| `Esc` | Pause (in a level) |
| `F` | Toggle fullscreen |
| `Enter` / `Space` | Advance cutscenes |

**Debug builds only:** `1` toggles the ImGui editor, `2` toggles free camera movement, `M` toggles the FPS display. With the free camera on, `I`/`J`/`K`/`L` pan it.

**Cheat codes** (typed as held-down key combos): `GOD` (god mode), `NEXT` (skip level), `END` (skip to end), `WIN`, `LOSE`.

## Engine architecture

The engine is built around an **Entity-Component-System (ECS)**. `main()` in [Application.cpp](Sandbox/Main/Application.cpp) creates an `Engine`, registers each top-level `GameSystem`, then runs the loop (`update` on every system, then swap buffers). Everything is wrapped in a try/catch that writes to `crash-log.txt`. A per-system timer feeds the debug performance viewer.

**Top-level systems, in update order:** Window → AssetsManager → Audio → ECSCoordinator → Graphics → Debug (ImGui) → Font → Camera → LayerManager → Cutscene → LoadingScreen.

### ECS

- `ECSCoordinator` ties together the `EntityManager`, `ComponentManager` and `SystemManager`.
- Limits: **5000 entities** and **32 component types** (a 32-bit signature bitset per entity).
- Components (all in [Sandbox/Components](Sandbox/Components)): Transform, AABB, OBB, Movement, Physics, ClosestPlatform, Animation, Player, Enemy, Collectable, Pump, Exit, Filter, MovPlatform, Navigation, Behaviour, Background, Button, UI, Font.
- ECS systems run in a fixed order, **logic first, then physics and collision, then rendering**:
  - `LogicSystemECS` dispatches to per-entity behaviours: Player, Platform, Enemy, Collectable, Pump, Filter, Exit, MovPlatform, Navigation and the NavigationArrow.
  - `PhysicsSystemECS` and the collision system use AABB/OBB colliders and a `Force` / `ForceManager`.
  - `GraphicSystemECS` draws sprites and animations.
  - `FontSystemECS` draws text.
- Entities can be cloned, given string IDs, and loaded or saved from JSON.

### Other engine modules

- **Graphics** (`Sandbox/Graphics`): OpenGL via GLEW/GLFW, GLSL shaders, 2D camera (position, zoom, rotation, lock-to-player), sprite animation data, FreeType text.
- **AssetsManager**: loads and caches textures, audio (FMOD), fonts, shaders and prefabs from `assets.json`.
- **AudioSystem**: FMOD channels for BGM, SFX, pump, rotation and cutscene sounds. Sounds are mapped via `audio_mappings.json`.
- **LayerManager**: draw layers with visibility flags. Entities can be moved between layers, and layers are saved to JSON.
- **Message system**: observer/observable messaging (`FallMessage`, `JumpMessage`, `CollisionMessage`).
- **Serialization**: `JSONSerializer` (nlohmann/json) reads and writes entities by ID. It handles dotted key paths and the math types.
- **FilePathManager**: resolves every JSON config path relative to the executable.
- **MathLibrary**: custom `Vector2D`, `Vector3D` and `Matrix3x3`.
- **Debug system**: ImGui editor with Game Viewport, Inspector, Hierarchy, Object Creation, Asset Browser, Audio panel, Console, Grid and a performance viewer. Also handles crash logging and CRT leak detection.

## Repository layout

```
.
├── monkeybrother.sln          Visual Studio solution
├── Sandbox/                   The engine and game (builds Mosscape.exe)
│   ├── Main/                  Entry point (Application.cpp)
│   ├── Engine/                Engine loop and system interface
│   ├── ECS/                   Coordinator, entity / component / system managers
│   ├── Components/            Component structs
│   ├── SystemECS/             Logic, physics, collision, graphics, font + behaviours
│   ├── Graphics/              Renderer, camera, shaders, animation data, fonts
│   ├── AssetsManager/         Asset loading and caching
│   ├── AudioSystem/           FMOD wrapper
│   ├── CutsceneSystem/        Cutscenes and loading screen
│   ├── LayerSystem/           Layer manager
│   ├── MessageSystem/         Observer / observable messaging
│   ├── Serialization/         JSON serialization
│   ├── FilePaths/             Path resolution
│   ├── GLFWFunctions/         Input, window callbacks, cheats, fullscreen
│   ├── WindowSystem/          Window creation
│   ├── DebugSystem/           ImGui editor panels, crash log
│   ├── MathLibrary/           Vector and matrix math
│   ├── ImGui/, Nlohmann/      Vendored Dear ImGui and nlohmann/json
│   └── Mosscape.exe           Prebuilt executable
├── lib/                       FMOD, GLEW, GLFW, FreeType, ImGui libraries
├── monkeybrother/             Small leftover engine DLL project (only a Test stub)
└── bin/, bin-int/             Build output (git-ignored)
```

## Build and run

**Requirements:** Windows 10/11, Visual Studio 2022 (toolset `v143`, Desktop development with C++), and a GPU with OpenGL support. Dependencies are in `lib/`.

1. Open `monkeybrother.sln`.
2. Set **Sandbox** as the startup project and pick Debug or Release (x64).
3. Build. The output is `bin/<Configuration>-x64/Sandbox/Mosscape.exe`.
4. Run it.

**Assets:** the `assets/` folder (textures, audio, fonts, shaders, prefabs and all the JSON level and menu files) is **not in the repository**. The `bin/` folder is git-ignored, and that is where the game looks for it. The game loads everything from `assets/` next to `Mosscape.exe`. Without it, the game won't start properly, so copy it into the output folder (for example `bin/Release-x64/Sandbox/assets/`). That folder needs these subfolders: `audio`, `fonts`, `imgui`, `json`, `prefabs`, `shaders`, `textures`. FMOD DLLs (`fmod*.dll`) must also sit beside the exe.

The window defaults to 1600x900 (set in `windowConfig.json`).

## Tech stack

C++ · OpenGL · GLEW · GLFW · FreeType · FMOD · Dear ImGui · nlohmann/json

## Team

Team MonkeHood: Joel Chu, Lew Zong Han Owen, Ian Loi, Lee Jing Wen, Liu Yaoting, Javier Chua, and other contributors (see `git log`).

## License

Apache License 2.0. See [LICENSE](LICENSE).
