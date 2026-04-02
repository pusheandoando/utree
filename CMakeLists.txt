# CMakeLists.txt
cmake_minimum_required(VERSION 3.17)

project(utree
    VERSION 1.0.0
    DESCRIPTION "Universal Tree - directory explorer and file dumper"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(cmake/CompilerOptions.cmake)

add_subdirectory(core)
add_subdirectory(cli)