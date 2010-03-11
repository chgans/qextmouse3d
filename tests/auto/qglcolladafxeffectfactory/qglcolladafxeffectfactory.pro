load(qttest_p4.prf)
TEMPLATE = app
QT += testlib
CONFIG += unittest \
    warn_on
LIBS += -L../../../lib \
    -L../../../bin
include(../../../threed/threed_dep.pri)
SOURCES += tst_qglcolladafxeffectfactory.cpp \
    ../../../demos/shaderwizard/qglcolladafxeffectfactory.cpp \
    ../../../demos/shaderwizard/qglcolladafxeffect.cpp \
    ../../../demos/shaderwizard/qglshaderprogrameffect.cpp
INCLUDEPATH += ../../../demos/shaderwizard/
OTHER_FILES += collada_cube.xml
RESOURCES += tst_qglcolladafxeffectfactory.qrc
