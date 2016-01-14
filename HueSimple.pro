QT += core network
QT -= gui

TARGET = HueSimple
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

win32:LIBS += winmm.lib
win32:DEFINES += __WINDOWS_MM__

SOURCES += main.cpp \
           RtMidi.cpp

#for stk root
INCLUDEPATH += ../../include/ ../../src/include/

