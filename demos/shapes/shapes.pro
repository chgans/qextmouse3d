TEMPLATE = app
TARGET = shapes
CONFIG += qt warn_on
QT += opengl
SOURCES = shapes.cpp
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
