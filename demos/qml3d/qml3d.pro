TEMPLATE = app
TARGET = qml3d
CONFIG += warn_on
QT += opengl \
    declarative \
    network
SOURCES = main.cpp \
    qml3dview.cpp
HEADERS = qml3dview.h
LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../src/threed/threed_dep.pri)
OTHER_FILES += ../declarative/button.qml
