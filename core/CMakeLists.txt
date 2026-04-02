# core/CMakeLists.txt
add_library(utree_core STATIC
    src/tree.cpp
    src/dump.cpp
)

target_include_directories(utree_core PUBLIC include)
target_link_libraries(utree_core PRIVATE utree_options)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(utree_core PUBLIC stdc++fs)
endif()