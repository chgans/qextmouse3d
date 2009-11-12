TEMPLATE = app
TARGET = graphicsview
CONFIG += qt debug warn_on
QT += opengl
SOURCES = teapotitem.cpp main.cpp
HEADERS = teapotitem.h
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
