# scripts/linux/build_debian.sh
#!/usr/bin/env bash





set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
VERSION="$(cat "${ROOT_DIR}/VERSION")"
PKG_NAME="utree"
BUILD_DIR="${ROOT_DIR}/build"
DIST_DIR="${ROOT_DIR}/dist"
BASELINE_IMAGE="ubuntu:20.04"
ARCH="$(dpkg --print-architecture 2>/dev/null || uname -m | sed 's/x86_64/amd64/;s/aarch64/arm64/')"
STAGING_DIR="${BUILD_DIR}/.deb_staging/${PKG_NAME}_${VERSION}_${ARCH}"

if ! command -v dpkg-deb &>/dev/null; then
    echo "[!!] dpkg-deb not found. Install it with: sudo apt install dpkg"
    exit 1
fi

if ! command -v dpkg-shlibdeps &>/dev/null; then
    echo "[!!] dpkg-shlibdeps not found. Install it with: sudo apt install dpkg-dev"
    exit 1
fi

rm -rf "${BUILD_DIR}"
mkdir -p "${DIST_DIR}"
mkdir -p "${STAGING_DIR}/usr/bin"
mkdir -p "${STAGING_DIR}/DEBIAN"

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

cp "${BUILD_DIR}/cli/utree" "${STAGING_DIR}/usr/bin/utree"
chmod 755 "${STAGING_DIR}/usr/bin/utree"

cat > "${STAGING_DIR}/DEBIAN/control" <<CONTROL
Package: ${PKG_NAME}
Version: ${VERSION}
Architecture: ${ARCH}
Maintainer: Christian <pusheandoando@github>
Section: utils
Priority: optional
Description: List and inspect files and directories in a specified filesystem location.
CONTROL

SHLIBS_DIR="${BUILD_DIR}/.shlibdeps"
mkdir -p "${SHLIBS_DIR}/debian"
touch "${SHLIBS_DIR}/debian/control"
cd "${SHLIBS_DIR}"
dpkg-shlibdeps --ignore-missing-info -O "${STAGING_DIR}/usr/bin/utree" > shlibdeps.out 2>/dev/null || true
DETECTED_DEPENDS="$(grep -oP '(?<=shlibs:Depends=).*' shlibdeps.out 2>/dev/null || true)"
cd "${ROOT_DIR}"

if [ -n "${DETECTED_DEPENDS}" ]; then
    echo "Depends: ${DETECTED_DEPENDS}" >> "${STAGING_DIR}/DEBIAN/control"
    echo "[OK] detected runtime dependencies: ${DETECTED_DEPENDS}"
else
    echo "[!!] could not auto-detect runtime dependencies, package will have no Depends field"
fi

dpkg-deb --root-owner-group --build "${STAGING_DIR}" "${DIST_DIR}/${PKG_NAME}_${VERSION}.deb"
rm -rf "${BUILD_DIR}"

echo "[OK] package ready  ->  dist/${PKG_NAME}_${VERSION}.deb  (${VERSION})"
echo "[OK] install with:  sudo apt install ./dist/${PKG_NAME}_${VERSION}.deb"