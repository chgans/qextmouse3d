TEMPLATE = app
TARGET = model_advanced
CONFIG += qt \
    debug \
    warn_on
QT += opengl
SOURCES = modelview.cpp \
    main.cpp
HEADERS = modelview.h
RESOURCES = model.qrc
LIBS += -L../../../lib \
    -L../../../bin
include(../../../threed/threed_dep.pri)
