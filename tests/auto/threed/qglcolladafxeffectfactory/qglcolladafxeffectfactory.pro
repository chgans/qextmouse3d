load(qttest_p4.prf)
TEMPLATE = app
QT += testlib
CONFIG += unittest warn_on

INCLUDEPATH += ../../../../demos/shaderwizard/
VPATH += ../../../../demos/shaderwizard/

SOURCES += \
    tst_qglcolladafxeffectfactory.cpp

LIBS += -L../../../../lib -L../../../../bin

include(../../../../src/threed/threed_dep.pri)

OTHER_FILES += collada_cube.xml
RESOURCES += tst_qglcolladafxeffectfactory.qrc
