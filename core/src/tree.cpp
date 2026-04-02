// core/src/tree.cpp
#include "utree/tree.hpp"

#include <algorithm>
#include <vector>





namespace fs = std::filesystem;

namespace utree {
static const char* BRANCH_MID = "\xe2\x94\x9c\xe2\x94\x80\xe2\x94\x80 ";
static const char* BRANCH_LAST = "\xe2\x94\x94\xe2\x94\x80\xe2\x94\x80 ";
static const char* PIPE = "\xe2\x94\x82   ";
static const char* BLANK = "    ";

static TreeResult walk_children(
    const fs::path& dir,
    const std::set<std::string>& exclude,
    const std::string& prefix,
    std::ostream& out
) {
    int dir_count = 0;
    int file_count = 0;

    std::vector<fs::directory_entry> entries;
    entries.reserve(64);

    for (const auto& e : fs::directory_iterator(dir)) {
        if (exclude.find(e.path().filename().string()) == exclude.end()) {
            entries.push_back(e);
        }
    }

    std::sort(entries.begin(), entries.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
        const bool a_dir = a.is_directory();
        const bool b_dir = b.is_directory();

        if (a_dir != b_dir) {
            return a_dir > b_dir;
        }

        auto lower = [](std::string s) -> std::string {
            std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            return s;
        };
        return lower(a.path().filename().string()) < lower(b.path().filename().string());
    });

    for (std::size_t i = 0; i < entries.size(); ++i) {
        const bool is_last = (i == entries.size() - 1);

        out << prefix
            << (is_last ? BRANCH_LAST : BRANCH_MID)
            << entries[i].path().filename().string()
            << '\n';

        if (entries[i].is_directory()) {
            const std::string child_prefix = prefix + (is_last ? BLANK : PIPE);
            auto [sub_dirs, sub_files] = walk_children(entries[i].path(), exclude, child_prefix, out);
            dir_count += 1 + sub_dirs;
            file_count += sub_files;
        } else {
            ++file_count;
        }
    }
    return {dir_count, file_count};
}

TreeResult print_tree(
    const fs::path& base,
    const std::set<std::string>& exclude,
    std::ostream& out
) {
    out << base.filename().string() << '\n';
    return walk_children(base, exclude, "", out);
}
}