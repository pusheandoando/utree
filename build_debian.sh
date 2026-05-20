# build_debian.sh
#!/usr/bin/env bash





set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VERSION="$(cat "${SCRIPT_DIR}/VERSION")"
PKG_NAME="utree"
BUILD_DIR="${SCRIPT_DIR}/build"
DIST_DIR="${SCRIPT_DIR}/dist"
ARCH="$(dpkg --print-architecture 2>/dev/null || uname -m | sed 's/x86_64/amd64/;s/aarch64/arm64/')"
STAGING_DIR="${BUILD_DIR}/.deb_staging/${PKG_NAME}_${VERSION}_${ARCH}"


if ! command -v dpkg-deb &>/dev/null; then
    echo "[!!] dpkg-deb not found. Install it with: sudo apt install dpkg"
    exit 1
fi


rm -rf "${BUILD_DIR}"
mkdir -p "${DIST_DIR}"


cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${BUILD_DIR}" -j"$(nproc)"


mkdir -p "${STAGING_DIR}/usr/bin"
mkdir -p "${STAGING_DIR}/DEBIAN"


cp "${BUILD_DIR}/cli/utree" "${STAGING_DIR}/usr/bin/utree"
chmod 755 "${STAGING_DIR}/usr/bin/utree"


cat > "${STAGING_DIR}/DEBIAN/control" <<CONTROL
Package: ${PKG_NAME}
Version: ${VERSION}
Architecture: ${ARCH}
Maintainer: Christian <pusheandoando@github>
Section: utils
Priority: optional
Description: Universal Tree - directory explorer and file dumper
 Visualizes directory structures as a tree and exports file contents.
CONTROL


dpkg-deb --root-owner-group --build "${STAGING_DIR}" "${DIST_DIR}/${PKG_NAME}_${VERSION}.deb"
rm -rf "${BUILD_DIR}"


echo "[OK] package ready  ->  dist/${PKG_NAME}_${VERSION}.deb  (${VERSION})"
echo "[OK] install with:  sudo apt install ./dist/${PKG_NAME}_${VERSION}.deb"