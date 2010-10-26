TEMPLATE = app
TARGET = sphere
CONFIG += qt \
    warn_on
QT += opengl
SOURCES = sphereview.cpp \
    main.cpp
HEADERS = sphereview.h
RESOURCES = sphere.qrc
LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
