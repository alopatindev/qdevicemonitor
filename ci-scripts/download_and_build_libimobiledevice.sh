#!/bin/bash

set -e -v -x

export THIRD_PARTY_DIR="3rdparty"
export PKG_CONFIG_PATH="$(pwd)/${THIRD_PARTY_DIR}/lib/pkgconfig"
export DEBUG_LIBIMOBILEDEVICE=0

function download_and_unpack {
    PACKNAME=$(echo "$1" | sed 's/-.*//')
    echo "${PACKNAME}"
    VERSION=$(echo "$1" | sed 's/.*-//')
    echo "${VERSION}"
    TARBALL="${PACKNAME}-${VERSION}.tar.gz"
    echo "${TARBALL}"
    wget -c --verbose --tries=10 --timeout=30 "https://github.com/libimobiledevice/${PACKNAME}/archive/${VERSION}.tar.gz" -O "${TARBALL}"
    #curl -sL "https://github.com/libimobiledevice/${PACKNAME}/archive/${VERSION}.tar.gz" -o "${TARBALL}"
    tar xzvf "${TARBALL}"

    if [ ${PACKNAME} = 'libimobiledevice' ]; then
        exec 0</dev/null
        ls -l
        patch -d "${1}/src" < lockdown-workaround.patch
    fi
}

function build_and_install {
    cd "$1"

    PACKNAME=$(echo "$1" | sed 's/-.*//')
    echo "${PACKNAME}"

    exec 0</dev/null
    NOCONFIGURE=1 ./autogen.sh

    PREFIX_DIR="$(pwd)/../${THIRD_PARTY_DIR}"
    if [ ${PACKNAME} = 'libimobiledevice' ] && [ ${DEBUG_LIBIMOBILEDEVICE} = 1 ]; then
        ./configure --prefix "${PREFIX_DIR}" --enable-debug-code || cat config.log
    else
        ./configure --prefix "${PREFIX_DIR}" || cat config.log
    fi

    make -j2
    make install
    cd ..
}

for i in libplist-1.12 libusbmuxd-1.0.10 libimobiledevice-1.2.0 ; do
    download_and_unpack "$i"
    build_and_install "$i"
done

for i in "cygwin1.dll" "cygintl-8.dll" "cygxml2-2.dll" "cygiconv-2.dll" "cygz.dll" "cyggcc_s-1.dll" "cygcrypto-1.0.0.dll" "cygssl-1.0.0.dll" "cyglzma-5.dll" "tail.exe"; do
    cp "/bin/${i}" "${THIRD_PARTY_DIR}/bin/"
done

# vim: textwidth=0
