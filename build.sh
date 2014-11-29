#!/bin/bash

ctags -R .
cd build-qdevicemonitor-Desktop-Debug/ && make -j16 && ./qdevicemonitor
