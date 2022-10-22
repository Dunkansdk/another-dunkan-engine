cmake_minimum_required (VERSION 3.8)
project (app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${PROJECT_SOURCE_DIR}/extern CACHE PATH "Missing description." FORCE)

#-----------------------------------------------
# STB_IMAGE
#-----------------------------------------------
FetchContent_Declare(
    stb_image
    GIT_REPOSITORY https://github.com/nothings/stb.git
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/stb_image
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(stb_image)

#-----------------------------------------------
# GLM
#-----------------------------------------------
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
    GIT_PROGRESS TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(glm)

#-----------------------------------------------
# GLFW
#-----------------------------------------------
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

#-----------------------------------------------
# VULKAN
#-----------------------------------------------
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

if (NOT imgui_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.82
  )
  FetchContent_GetProperties(imgui)
  if (NOT imgui_POPULATED) # Have we downloaded imgui yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(imgui)
    #message("imgui_SOURCE_DIR " ${imgui_SOURCE_DIR})
    include_directories(${imgui_SOURCE_DIR})
    include_directories(${imgui_SOURCE_DIR}/backends)

    list(APPEND CPP_HEADERS_FILES ${imgui_SOURCE_DIR}/imgui.cpp)
    list(APPEND CPP_SOURCES_FILES ${imgui_SOURCE_DIR}/imgui_demo.cpp)
    list(APPEND CPP_SOURCES_FILES ${imgui_SOURCE_DIR}/imgui_draw.cpp)
    list(APPEND CPP_SOURCES_FILES ${imgui_SOURCE_DIR}/imgui_tables.cpp)
    list(APPEND CPP_SOURCES_FILES ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
    list(APPEND CPP_SOURCES_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)
    list(APPEND CPP_SOURCES_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)

    list(APPEND CPP_HEADERS_FILES ${imgui_SOURCE_DIR}/imgui.h)
    list(APPEND CPP_HEADERS_FILES ${imgui_SOURCE_DIR}/imconfig.h)
    list(APPEND CPP_HEADERS_FILES ${imgui_SOURCE_DIR}/imgui_internal.h)
    #list(APPEND CPP_HEADERS_FILES ${imgui_SOURCE_DIR}/imstb_rectpack.h)
    #list(APPEND CPP_HEADERS_FILES ${imgui_SOURCE_DIR}/imstb_textedit.h)
    #list(APPEND CPP_HEADERS_FILES ${imgui_SOURCE_DIR}/imstb_truetype.h)
    list(APPEND CPP_HEADERS_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h)
    list(APPEND CPP_HEADERS_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.h)

    list(APPEND CPP_INCLUDE_DIRS ${imgui_SOURCE_DIR})
    list(APPEND CPP_INCLUDE_DIRS ${imgui_SOURCE_DIR}/backends)
  endif()
endif()

#-----------------------------------------------
# Sources
#-----------------------------------------------
set(APP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
file(GLOB_RECURSE APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
include_directories(${APP_INCLUDE_DIR} ${stb_image_SOURCE_DIR})

#-----------------------------------------------
# Application target
#-----------------------------------------------
add_executable (app ${APP_SRC_FILES} ${CPP_SOURCES_FILES} ${CPP_HEADERS_FILES})
if(WIN32 OR WIN64)
    target_link_libraries(app PRIVATE glm glfw Vulkan-Headers vulkan)
else()
    target_link_libraries(app PRIVATE glm glfw Vulkan-Headers vulkan)
endif()

source_group("src" FILES ${APP_SRC_FILES})
source_group("include" FILES ${APP_INCLUDE_DIR}/*.hpp)

#-----------------------------------------------
# Compile flags
#-----------------------------------------------
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -fsanitize=address,undefined")
endif()
