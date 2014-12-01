#!/bin/bash

ctags -R .
cd qdevicemonitor && qmake 'CONFIG += debug' && make -j8 && gdb ./qdevicemonitor
