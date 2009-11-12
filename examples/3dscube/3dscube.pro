TEMPLATE = app
TARGET = 3dscube
CONFIG += qt \
    debug \
    warn_on
QT += opengl
SOURCES += main.cpp \
    cubeview.cpp \
    mainwindow.cpp
HEADERS += cubeview.h \
    mainwindow.h
LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
RESOURCES += 3dscube.qrc
FORMS += mainwindow.ui
