TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

SOURCES += tst_qglbuilder_perf.cpp

LIBS += -L../../../lib -L../../../bin

include(../../../src/threed/threed_dep.pri)
