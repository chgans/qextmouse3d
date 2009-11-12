TEMPLATE = app
TARGET = pageflip
CONFIG += qt debug warn_on
QT += opengl
SOURCES = pageflip.cpp pageflipmath.cpp
HEADERS = pageflipmath_p.h
RESOURCES = pageflip.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
