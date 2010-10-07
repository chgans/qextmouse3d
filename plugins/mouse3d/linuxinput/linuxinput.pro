TARGET  = qmouse3dlinuxinput
include(../../qpluginbase.pri)

HEADERS += qmouse3dlinuxinputdevice.h
SOURCES += main.cpp \
           qmouse3dlinuxinputdevice.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/mouse3d
target.path += $$[QT_INSTALL_PLUGINS]/mouse3d
INSTALLS += target

LIBS += -L../../../lib -L../../../bin
QT += opengl network

include(../../../threed/threed_dep.pri)
