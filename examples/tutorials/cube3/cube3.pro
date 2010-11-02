TEMPLATE = app
TARGET = cube3
CONFIG += qt warn_on
QT += opengl
SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
LIBS += -L../../../lib -L../../../bin

include(../../../src/threed/threed_dep.pri)
