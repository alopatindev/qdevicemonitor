QT += core testlib
TEMPLATE = app
TARGET = tests
INCLUDEPATH += .
CONFIG += c++11 debug
QT_VERSION = 5
QMAKE_CXXFLAGS += -O0

HEADERS += \
    TestStringRingBuffer.h \
    ../StringRingBuffer.h

SOURCES += \
    tests.cpp
