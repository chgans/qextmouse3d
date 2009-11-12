load(qttest_p4.prf)
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on
VPATH += ../../shared

INCLUDEPATH += ../../shared
HEADERS += compareimage.h
SOURCES += tst_compareimage.cpp compareimage.cpp

LIBS += -L../../../lib
