# another-dunkan-engine
A Vulkan-based game engine with PBR rendering, deferred shading, and SSAO support.

## Features
- **Deferred Rendering Pipeline** - G-Buffer based rendering with PBR lighting
- **SSAO (Screen-Space Ambient Occlusion)** - Enhanced depth perception
- **Modular Architecture** - Clean separation of concerns with ApplicationConfig, LightingManager, and DebugUI
- **2.5D Isometric Lighting** - XZ horizontal positioning with Y depth control
- **ImGui Debug Interface** - Real-time configuration and performance monitoring
- **ECS (Entity Component System)** - Flexible game object management

## Prerequisites

### Windows
- **Vulkan SDK** - v1.4.328.1 or later ([Download](https://vulkan.lunarg.com/))
- **CMake** - v3.10 or later
- **Ninja Build** - Recommended (or Visual Studio)
- **MinGW-w64** or **MSVC** - C++20 compiler

### Linux
```bash
sudo apt-get install libgl1-mesa-dev libudev-dev libopenal-dev \
  libvorbis-dev libsndfile1-dev libx11-dev libxcursor-dev \
  libxrandr-dev libxinerama-dev libxi-dev libfreetype6-dev \
  libvulkan-dev vulkan-tools
```

## Build Instructions

### Windows (Ninja)
```powershell
# Clean build
rm -Recurse -Force dunkan/build

# Configure
cd dunkan
cmake -S . -B build -G Ninja

# Build
cmake --build build --config Debug

# Run
./build/app.exe
```

### Windows (Visual Studio)
```powershell
cd dunkan
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Debug
./build/Debug/app.exe
```

### Linux
```bash
cd dunkan
mkdir -p build
cd build
cmake -G "Unix Makefiles" .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
./app
```

## Project Structure
```
another-dunkan-engine/
├── dunkan/                 # Main engine code
│   ├── include/
│   │   ├── app/           # Application components
│   │   │   ├── ApplicationConfig.hpp
│   │   │   ├── LightingManager.hpp
│   │   │   └── DebugUI.hpp
│   │   ├── ecs/           # Entity Component System
│   │   ├── game/          # Game-specific code
│   │   └── vulkan/        # Vulkan rendering system
│   ├── src/               # Implementation files
│   │   ├── app/           # Modular components
│   │   └── main.cpp       # Application entry point
│   └── CMakeLists.txt
├── shaders/               # GLSL shader files
├── data/                  # Game assets
└── README.md
```

## Configuration

### Gamma Correction
Default: `0.8f` - Adjustable via ImGui configuration panel

### SSAO Settings
- **Radius:** Controls ambient occlusion spread
- **Bias:** Reduces self-shadowing artifacts
- **Kernel Size:** Number of samples (performance vs quality)

### Debug Views
- **Normal** - Standard PBR rendering
- **Albedo** - Base color only
- **Normal Map** - Surface normals visualization
- **Depth** - Depth buffer visualization
- **SSAO** - Ambient occlusion channel

## Development

### Adding New Components
1. Create header in `dunkan/include/app/`
2. Implement in `dunkan/src/app/`
3. Include in `main.cpp`
4. Register with CMake (auto-detected via GLOB_RECURSE)

### Modifying Shaders
Shaders are located in `shaders/` and automatically compiled to SPIR-V during build.

## Troubleshooting

### "WinMain not found" (Windows)
The project uses console subsystem. Ensure CMakeLists.txt has:
```cmake
set_target_properties(app PROPERTIES WIN32_EXECUTABLE FALSE)
```

### Missing Vulkan SDK
Set the `VULKAN_SDK` environment variable:
```powershell
# Windows
$env:VULKAN_SDK = "C:\VulkanSDK\1.4.328.1"

# Linux
export VULKAN_SDK=/path/to/vulkan/sdk
```

### Shader Compilation Errors
Ensure `glslangValidator` is in your PATH (included with Vulkan SDK).

## License
[Add your license here]

## Contributing
[Add contribution guidelines here]

