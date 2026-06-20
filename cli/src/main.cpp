// cli/src/main.cpp
#include "utree/dump.hpp"
#include "utree/tree.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>





static constexpr const char* kVersion = UTREE_VERSION;

namespace fs = std::filesystem;

static void print_help(const char* prog) {
    std::cout
        << "Universal Tree (utree) " << kVersion << " - Written by Christian (@pusheandoando)\n"
        << "List and inspect files and directories in a specified filesystem location.\n"
        << '\n'
        << "Usage:\n"
        << "  " << prog << " <path> [options]\n"
        << '\n'
        << "Options:\n"
        << "  -e, --exclude <items>   Comma-separated names/patterns to exclude (e.g. node_modules,*.o)\n"
        << "                          Can be specified multiple times\n"
        << "  -i, --include <items>   Comma-separated names/patterns to include exclusively\n"
        << "                          Can be specified multiple times\n"
        << "  --dump                  Print file contents with relative paths and summary\n"
        << "  --output <file>         Write output to a .txt file instead of stdout\n"
        << "  -v, --version           Show version information\n"
        << "  -h, --help              Show this help message\n"
        << '\n';
}

static void append_tokens(const std::string& raw, std::set<std::string>& out) {
    std::istringstream ss(raw);
    std::string token;

    while (std::getline(ss, token, ',')) {
        const std::size_t start = token.find_first_not_of(" \t");
        const std::size_t end = token.find_last_not_of(" \t");

        if (start != std::string::npos) {
            out.insert(token.substr(start, end - start + 1));
        }
    }
}

static std::string ensure_txt(const std::string& path) {
    const std::string suffix = ".txt";
    std::string base = path;

    while (base.size() > suffix.size() && base.compare(base.size() - suffix.size(), suffix.size(), suffix) == 0) {
        base.erase(base.size() - suffix.size());
    }
    return base + suffix;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help(argv[0]);
        return 0;
    }

    std::string path_str;
    std::set<std::string> exclude;
    std::set<std::string> include;
    bool do_dump = false;
    std::string output_path;

    for (int i = 1; i < argc; ++i) {
        const std::string arg(argv[i]);

        if (arg == "-h" || arg == "--help") {
            print_help(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            std::cout << "utree " << kVersion << '\n';
            return 0;
        } else if (arg == "--dump") {
            do_dump = true;
        } else if ((arg == "-e" || arg == "--exclude") && i + 1 < argc) {
            append_tokens(argv[++i], exclude);
        } else if ((arg == "-i" || arg == "--include") && i + 1 < argc) {
            append_tokens(argv[++i], include);
        } else if (arg == "--output" && i + 1 < argc) {
            output_path = ensure_txt(argv[++i]);
        } else if (!arg.empty() && arg[0] != '-') {
            path_str = arg;
        } else {
            std::cerr << "[!!] unknown option: " << arg << '\n';
            return 1;
        }
    }

    if (path_str.empty()) {
        std::cerr << "[!!] path is required\n";
        print_help(argv[0]);
        return 1;
    }

    const fs::path base(path_str);

    if (!fs::exists(base)) {
        std::cerr << "[!!] path does not exist: " << path_str << '\n';
        return 1;
    }

    if (!fs::is_directory(base)) {
        std::cerr << "[!!] not a directory: " << path_str << '\n';
        return 1;
    }

    std::ostream* out = &std::cout;
    std::ofstream file_out;
    fs::path output_canonical;

    if (!output_path.empty()) {
        file_out.open(output_path, std::ios::out | std::ios::trunc);

        if (!file_out.is_open()) {
            std::cerr << "[!!] cannot open output file: " << output_path << '\n';
            return 1;
        }
        out = &file_out;

        std::error_code ec;
        const auto abs = fs::absolute(fs::path(output_path));
        output_canonical = fs::weakly_canonical(abs, ec);
    }

    if (do_dump) {
        const auto [dirs, files] = utree::dump_files(base, exclude, include, *out, output_canonical);
        out->flush();
        *out << '\n' << '\n';
        *out << dirs << " directories, " << files << " files\n";
        out->flush();
    } else {
        const auto [dirs, files] = utree::print_tree(base, exclude, include, *out);
        *out << '\n';
        *out << dirs << " directories, " << files << " files\n";
        out->flush();
    }
    return 0;
}