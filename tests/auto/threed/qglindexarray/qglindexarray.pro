load(qttest_p4.prf)
TARGET=qglindexarray
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

SOURCES += tst_qglindexarray.cpp

LIBS += -L../../../../lib -L../../../../bin

include(../../../../threed/threed_dep.pri)
