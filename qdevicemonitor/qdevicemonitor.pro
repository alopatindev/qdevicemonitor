#-------------------------------------------------
#
# Project created by QtCreator 2014-11-28T22:02:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qdevicemonitor
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    DeviceAdapter.cpp \
    AndroidDevice.cpp \
    BaseDevice.cpp \
    DeviceWidget.cpp

HEADERS  += MainWindow.h \
    DeviceAdapter.h \
    AndroidDevice.h \
    BaseDevice.h \
    DataTypes.h \
    DeviceWidget.h

FORMS    += MainWindow.ui \
    DeviceWidget.ui

QMAKE_CXXFLAGS += -std=c++11
