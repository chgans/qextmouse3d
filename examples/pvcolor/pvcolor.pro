TEMPLATE = app
TARGET = pvcolor
CONFIG += qt \
    warn_on
QT += opengl
SOURCES = pvcolor.cpp \
    main.cpp
HEADERS = pvcolor.h
LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../src/threed/threed_dep.pri)
FORMS += 
