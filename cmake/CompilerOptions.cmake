# cmake/CompilerOptions.cmake
option(UTREE_PORTABLE_BUILD "Target a portable x86-64 baseline instead of march=native" ON)

add_library(utree_options INTERFACE)

target_compile_options(utree_options INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall;-Wextra;-Wpedantic>
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>>:-O3;-flto>
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>,$<BOOL:${UTREE_PORTABLE_BUILD}>>:-march=x86-64-v2;-mtune=generic>
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>,$<NOT:$<BOOL:${UTREE_PORTABLE_BUILD}>>>:-march=native>
    $<$<CXX_COMPILER_ID:MSVC>:/W4;/MP>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/O2;/GL>
)

target_link_options(utree_options INTERFACE
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>>:-flto>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/LTCG>
)