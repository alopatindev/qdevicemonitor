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
    DeviceFacade.cpp \
    AndroidDevice.cpp \
    IOSDevice.cpp \
    TextFileDevice.cpp \
    BaseDevice.cpp \
    DeviceWidget.cpp \
    SettingsDialog.cpp \
    Utils.cpp

HEADERS  += MainWindow.h \
    DeviceFacade.h \
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

CONFIG += c++11

QMAKE_CXXFLAGS += -Werror -Wfatal-errors -pedantic-errors -pedantic -Wextra -Wall
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_LFLAGS_RELEASE -= -O1

VERSION = $$(VERSION)

DEFINES += VERSION=\\\"$$(VERSION_WITH_BUILD_NUMBER)\\\"

linux-clang {
    QMAKE_CXXFLAGS += -Wno-deprecated-register -Wno-nested-anon-types  # FIXME: Qt bugs
}

win32 {
    RC_FILE = winicon.rc
}

macx {
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_LFLAGS += -stdlib=libc++
    ICON = icons/app_icon.icns
}
