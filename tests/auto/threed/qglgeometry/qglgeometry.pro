load(qttest_p4.prf)
TEMPLATE=app
TARGET=qglgeometry
QT += testlib
CONFIG += unittest warn_on

SOURCES += tst_qglgeometry.cpp

LIBS += -L../../../../lib -L../../../../bin

include(../../../../threed/threed_dep.pri)
