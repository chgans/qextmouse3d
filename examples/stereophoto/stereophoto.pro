TEMPLATE = app
TARGET = stereophoto
HEADERS = imageviewer.h \
          imageview.h \
          qstereoimage.h \
          qstereopixmap.h
SOURCES = imageviewer.cpp \
          imageview.cpp \
          qstereoimage.cpp \
          qstereopixmap.cpp \
          main.cpp
CONFIG += qt warn_on
QT += opengl
RESOURCES = stereophoto.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
