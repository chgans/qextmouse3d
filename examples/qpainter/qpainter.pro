TEMPLATE = app
TARGET = qpainter
CONFIG += qt warn_on
QT += opengl
SOURCES = painterwidget.cpp main.cpp
HEADERS = painterwidget.h
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../src/threed/threed_dep.pri)
