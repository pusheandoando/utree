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

static bool glob_match(const std::string& pattern, const std::string& name) {
    if (pattern.empty()) {
        return name.empty();
    }

    if (pattern[0] == '*') {
        const std::string suffix = pattern.substr(1);
        
        if (suffix.size() > name.size()) {
            return false;
        }
        return name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    return pattern == name;
}

static bool matches_any(const std::string& name, const std::set<std::string>& patterns) {
    for (const auto& p : patterns) {
        if (glob_match(p, name)) {
            return true;
        }
    }
    return false;
}

static bool is_glob(const std::string& pattern) {
    return pattern.find('*') != std::string::npos;
}

static bool file_included(
    const std::string& name,
    const std::set<std::string>& exclude,
    const std::set<std::string>& include
) {
    if (matches_any(name, exclude)) {
        return false;
    }
    
    if (include.empty()) {
        return true;
    }
    return matches_any(name, include);
}

static bool dir_included(
    const std::string& name,
    const std::set<std::string>& exclude,
    const std::set<std::string>& include
) {
    if (matches_any(name, exclude)) {
        return false;
    }
    
    if (include.empty()) {
        return true;
    }
    
    for (const auto& p : include) {
        if (!is_glob(p) && p == name) {
            return true;
        }
    }
    return true;
}

static TreeResult walk_children(
    const fs::path& dir,
    const std::set<std::string>& exclude,
    const std::set<std::string>& include,
    const std::string& prefix,
    std::ostream& out
) {
    int dir_count = 0;
    int file_count = 0;

    std::vector<fs::directory_entry> entries;
    entries.reserve(64);

    for (const auto& e : fs::directory_iterator(dir)) {
        const std::string name = e.path().filename().string();

        if (e.is_directory()) {
            if (!dir_included(name, exclude, include)) {
                continue;
            }
        } else if (e.is_regular_file()) {
            if (!file_included(name, exclude, include)) {
                continue;
            }
        }

        entries.push_back(e);
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
            auto [sub_dirs, sub_files] = walk_children(entries[i].path(), exclude, include, child_prefix, out);
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
    const std::set<std::string>& include,
    std::ostream& out
) {
    out << base.filename().string() << '\n';
    return walk_children(base, exclude, include, "", out);
}
}