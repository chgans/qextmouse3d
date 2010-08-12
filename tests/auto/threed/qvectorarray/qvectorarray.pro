load(qttest_p4.prf)
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

INCLUDEPATH += ../../../shared
SOURCES += tst_qvectorarray.cpp

LIBS += -L../../../../lib -L../../../../bin

include(../../../../threed/threed_dep.pri)
