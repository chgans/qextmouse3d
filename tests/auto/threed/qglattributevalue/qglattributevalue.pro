load(qttest_p4.prf)
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

INCLUDEPATH += ../../../../threed/viewing
VPATH += ../../../../threed/viewing

SOURCES += tst_qglattributevalue.cpp

LIBS += -L../../../../lib -L../../../../bin

include(../../../../src/threed/threed_dep.pri)
