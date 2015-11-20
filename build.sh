#!/bin/bash

set -e

ln -sf ../../scripts/pre-commit.sh .git/hooks/pre-commit

OSX=0
if [ "$(uname)" = Darwin ]; then
    OSX=1
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
