load(qttest_p4.prf)
TARGET=qglvertexdescription
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

SOURCES += tst_qglvertexdescription.cpp

LIBS += -L../../../../lib -L../../../../bin

include(../../../../threed/threed_dep.pri)
