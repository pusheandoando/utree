// core/src/dump.cpp
#include "utree/dump.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <vector>





namespace fs = std::filesystem;
static constexpr std::size_t BINARY_PROBE_BYTES = 8192;
static constexpr std::streamsize IO_CHUNK = 65536;

namespace utree {
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

static bool is_binary(const fs::path& path) {
    std::ifstream f(path, std::ios::binary);

    if (!f) {
        return false;
    }

    char buf[BINARY_PROBE_BYTES];
    const auto n = static_cast<std::size_t>(f.read(buf, static_cast<std::streamsize>(BINARY_PROBE_BYTES)).gcount());

    for (std::size_t i = 0; i < n; ++i) {
        if (buf[i] == '\0') {
            return true;
        }
    }
    return false;
}

static void copy_stream(std::ifstream& src, std::ostream& dst) {
    std::array<char, IO_CHUNK> buf;

    while (src.read(buf.data(), IO_CHUNK) || src.gcount() > 0) {
        dst.write(buf.data(), src.gcount());
    }
}

static TreeResult walk(
    const fs::path& base,
    const fs::path& current,
    const std::set<std::string>& exclude,
    const std::set<std::string>& include,
    std::ostream& out,
    const fs::path& skip_canonical
) {
    int dir_count = 0;
    int file_count = 0;

    std::vector<fs::directory_entry> entries;
    entries.reserve(64);

    for (const auto& e : fs::directory_iterator(current)) {
        const std::string name = e.path().filename().string();

        if (!skip_canonical.empty() && e.is_regular_file()) {
            std::error_code ec;
            
            const auto canon = fs::canonical(e.path(), ec);
            if (!ec && canon == skip_canonical) {
                continue;
            }
        }

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
        auto lower = [](std::string s) -> std::string {
            std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            return s;
        };
        return lower(a.path().filename().string()) < lower(b.path().filename().string());
    });

    for (const auto& entry : entries) {
        if (entry.is_directory()) {
            ++dir_count;
            auto [sub_dirs, sub_files] = walk(base, entry.path(), exclude, include, out, skip_canonical);
            dir_count += sub_dirs;
            file_count += sub_files;
        } else if (entry.is_regular_file()) {
            ++file_count;
            const auto rel = entry.path().lexically_relative(base);

            out << "\n\n\n\n\n";
            out << rel.string() << ":\n";
            out << std::string(20, '-') << '\n';

            if (is_binary(entry.path())) {
                out << "[binary file]\n";
                continue;
            }

            std::ifstream f(entry.path());

            if (f) {
                copy_stream(f, out);
            } else {
                out << "[could not read file]\n";
            }
            out << '\n';
        }
    }
    return {dir_count, file_count};
}

TreeResult dump_files(
    const fs::path& base,
    const std::set<std::string>& exclude,
    const std::set<std::string>& include,
    std::ostream& out,
    const fs::path& skip_canonical
) {
    return walk(base, base, exclude, include, out, skip_canonical);
}
}