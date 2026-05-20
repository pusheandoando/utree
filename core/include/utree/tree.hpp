// core/include/utree/tree.hpp
#pragma once

#include <filesystem>
#include <ostream>
#include <set>
#include <string>





namespace utree {
struct TreeResult {
    int dirs;
    int files;
};

TreeResult print_tree(
    const std::filesystem::path& base,
    const std::set<std::string>& exclude,
    const std::set<std::string>& include,
    std::ostream& out
);
}