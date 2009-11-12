TEMPLATE = app
TARGET = cube4
CONFIG += qt debug warn_on
QT += opengl
SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = cube4.qrc
LIBS += -L../../../lib -L../../../bin

include(../../../threed/threed_dep.pri)
