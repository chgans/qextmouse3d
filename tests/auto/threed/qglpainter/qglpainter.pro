load(qttest_p4.prf)
TARGET=qglpainter
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

INCLUDEPATH += ../../../shared
VPATH += ../../../shared

SOURCES += tst_qglpainter.cpp \
           qgltestwidget.cpp \
           compareimage.cpp \
           qglsimulator.cpp
HEADERS += qgltestwidget.h \
           compareimage.h \
           qglsimulator.h

LIBS += -L../../../../lib -L../../../../bin

include(../../../../threed/threed_dep.pri)
