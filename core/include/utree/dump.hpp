// core/include/utree/dump.hpp
#pragma once

#include "utree/tree.hpp"

#include <filesystem>
#include <ostream>
#include <set>
#include <string>





namespace utree {
TreeResult dump_files(
    const std::filesystem::path& base,
    const std::set<std::string>& exclude,
    std::ostream& out,
    const std::filesystem::path& skip_canonical = {}
);
}