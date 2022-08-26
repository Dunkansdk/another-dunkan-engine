cmake_minimum_required (VERSION 3.8)
project (app C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in
  ${CMAKE_SOURCE_DIR}/build/sfml-download/CMakeLists.txt
)

execute_process(
  COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/sfml-download
)

if(result)
  message(FATAL_ERROR "CMake step for sfml failed: ${result}")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/sfml-download
)

if(result)
  message(FATAL_ERROR "Build step for sfml failed: ${result}")
endif()

add_subdirectory(
  ${CMAKE_SOURCE_DIR}/build/sfml-src
  ${CMAKE_SOURCE_DIR}/build/sfml-build
)

#----------------------------------
set(SFML_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/build/sfml-build/include/)
set(APP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
file(GLOB_RECURSE APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
include_directories(${SFML_INCLUDE_DIR} ${APP_INCLUDE_DIR})
include_directories (/usr/include/freetype2)

# application target
add_executable (app ${APP_SRC_FILES}) 
if(WIN32 OR WIN64)
  target_link_libraries(app sfml-window sfml-system sfml-graphics)
else()
  target_link_libraries(app sfml-window sfml-system sfml-graphics pthread X11)
endif()
source_group("src" FILES ${APP_SRC_FILES})
source_group("include" FILES ${APP_INCLUDE_DIR}/*.h)
