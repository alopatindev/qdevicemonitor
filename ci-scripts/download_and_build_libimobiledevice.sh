#!/bin/bash

set -e

export THIRD_PARTY_DIR="3rdparty"
export PKG_CONFIG_PATH="$(pwd)/${THIRD_PARTY_DIR}/lib/pkgconfig"

function download_and_unpack {
    PACKNAME=$(echo "$1" | sed 's/-.*//')
    VERSION=$(echo "$1" | sed 's/.*-//')
    TARBALL="${PACKNAME}-${VERSION}.tar.gz"
    wget -c -v --tries=10 --timeout=30 "https://github.com/libimobiledevice/${PACKNAME}/archive/${VERSION}.tar.gz" -O "${TARBALL}"
    tar xzvf "${TARBALL}"

    if [ ${PACKNAME} = 'libimobiledevice' ]; then
        exec 0</dev/null
        patch -d "${1}/src" < lockdown-workaround.patch
    fi
}

function build_and_install {
    cd "$1"
    exec 0</dev/null
    NOCONFIGURE=1 ./autogen.sh
    ./configure --prefix "$(pwd)/../${THIRD_PARTY_DIR}"
    make -j2
    make install
    cd ..
}

for i in libplist-1.12 libusbmuxd-1.0.10 libimobiledevice-1.1.7 ; do
    download_and_unpack "$i"
    build_and_install "$i"
done

cp "/bin/cygwin1.dll" "${THIRD_PARTY_DIR}/bin/"
cp "/bin/cygintl-8.dll" "${THIRD_PARTY_DIR}/bin/"
cp "/bin/cygxml2-2.dll" "${THIRD_PARTY_DIR}/bin/"
cp "/bin/cygiconv-2.dll" "${THIRD_PARTY_DIR}/bin/"
cp "/bin/cygz.dll" "${THIRD_PARTY_DIR}/bin/"
cp "/bin/cyggcc_s-1.dll" "${THIRD_PARTY_DIR}/bin/"
cp "/bin/cygcrypto-1.0.0.dll" "${THIRD_PARTY_DIR}/bin/"
cp "/bin/cygssl-1.0.0.dll" "${THIRD_PARTY_DIR}/bin/"
cp "/bin/tail.exe" "${THIRD_PARTY_DIR}/bin/"

# vim: textwidth=0
