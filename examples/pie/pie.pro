TEMPLATE = app
TARGET = pie
CONFIG += qt debug warn_on
QT += opengl
SOURCES = pieview.cpp main.cpp
HEADERS = pieview.h
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
