TEMPLATE = app
TARGET = builder
CONFIG += qt \
    debug \
    warn_on
QT += opengl
SOURCES = builder.cpp \
    main.cpp
HEADERS = builder.h
LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
FORMS += 
RESOURCES += builder.qrc
