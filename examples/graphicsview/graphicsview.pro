TEMPLATE = app
TARGET = graphicsview
CONFIG += qt warn_on
QT += opengl
VPATH += $$PWD/../../src/threed/geometry
SOURCES = \
    cubeitem.cpp \
    modelitem.cpp \
    teapotitem.cpp \
    main.cpp \
    edge.cpp \
    node.cpp \
    graph.cpp \
    scene.cpp \
    view.cpp
HEADERS = \
    cubeitem.h \
    modelitem.h \
    teapotitem.h \
    edge.h \
    node.h \
    graph.h \
    scene.h \
    view.h
RESOURCES = \
    graphicsview.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../src/threed/threed_dep.pri)
