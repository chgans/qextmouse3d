TEMPLATE = app
TARGET = nesting
CONFIG += qt warn_on
QT += opengl
SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = nesting.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../src/threed/threed_dep.pri)
