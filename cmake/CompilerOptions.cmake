# cmake/CompilerOptions.cmake
option(UTREE_PORTABLE_BUILD "Target the best portable x86-64 baseline instead of march=native" ON)

include(CheckCXXCompilerFlag)

set(UTREE_PORTABLE_MARCH "x86-64")

if(UTREE_PORTABLE_BUILD AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    foreach(candidate x86-64-v3 x86-64-v2 x86-64)
        check_cxx_compiler_flag("-march=${candidate}" UTREE_SUPPORTS_${candidate})
        if(UTREE_SUPPORTS_${candidate})
            set(UTREE_PORTABLE_MARCH "${candidate}")
            break()
        endif()
    endforeach()
endif()

add_library(utree_options INTERFACE)

target_compile_options(utree_options INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall;-Wextra;-Wpedantic>
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>>:-O3;-flto>
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>,$<BOOL:${UTREE_PORTABLE_BUILD}>>:-march=${UTREE_PORTABLE_MARCH};-mtune=generic>
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>,$<NOT:$<BOOL:${UTREE_PORTABLE_BUILD}>>>:-march=native>
    $<$<CXX_COMPILER_ID:MSVC>:/W4;/MP>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/O2;/GL>
)

target_link_options(utree_options INTERFACE
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>>:-flto>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/LTCG>
)