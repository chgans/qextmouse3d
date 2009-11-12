TEMPLATE = app
TARGET = painter
CONFIG += qt debug warn_on
QT += opengl
SOURCES = painterwidget.cpp main.cpp
HEADERS = painterwidget.h
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
