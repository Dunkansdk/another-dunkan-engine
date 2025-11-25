cmake_minimum_required (VERSION 3.15)
project (app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
set(CMAKE_OSX_ARCHITECTURES "x86_64")

add_definitions(-DDEBUG_IMGUI)

# Auto-detect Vulkan SDK on Windows if VULKAN_SDK not set
if(WIN32 AND NOT DEFINED ENV{VULKAN_SDK})
    file(GLOB VULKAN_SDK_PATHS "C:/VulkanSDK/*")
    if(VULKAN_SDK_PATHS)
        list(GET VULKAN_SDK_PATHS 0 VULKAN_SDK_PATH)
        message(STATUS "Auto-detected Vulkan SDK: ${VULKAN_SDK_PATH}")
        set(ENV{VULKAN_SDK} "${VULKAN_SDK_PATH}")
        set(Vulkan_DIR "${VULKAN_SDK_PATH}")
    endif()
endif()

# Find Vulkan SDK
find_package(Vulkan)
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan SDK not found!\n"
            "Please install the Vulkan SDK from: https://vulkan.lunarg.com/\n"
            "After installing, make sure VULKAN_SDK environment variable is set.\n"
            "You may need to restart your terminal/IDE after installation.")
endif()
message(STATUS "Found Vulkan: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan Library: ${Vulkan_LIBRARIES}")

# Fetch GLFW
include(FetchContent)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw
  GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Fetch GLM - header-only, no CMake processing
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm
  GIT_TAG 0.9.9.8
  GIT_SHALLOW TRUE
)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
    # Don't add_subdirectory to avoid GLM's CMakeLists issues
    # GLM is header-only, just include the directory
    set(GLM_INCLUDE_DIR "${glm_SOURCE_DIR}")
endif()

# Fetch stb_image for texture loading
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb
  GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# Fetch ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.89.9
)
FetchContent_MakeAvailable(imgui)

message(STATUS "Fetching ImGui...")

set(IMGUI_DIR ${imgui_SOURCE_DIR})

# ImGui sources
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

#----------------------------------
set(APP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
file(GLOB_RECURSE APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(APPEND APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# Add stb_image include directory
include_directories(
    ${APP_INCLUDE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${stb_SOURCE_DIR}
    ${GLM_INCLUDE_DIR}
    ${glfw_SOURCE_DIR}/include
    ${Vulkan_INCLUDE_DIRS}
    /usr/local/bin 
    $ENV{VULKAN_SDK}/Bin
    $ENV{VULKAN_SDK}/Bin
)

# application target
add_executable (app ${APP_SRC_FILES} ${IMGUI_SOURCES}) 

target_link_libraries(app 
    Vulkan::Vulkan
    glfw
)

# Debug: Print source files
message(STATUS "Source files: ${APP_SRC_FILES}")

# Debug: Print variables
message(STATUS "WIN32: ${WIN32}")
message(STATUS "MINGW: ${MINGW}")
message(STATUS "MSVC: ${MSVC}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")

# Ensure console subsystem (avoid WinMain requirement)
set_target_properties(app PROPERTIES WIN32_EXECUTABLE FALSE)

if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,console")
endif()

# Compile shaders to SPIR-V
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/shaders/*.frag"
    "${CMAKE_SOURCE_DIR}/shaders/*.vert"
)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(app Shaders)

source_group("src" FILES ${APP_SRC_FILES})
source_group("include" FILES ${APP_INCLUDE_DIR}/*.h)
source_group("imgui" FILES ${IMGUI_SOURCES})

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  # Disable specific warnings for ImGui and external code
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Wdeprecated-declarations")
endif()

# Post-build: Copy shaders and data to bin directory for standalone execution
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/../shaders
        ${CMAKE_SOURCE_DIR}/bin/shaders
    COMMENT "Copying shaders to bin directory"
)

add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/../data
        ${CMAKE_SOURCE_DIR}/bin/data
    COMMENT "Copying data to bin directory"
)
