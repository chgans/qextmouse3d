TEMPLATE = app
TARGET = spot
CONFIG += qt \
    debug \
    warn_on
QT += opengl
SOURCES += spotview.cpp \
    main.cpp \
    triplane.cpp \
    spotlight.cpp
HEADERS += spotview.h \
    triplane.h \
    spotlight.h
LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
