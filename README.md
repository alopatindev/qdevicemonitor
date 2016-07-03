QDeviceMonitor
==============

[![Release](https://img.shields.io/github/release/alopatindev/qdevicemonitor.svg)](https://github.com/alopatindev/qdevicemonitor/releases/latest)
[![Build Status](https://api.travis-ci.org/alopatindev/qdevicemonitor.svg?branch=master)](https://travis-ci.org/alopatindev/qdevicemonitor)
[![Build Status](https://ci.appveyor.com/api/projects/status/0uex640qxdalur5n?svg=true)](https://ci.appveyor.com/project/alopatindev/qdevicemonitor/build/artifacts)
[![Language](https://img.shields.io/badge/language-C++11-blue.svg)](https://isocpp.org)
[![Language](https://img.shields.io/badge/language-C11-blue.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))

Crossplatform log viewer for Android, iOS and text files.

Key Features
------------
1. GNU/Linux, Mac OS X and Windows support
2. Logs can be taken from Android, iOS or from text file
3. Filtering support (RegExp)
    1. Automatic filter history that is used as autocomplete
    2. Prefixes **pid:, tid:, tag: or text:** (Android only)
4. Verbosity level (Android only)
5. Backup to text files automatically. Automatic old files removal
6. Color Highlight (two color schemes)
7. Clear Log
8. Add a Mark to Log
9. Open Log in External Text Editor

![Main Window](screenshots/mainwindow-osx.png)

Installation
------------

### GNU/Linux
#### Ubuntu
1. Add **universe** repository (Software Center — Edit — Software Sources...)
2. `sudo add-apt-repository --yes ppa:ubuntu-sdk-team/ppa`
3. `sudo apt-get update`
4. Install the .deb

#### Gentoo
[Use .ebuild](https://bugs.gentoo.org/show_bug.cgi?id=532898) from Gentoo's Bugzilla

### Mac OS X
Open the .dmg and move the app to /Applications

### Windows
1. For Android support
    1. Install Android SDK
    2. Install **Tools**, **Platform-tools** and **USB Driver** with Android SDK manager
    3. Add the path to `platform-tools` directory (from Android SDK) to **Path** environment variable (with Control Panel — System — Advanced — Environment Variables)
2. Unpack the .zip and run `qdevicemonitor.exe`

## Latest (Unstable) Builds

[![Download Ubuntu](https://img.shields.io/badge/ubuntu-download-orange.svg)](https://sourceforge.net/projects/qdevicemonitor/files/ci/ubuntu/)
[![Download OS X](https://img.shields.io/badge/os%20x-download-yellowgreen.svg)](https://sourceforge.net/projects/qdevicemonitor/files/ci/osx/)
[![Download Windows](https://img.shields.io/badge/windows-download-blue.svg)](https://ci.appveyor.com/project/alopatindev/qdevicemonitor/build/artifacts)

### Build from the Source Code
Make sure that you have installed [Qt >= 5.6](http://www.qt.io/download-open-source)
* open `qdevicemonitor/qdevicemonitor.pro` with QtCreator and build the project
* **or** use the following terminal commands:
```
cd qdevicemonitor
qmake
make -j8
```

Troubleshooting
---------------
Check [TROUBLESHOOTING.md](TROUBLESHOOTING.md)

License
-------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; read LICENSE for details.

Copyright (C) 2014—2016  Alexander Lopatin <alopatindev ät gmail dot com>
