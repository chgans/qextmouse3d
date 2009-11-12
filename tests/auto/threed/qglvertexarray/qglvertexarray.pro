load(qttest_p4.prf)
TARGET=qglvertexarray
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

SOURCES += tst_qglvertexarray.cpp

LIBS += -L../../../../lib -L../../../../bin

include(../../../../threed/threed_dep.pri)
