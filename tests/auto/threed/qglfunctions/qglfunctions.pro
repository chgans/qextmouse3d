load(qttest_p4.prf)
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

SOURCES += tst_qglfunctions.cpp

LIBS += -L../../../../lib -L../../../../bin

include(../../../../threed/threed_dep.pri)