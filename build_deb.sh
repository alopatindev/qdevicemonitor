#!/bin/bash

set -e

VERSION="0.0-1"
PROGRAMNAME="qdevicemonitor"

mkdir -p ./debian/DEBIAN
mkdir -p ./debian/usr/bin
find ./debian -type d | xargs chmod 755

echo "Package: ${PROGRAMNAME}
Version: ${VERSION}
Section: contrib
Priority: optional
Architecture: amd64
Depends: libqt5widgets5 (>= 5.0.0)
Maintainer: Alexander Lopatin <alexlopatin ät gmail.com>
Description: Crossplatform Android, iOS and text file log viewer
 This programs shows a log from devices that run Android and iOS.
 Also it is a GUI for tailf command-line tool." > debian/DEBIAN/control

fakeroot dpkg-deb --build debian
mv debian.deb "${PROGRAMNAME}-${VERSION}-$(date -u +%Y%m%d-%H-%M-%S).deb"
