TEMPLATE = app
TARGET = cubehouse
CONFIG += qt debug warn_on
QT += opengl
SOURCES = cubeview.cpp main.cpp \
    projectivetextureeffect.cpp
HEADERS = cubeview.h \
    projectivetextureeffect.h
RESOURCES = cube.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
