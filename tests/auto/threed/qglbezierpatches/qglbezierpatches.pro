load(qttest_p4.prf)
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

SOURCES += tst_qglbezierpatches.cpp

LIBS += -L../../../../lib -L../../../../bin

include(../../../../src/threed/threed_dep.pri)
