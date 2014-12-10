#!/bin/bash

ctags -R .
cd qdevicemonitor && qmake 'CONFIG += debug' && (time make -j8) && gdb ./qdevicemonitor
