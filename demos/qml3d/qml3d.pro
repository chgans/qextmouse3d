TEMPLATE = app
TARGET = qml3d
CONFIG += qt \
    debug \
    warn_on
QT += opengl \
    declarative \
    network
SOURCES = main.cpp \
    qml3dview.cpp
HEADERS = qml3dview.h
LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
