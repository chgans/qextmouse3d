TEMPLATE = app
TARGET = stereophoto
HEADERS = imageviewer.h
SOURCES = imageviewer.cpp \
          main.cpp
CONFIG += qt debug warn_on
QT += opengl
RESOURCES = stereophoto.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
