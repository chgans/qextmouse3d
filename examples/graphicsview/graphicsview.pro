TEMPLATE = app
TARGET = graphicsview
CONFIG += qt warn_on
QT += opengl
SOURCES = \
    cubeitem.cpp \
    main.cpp \
    edge.cpp \
    node.cpp \
    graph.cpp \
    scene.cpp
HEADERS = \
    cubeitem.h \
    edge.h \
    node.h \
    graph.h \
    scene.h
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../src/threed/threed_dep.pri)
