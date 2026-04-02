# build.sh
#!/usr/bin/env bash





set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VERSION="1.0.0"
BUILD_DIR="${SCRIPT_DIR}/build"
DIST_DIR="${SCRIPT_DIR}/dist"


rm -rf "${BUILD_DIR}"
mkdir -p "${DIST_DIR}"


cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${BUILD_DIR}" -j"$(nproc)"


cp "${BUILD_DIR}/cli/utree" "${DIST_DIR}/utree"
chmod 755 "${DIST_DIR}/utree"


rm -rf "${BUILD_DIR}"


echo "[OK] build complete  ->  dist/utree  (${VERSION})"