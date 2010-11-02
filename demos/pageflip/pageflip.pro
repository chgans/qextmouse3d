TEMPLATE = app
TARGET = pageflip
CONFIG += qt warn_on
QT += opengl
SOURCES = pageflip.cpp pageflipmath.cpp
HEADERS = pageflipmath_p.h
RESOURCES = pageflip.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../src/threed/threed_dep.pri)
