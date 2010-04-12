TEMPLATE = app
TARGET = graphicsview
CONFIG += qt debug warn_on
QT += opengl
SOURCES = \
    cubeitem.cpp \
    main.cpp \
    edge.cpp \
    node.cpp \
    graph.cpp
HEADERS = \
    cubeitem.h \
    edge.h \
    node.h \
    graph.h
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
