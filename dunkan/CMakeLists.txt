cmake_minimum_required (VERSION 3.8)
project (app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in
  ${CMAKE_SOURCE_DIR}/build/sfml-download/CMakeLists.txt
)

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.4
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    vulkan_headers
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG        sdk-1.2.182.0
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(vulkan_headers)

FetchContent_Declare(
    vulkan_loader
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader.git
    GIT_TAG        sdk-1.2.182.0
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
set(BUILD_WSI_WAYLAND_SUPPORT OFF CACHE INTERNAL "Build Wayland support")
FetchContent_MakeAvailable(vulkan_loader)

#----------------------------------
set(APP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
file(GLOB_RECURSE APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
include_directories(${APP_INCLUDE_DIR})

# application target
add_executable (app ${APP_SRC_FILES})
if(WIN32 OR WIN64)
    target_link_libraries(app PRIVATE glfw Vulkan-Headers vulkan)
else()
    target_link_libraries(app PRIVATE glfw Vulkan-Headers vulkan)
endif()

source_group("src" FILES ${APP_SRC_FILES})
source_group("include" FILES ${APP_INCLUDE_DIR}/*.h)

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -fsanitize=address,undefined")
endif()
