TEMPLATE = app
TARGET = cube4
CONFIG += qt warn_on
QT += opengl
SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = cube.qrc
LIBS += -L../../../lib -L../../../bin

include(../../../threed/threed_dep.pri)
