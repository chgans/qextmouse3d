TEMPLATE = app
TARGET = pvcolor
CONFIG += qt \
    debug \
    warn_on
QT += opengl
SOURCES = pvcolor.cpp \
    main.cpp
HEADERS = pvcolor.h
LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
FORMS += 
