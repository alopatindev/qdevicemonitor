#!/bin/bash

set -e

REVISION=$(git rev-parse HEAD)
VERSION="0.0.1"
PROGRAMNAME="qdevicemonitor"

# round((size in bytes)/1024)
INSTALLED_SIZE=$(du -s ./debian/usr | awk '{x=$1/1024; i=int(x); if ((x-i)*10 >= 5) {f=1} else {f=0}; print i+f}')
echo "size=${INSTALLED_SIZE}"

mkdir -p ./debian/DEBIAN
mkdir -p ./debian/usr/bin
find ./debian -type d | xargs chmod 755
cp -v "${PROGRAMNAME}/${PROGRAMNAME}" ./debian/usr/bin/

echo "Package: ${PROGRAMNAME}
Version: ${VERSION}
Section: contrib
Priority: optional
Architecture: amd64
Depends: libqt5widgets5 (>= 5.0.0)
Installed-Size: ${INSTALLED_SIZE}
Maintainer: Alexander Lopatin <alexlopatin ät gmail.com>
Description: Crossplatform Android, iOS and text file log viewer
 This programs shows a log from devices that run Android and iOS.
 Also it is a GUI for tailf command-line tool." > debian/DEBIAN/control

fakeroot dpkg-deb --build debian
mv debian.deb "${PROGRAMNAME}-${VERSION}-$(date -u +%Y%m%d-%H-%M-%S)-${REVISION:0:8}.deb"

# vim: textwidth=0
