TEMPLATE = app
TARGET = graphicsview
CONFIG += qt debug warn_on
QT += opengl
SOURCES = \
    cubeitem.cpp \
    teapotitem.cpp \
    main.cpp \
    coloritem.cpp \
    robot.cpp
HEADERS = \
    cubeitem.h \
    teapotitem.h \
    coloritem.h \
    robot.h
RESOURCES = graphicsview.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
