QDeviceMonitor
==============

[![Build Status](https://api.travis-ci.org/alopatindev/qdevicemonitor.svg?branch=master)](https://travis-ci.org/alopatindev/qdevicemonitor)
[![Build Status](https://ci.appveyor.com/api/projects/status/0uex640qxdalur5n?svg=true)](https://ci.appveyor.com/project/alopatindev/qdevicemonitor)

Crossplatform log viewer for Android, iOS and text files.

Key Features
------------
1. GNU/Linux, Mac OS X and Windows support
2. Logs can be taken from Android, iOS or from text file
3. Filtering support
    1. Plain text, wildcards and regexes
    2. Prefixes (pid:, tid:, tag: or text:)
    3. Verbosity level
    4. Automatic filter history that is used as autocomplete
4. Backing up to text files automatically. Automatic old files removal
5. Log color highlighting (two color schemes)

Installation
------------

### GNU/Linux
#### Ubuntu
1. Add **universe** repository (Software Center — Edit — Software Sources...)
2. sudo add-apt-repository --yes ppa:ubuntu-sdk-team/ppa
3. sudo apt-get update
4. [Download](http://qdevicemonitor.uhostall.com/ubuntu) and install the last deb package

#### Gentoo
[Use ebuild](https://bugs.gentoo.org/show_bug.cgi?id=532898) from Gentoo's Bugzilla.

### Mac OS X
[Coming soon](http://qdevicemonitor.uhostall.com/osx).

### Windows
[Download](https://ci.appveyor.com/project/alopatindev/qdevicemonitor/build/artifacts), unzip and run.

Full builds list is [here](https://ci.appveyor.com/project/alopatindev/qdevicemonitor/history).

### Build from Source Code
Make sure you that have installed [Qt >= 5](http://www.qt.io/download-open-source).
* open **qdevicemonitor/qdevicemonitor.pro** with QtCreator and build the project
* **or** use the following terminal commands:
```
cd qdevicemonitor
qmake
make -j
```

License
-------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; read LICENSE for details.

Copyright (C) 2014  Alexander Lopatin <alopatindev ät gmail dot com>
