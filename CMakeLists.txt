# CMakeLists.txt
cmake_minimum_required(VERSION 3.17)

project(utree
    DESCRIPTION "Universal Tree (utree)"
    LANGUAGES CXX
)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" UTREE_VERSION)
string(STRIP "${UTREE_VERSION}" UTREE_VERSION)

project(utree VERSION "${UTREE_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(cmake/CompilerOptions.cmake)

add_subdirectory(core)
add_subdirectory(cli)