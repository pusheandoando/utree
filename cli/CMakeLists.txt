# cli/CMakeLists.txt
add_executable(utree src/main.cpp)

target_link_libraries(utree PRIVATE utree_core utree_options)

install(TARGETS utree DESTINATION bin)