load(qttest_p4.prf)
TARGET=qglabstractsurface
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

SOURCES += tst_qglabstractsurface.cpp

LIBS += -L../../../../lib -L../../../../bin

include(../../../../threed/threed_dep.pri)
