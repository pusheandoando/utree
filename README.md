# Universal Tree (utree) - v1.0.5
List and inspect files and directories in a specified filesystem location. Written by Christian (@pusheandoando)





## Requirements
### Build dependencies
```bash
sudo apt install build-essential cmake
```
CMake 3.17+, GCC with C++17 support (GCC 8+). No external libraries needed.

## Build
```bash
chmod +x scripts/linux/build.sh
scripts/linux/build.sh
```

## Build a .deb package
```bash
chmod +x scripts/linux/build_debian.sh
scripts/linux/build_debian.sh
```





## Usage
```
utree <path> [options]

Options:
  -e, --exclude <items>   Comma-separated names/patterns to exclude (e.g. node_modules,*.o)
                          Can be specified multiple times
  -i, --include <items>   Comma-separated names/patterns to include exclusively
                          Can be specified multiple times
  --dump                  Print file contents with relative paths and summary
  --output <file>         Write output to a .txt file instead of stdout
  -v, --version           Show version information
  -h, --help              Show this help message
```

### Visualize a directory tree
```bash
utree ~/my-project
```

### Dump all file contents to stdout
```bash
utree ~/my-project --dump
```

### Dump only specific files and folders
```bash
utree ~/my-project --dump -i "src,*.cpp,*.hpp"
```

### Dump everything except build artifacts and dependencies
```bash
utree ~/my-project --dump -e "build,node_modules,*.o,*.a"
```

### Write output to a file
```bash
utree ~/my-project --dump --output snapshot
# writes to snapshot.txt
```

## Patterns
`-i` and `-e` accept exact names and `*`-prefix glob patterns:

| Pattern | Matches |
|---|---|
| `node_modules` | directory or file named exactly `node_modules` |
| `*.py` | all files ending in `.py` |
| `*.android.js` | all files ending in `.android.js` |

Glob patterns apply to files only. Directories are matched by exact name.
When `-i` is active, directories are always traversed so that matching files inside them can be reached.