TEMPLATE = app
TARGET = penguin
DESTDIR = ../../../bin
CONFIG += qt warn_on
QT += opengl network
SOURCES = modelview.cpp \
    main.cpp
HEADERS = modelview.h
RESOURCES = model_advanced.qrc
LIBS += -L../../../lib \
    -L../../../bin
include(../../../src/threed/threed_dep.pri)
