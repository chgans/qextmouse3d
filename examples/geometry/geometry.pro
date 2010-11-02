TEMPLATE = app
TARGET = geometry
CONFIG += qt \
    warn_on
QT += opengl
SOURCES = geometryview.cpp \
    main.cpp \
    geometry.cpp \
    quadplane.cpp
HEADERS = geometryview.h \
    geometry.h \
    quadplane.h
RESOURCES = geometry.qrc
LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../src/threed/threed_dep.pri)
