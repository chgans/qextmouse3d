TEMPLATE = app
TARGET = displaylist
CONFIG += qt \
    debug \
    warn_on
QT += opengl
SOURCES = displaylist.cpp \
    main.cpp
HEADERS = displaylist.h
LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
FORMS += 
RESOURCES += displaylist.qrc
