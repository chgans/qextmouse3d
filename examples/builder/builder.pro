TEMPLATE = app
TARGET = builder
CONFIG += qt \
    warn_on
QT += opengl
SOURCES = builder.cpp \
    main.cpp
HEADERS = builder.h
LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../src/threed/threed_dep.pri)
FORMS += 
RESOURCES += builder.qrc
