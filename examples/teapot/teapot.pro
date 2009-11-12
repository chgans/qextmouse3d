TEMPLATE = app
TARGET = teapot
CONFIG += qt debug warn_on
QT += opengl
SOURCES = teapotview.cpp main.cpp
HEADERS = teapotview.h
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
