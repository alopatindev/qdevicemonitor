set QTDIR=C:\Qt\5.5\mingw492_32
set MINGW=C:\MinGW

set CYG_ROOT=C:/cygwin
set CYG_ROOT_WIN=C:\cygwin
set CYG_MIRROR=http://cygwin.mirror.constant.com
set CYG_CACHE=C:/cygwin/var/cache/setup
set CYG_HOMEDIR=C:\cygwin\tmp
set CYG_INSTALLER=setup-x86.exe

set PATH=%PATH%;%QTDIR%\bin;%MINGW%\bin;C:\Program Files\7-Zip

set REVISION=%APPVEYOR_REPO_COMMIT:~0,8%

set VERSION_WITH_BUILD_NUMBER="%VERSION% build %APPVEYOR_BUILD_NUMBER% rev %REVISION%"

set PROGRAMNAME=qdevicemonitor
set OUTPUT_DATE=%date:~-4,4%%date:~-10,2%%date:~-7,2%
set OUTPUT_DIRECTORY=%PROGRAMNAME%-%OUTPUT_DATE%-%APPVEYOR_BUILD_NUMBER%-%REVISION%
set OUTPUT_FILENAME=%OUTPUT_DIRECTORY%.zip
