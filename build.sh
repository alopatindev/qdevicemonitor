#!/bin/bash

set -e
OSX=0
if [ "$(uname)" = Darwin ]; then
    OSX=1
    export PATH=~/Qt5.4.0/5.4/clang_64/bin/:${PATH}
fi

ctags -R .
cd qdevicemonitor

if [ $OSX = 1 ]; then
    qmake 'CONFIG += debug'
    time make -j8
    lldb qdevicemonitor.app/Contents/MacOS/qdevicemonitor
else
    #qmake -spec linux-g++ 'CONFIG += debug'
    qmake -spec linux-clang 'CONFIG += debug'
    time make -j16
    gdb ./qdevicemonitor
fi
