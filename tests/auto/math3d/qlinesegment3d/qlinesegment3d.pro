load(qttest_p4.prf)
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

LIBS += -L../../../../lib -L../../../../bin

include(../../../../threed/threed_dep.pri)

SOURCES += tst_qlinesegment3d.cpp
