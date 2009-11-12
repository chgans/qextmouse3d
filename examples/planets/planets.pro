TEMPLATE = app
TARGET = planets
CONFIG += qt debug warn_on
QT += opengl
SOURCES = planetview.cpp main.cpp
HEADERS = planetview.h
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
