TEMPLATE = app
TARGET = penguin
CONFIG += qt warn_on
QT += opengl network
SOURCES = modelview.cpp \
    main.cpp
HEADERS = modelview.h
RESOURCES = model.qrc
LIBS += -L../../../lib \
    -L../../../bin
include(../../../threed/threed_dep.pri)
