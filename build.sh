#!/bin/bash

set -e
unalias -a

OSX=0
if [ "$(uname)" = Darwin ]; then
    OSX=1
fi

source ci-scripts/set_env.sh

ctags -R .
cd qdevicemonitor

if [ $OSX = 1 ]; then
    PKG_CONFIG_PATH="$(brew info libusb | grep '/Cellar/' | awk '{print $1}')/lib/pkgconfig"
    LIBUSB_CFLAGS=$(pkg-config --cflags 'libusb-1.0')
    LIBUSB_LFLAGS=$(pkg-config --libs 'libusb-1.0')

    QTDIR=$(brew info qt55 | grep '/Cellar/' | awk '{print $1}')
    PATH="${QTDIR}/bin:${PATH}"

    qmake 'CONFIG += debug' "QMAKE_CFLAGS += ${LIBUSB_CFLAGS}" "QMAKE_LFLAGS += ${LIBUSB_LFLAGS}"
    time make -j8
    lldb qdevicemonitor.app/Contents/MacOS/qdevicemonitor
else
    #qmake -spec linux-g++ 'CONFIG += debug'
    qmake -spec linux-clang 'CONFIG += debug'
    time make -j16
    gdb -quiet ./qdevicemonitor
fi
