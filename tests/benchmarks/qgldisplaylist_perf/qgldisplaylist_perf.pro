TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

SOURCES += tst_qgldisplaylist_perf.cpp

LIBS += -L../../../lib -L../../../bin

include(../../../threed/threed_dep.pri)
