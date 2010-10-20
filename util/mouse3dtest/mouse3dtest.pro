TEMPLATE = app

HEADERS += mousedetails.h
FORMS += mousedetails.ui
SOURCES += main.cpp mousedetails.cpp

LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
