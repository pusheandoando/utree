# scripts/linux/build.sh
#!/usr/bin/env bash





set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
VERSION="$(cat "${ROOT_DIR}/VERSION")"
BUILD_DIR="${ROOT_DIR}/build"
DIST_DIR="${ROOT_DIR}/dist"
BASELINE_IMAGE="ubuntu:20.04"

rm -rf "${BUILD_DIR}"
mkdir -p "${DIST_DIR}"

if command -v docker &>/dev/null; then
    echo "[..] docker found, building inside ${BASELINE_IMAGE} for a portable glibc/libstdc++ baseline"
    docker run --rm \
        -v "${ROOT_DIR}:/src" \
        -w /src \
        -e HOST_UID="$(id -u)" \
        -e HOST_GID="$(id -g)" \
        "${BASELINE_IMAGE}" \
        bash -c "
            set -euo pipefail
            export DEBIAN_FRONTEND=noninteractive
            apt-get update -qq
            apt-get install -y -qq build-essential python3-pip >/dev/null
            pip3 install --quiet --upgrade pip
            pip3 install --quiet cmake
            command -v cmake
            cmake --version
            cmake -S /src -B /src/build -DCMAKE_BUILD_TYPE=Release -DUTREE_PORTABLE_BUILD=ON
            cmake --build /src/build -j\$(nproc)
            chown -R \"\${HOST_UID}:\${HOST_GID}\" /src/build
        "
else
    echo "[!!] docker not found, building with the host toolchain"
    echo "[!!] resulting binary will require a glibc/libstdc++ at least as new as this host"
    cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release -DUTREE_PORTABLE_BUILD=ON
    cmake --build "${BUILD_DIR}" -j"$(nproc)"
fi

cp "${BUILD_DIR}/cli/utree" "${DIST_DIR}/utree"
chmod 755 "${DIST_DIR}/utree"

rm -rf "${BUILD_DIR}"

echo "[OK] build complete  ->  dist/utree  (${VERSION})"