TEMPLATE = app
TARGET = cube
CONFIG += qt debug warn_on
QT += opengl
SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = cube.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
