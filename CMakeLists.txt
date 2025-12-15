# CMakeList.txt : Upper level configuration file
cmake_minimum_required (VERSION 3.10)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY 
    ${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}/)

project (VULKANCMAKE C CXX)
add_subdirectory ("dunkan")
