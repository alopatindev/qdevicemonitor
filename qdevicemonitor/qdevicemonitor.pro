#-------------------------------------------------
#
# Project created by QtCreator 2014-11-28T22:02:24
#
#-------------------------------------------------

QT       += core gui

QT_VERSION = 5
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qdevicemonitor
TEMPLATE = app

RESOURCES = resources.qrc

SOURCES += main.cpp\
        MainWindow.cpp \
    DeviceAdapter.cpp \
    AndroidDevice.cpp \
    IOSDevice.cpp \
    TextFileDevice.cpp \
    BaseDevice.cpp \
    DeviceWidget.cpp \
    SettingsDialog.cpp \
    Utils.cpp

HEADERS  += MainWindow.h \
    DeviceAdapter.h \
    AndroidDevice.h \
    IOSDevice.h \
    TextFileDevice.h \
    BaseDevice.h \
    DataTypes.h \
    DeviceWidget.h \
    SettingsDialog.h \
    Utils.h \
    ThemeColors.h

FORMS    += MainWindow.ui \
    DeviceWidget.ui \
    SettingsDialog.ui

QMAKE_CXXFLAGS += -std=c++0x -Werror

win32:QMAKE_CXXFLAGS += -std=gnu++0x
win32:RC_FILE = winicon.rc
