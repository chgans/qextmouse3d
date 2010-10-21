#-------------------------------------------------
#
# Project created by QtCreator 2010-10-20T09:34:12
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = tank
TEMPLATE = app

SOURCES += main.cpp\
        tankview.cpp \
    tank.cpp \
    quadplane.cpp

HEADERS  += tankview.h \
    tank.h \
    quadplane.h

LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
