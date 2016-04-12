#-------------------------------------------------
#
# Project created by QtCreator 2014-11-28T22:02:24
#
#-------------------------------------------------

QT += core gui

QT_VERSION = 5
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qdevicemonitor
TEMPLATE = app

RESOURCES = resources.qrc

SOURCES += \
    main.cpp \
    Utils.cpp \
    devices/BaseDevice.cpp \
    devices/DeviceFacade.cpp \
    devices/AndroidDevice.cpp \
    devices/IOSDevice.cpp \
    devices/TextFileDevice.cpp \
    devices/trackers/AndroidDevicesTracker.cpp \
    devices/trackers/IOSDevicesTracker.cpp \
    devices/trackers/usb/BaseUsbTracker.cpp \
    devices/trackers/usb/TimerUsbTracker.cpp \
    ui/MainWindow.cpp \
    ui/DeviceWidget.cpp \
    ui/SettingsDialog.cpp

HEADERS += \
    DataTypes.h \
    StringRingBuffer.h \
    Utils.h \
    ui/MainWindow.h \
    devices/DeviceFacade.h \
    devices/AndroidDevice.h \
    devices/IOSDevice.h \
    devices/TextFileDevice.h \
    devices/BaseDevice.h \
    devices/trackers/BaseDevicesTracker.h \
    devices/trackers/AndroidDevicesTracker.h \
    devices/trackers/IOSDevicesTracker.h \
    devices/trackers/TextFileDevicesTracker.h \
    devices/trackers/usb/BaseUsbTracker.h \
    devices/trackers/usb/TimerUsbTracker.h \
    ui/DeviceWidget.h \
    ui/SettingsDialog.h \
    ui/ThemeColors.h

FORMS += \
    ui/MainWindow.ui \
    ui/DeviceWidget.ui \
    ui/SettingsDialog.ui

CONFIG += c++11 link_pkgconfig

QMAKE_CXXFLAGS += -Werror -Wfatal-errors -pedantic-errors -pedantic -Wextra -Wall
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_LFLAGS_RELEASE -= -O1

VERSION = $$(VERSION)

DEFINES += VERSION=\\\"$$(VERSION_WITH_BUILD_NUMBER)\\\"

linux-clang {
    QMAKE_CXXFLAGS += -Wno-deprecated-register -Wno-nested-anon-types  # FIXME: Qt bugs
    SOURCES += devices/trackers/usb/UdevUsbTracker.cpp
    HEADERS += devices/trackers/usb/UdevUsbTracker.h
    PKGCONFIG += libudev
}

win32 {
    RC_FILE = winicon.rc
}

macx {
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_LFLAGS += -stdlib=libc++
    ICON = icons/app_icon.icns
}
