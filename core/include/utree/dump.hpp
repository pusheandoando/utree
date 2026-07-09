// core/include/utree/dump.hpp
#pragma once

#include "utree/tree.hpp"

#include <set>
#include <string>
#include <ostream>
#include <filesystem>





namespace utree {
TreeResult dump_files(
    const std::filesystem::path& base,
    const std::set<std::string>& exclude,
    const std::set<std::string>& include,
    std::ostream& out,
    const std::filesystem::path& skip_canonical = {}
);
}