#!/bin/bash

set -e

echo "creating deb for '${PROGRAMNAME}'; output '${OUTPUT_FILENAME}'"
mkdir -p ./debian/DEBIAN
mkdir -p ./debian/usr/bin
mkdir -p "./debian/usr/share/doc/${PROGRAMNAME}"
find ./debian -type d | xargs chmod 755

mv "${PROGRAMNAME}/${PROGRAMNAME}" ./debian/usr/bin/
chmod 755 "./debian/usr/bin/${PROGRAMNAME}"

echo "This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; read LICENSE for details.

Copyright (C) 2014—2015  Alexander Lopatin <alopatindev ät gmail dot com>" > "./debian/usr/share/doc/${PROGRAMNAME}/copyright"

# round((size in bytes)/1024)
INSTALLED_SIZE=$(du -s ./debian/usr | awk '{x=$1/1024; i=int(x); if ((x-i)*10 >= 5) {f=1} else {f=0}; print i+f}')
echo "size=${INSTALLED_SIZE}"

echo "Package: ${PROGRAMNAME}
Version: ${VERSION}
Section: contrib
Priority: optional
Architecture: amd64
Depends: libqt5widgets5 (>= 5.0.0), libimobiledevice-utils (>= 1.1.5), usbmuxd (>= 1.0.8), android-tools-adb (>= 4.2.2), libc6 (>= 2.15)
Installed-Size: ${INSTALLED_SIZE}
Maintainer: Alexander Lopatin <alopatindev.spamhere@gmail.com>
Description: Crossplatform Android, iOS and text files log viewer
 This program shows a log from devices that run Android and iOS.
 Also it is a GUI for tailf command-line tool." > debian/DEBIAN/control

fakeroot dpkg-deb --build debian
ls -l
echo mv debian.deb "${OUTPUT_FILENAME}"
mv debian.deb "${OUTPUT_FILENAME}"
ls -l
ls -lR debian

# vim: textwidth=0
