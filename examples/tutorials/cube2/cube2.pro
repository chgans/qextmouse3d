TEMPLATE = app
TARGET = cube2
CONFIG += qt debug warn_on
QT += opengl
SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
LIBS += -L../../../lib -L../../../bin

include(../../../threed/threed_dep.pri)
