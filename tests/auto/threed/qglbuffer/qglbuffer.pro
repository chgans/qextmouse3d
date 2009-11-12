load(qttest_p4.prf)
TARGET=qglbuffer
TEMPLATE=app
QT += testlib opengl
CONFIG += unittest warn_on

SOURCES += tst_qglbuffer.cpp

LIBS += -L../../../../lib -L../../../../bin

include(../../../../threed/threed_dep.pri)
