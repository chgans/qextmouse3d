TEMPLATE = app
TARGET = stereo
CONFIG += qt debug warn_on
QT += opengl
SOURCES = stereoview.cpp main.cpp
HEADERS = stereoview.h
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
