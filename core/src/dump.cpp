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
    std::ostream& out,
    const fs::path& skip_canonical
) {
    int dir_count  = 0;
    int file_count = 0;

    std::vector<fs::directory_entry> entries;
    entries.reserve(64);

    for (const auto& e : fs::directory_iterator(current)) {
        if (exclude.find(e.path().filename().string()) != exclude.end()) {
            continue;
        }

        if (!skip_canonical.empty()) {
            std::error_code ec;
            const auto canon = fs::canonical(e.path(), ec);

            if (!ec && canon == skip_canonical) {
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
            auto [sub_dirs, sub_files] = walk(base, entry.path(), exclude, out, skip_canonical);
            dir_count  += sub_dirs;
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
    std::ostream& out,
    const fs::path& skip_canonical
) {
    return walk(base, base, exclude, out, skip_canonical);
}
}