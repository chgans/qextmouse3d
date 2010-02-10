load(qttest_p4.prf)
TARGET=qbsptree
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

SOURCES += tst_qbsptree.cpp

LIBS += -L../../../../lib -L../../../../bin

include(../../../../threed/threed_dep.pri)
