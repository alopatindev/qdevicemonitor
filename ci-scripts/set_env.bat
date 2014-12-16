set QTDIR=C:\Qt\Qt5.3.2\5.3\mingw482_32
set MINGW=C:\MinGW
set PATH=%PATH%;%QTDIR%\bin;%MINGW%\bin;C:\Program Files\7-Zip
set OUTPUT_DATETIME=%date:~-4,4%%date:~-10,2%%date:~-7,2%-%time:~0,2%-%time:~3,2%-%time:~6,2%
set OUTPUT_DATETIME=%OUTPUT_DATETIME: =0%
set OUTPUT_DIRECTORY=qdevicemonitor-%OUTPUT_DATETIME%-%APPVEYOR_REPO_COMMIT:~0,8%
set OUTPUT_FILENAME=%OUTPUT_DIRECTORY%.zip
