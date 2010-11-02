TEMPLATE = app
TARGET = cube
CONFIG += qt warn_on
QT += opengl
SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = cube.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../src/threed/threed_dep.pri)
