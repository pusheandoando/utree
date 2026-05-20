# cli/CMakeLists.txt
add_executable(utree src/main.cpp)

target_compile_definitions(utree PRIVATE UTREE_VERSION="${UTREE_VERSION}")

target_link_libraries(utree PRIVATE utree_core utree_options)

install(TARGETS utree DESTINATION bin)