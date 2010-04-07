TEMPLATE = app
TARGET = graphicsview
CONFIG += qt debug warn_on
QT += opengl
SOURCES = \
    cubeitem.cpp \
    teapotitem.cpp \
    main.cpp \
    edge.cpp \
    node.cpp \
    graph.cpp
HEADERS = \
    cubeitem.h \
    teapotitem.h \
    edge.h \
    node.h \
    graph.h
RESOURCES = graphicsview.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
