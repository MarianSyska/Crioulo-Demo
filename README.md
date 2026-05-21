# Crioulo Demo

Example applications demonstrating the [Crioulo](https://github.com/MarianSyska/Crioulo) 3D rendering library — a modern C++20 OpenGL renderer with Physically Based Rendering.

## Examples

| Example | Description |
|---|---|
| **01 Simple Square** | Renders a single quad with a hardcoded orange color. Minimal setup to verify the rendering pipeline. |
| **02 Model Loading** | Loads a PBR-equipped OBJ model (backpack) with skybox, point lighting, and a free-fly camera. |

Both examples support keyboard-controlled camera movement (WASD/QE for position, arrow keys for look direction).

## Requirements

- **CMake** 4.2+
- **C++20** compiler (MSVC 2022)
- **OpenGL** 4.5+
- Python 3.x

The [Crioulo](https://github.com/MarianSyska/Crioulo) engine, GLFW 3, Assimp, and stb_image are bundled as vendored dependencies.

## Build

```bash
git clone --recursive https://github.com/MarianSyska/Crioulo-Demo.git
cd Crioulo-Demo
cmake -S . -B out/build
cmake --build out/build
```

Open using Visual Studio's "Open a local folder" for CMake integration, or generate project files with a CMake generator of your choice.

## Project Structure

```
├── CMakeLists.txt                  # Root build configuration
├── configuration/                  # Generated config headers (root directory)
├── include/CriouloDemo/
│   ├── common/                     # BaseApplication, ModelLoader
│   ├── examples/                   # Per-example Application headers
│   └── util/                       # File I/O helpers
├── lib/Crioulo/                    # Crioulo engine (git submodule)
├── shaders/examples/               # GLSL shaders per example
├── src/
│   ├── common/                     # Shared implementation
│   └── examples/                   # Per-example main + application code
└── data/models/textures/           # Runtime assets
```

## Attribution

- Backpack model sourced from [learnopengl.com](https://learnopengl.com) (CC BY 4.0).
- Third-party libraries: [GLFW](https://github.com/glfw/glfw) (zlib/libpng), [Assimp](https://github.com/assimp/assimp) (BSD 3-Clause), [stb_image](https://github.com/nothings/stb) (MIT), [Crioulo](https://github.com/MarianSyska/Crioulo) (MIT).
