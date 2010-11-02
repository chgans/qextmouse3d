TEMPLATE = app
TARGET = stereo
CONFIG += qt warn_on
QT += opengl
SOURCES = stereoview.cpp main.cpp
HEADERS = stereoview.h
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../src/threed/threed_dep.pri)
