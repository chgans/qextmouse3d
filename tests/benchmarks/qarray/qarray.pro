TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

SOURCES += tst_qarray.cpp

LIBS += -L../../../lib -L../../../bin

include(../../../src/threed/threed_dep.pri)
