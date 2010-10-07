TARGET  = qmouse3dmagellan
include(../../qpluginbase.pri)

HEADERS += qmouse3dmagellandevice.h
SOURCES += main.cpp \
           qmouse3dmagellandevice.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/mouse3d
target.path += $$[QT_INSTALL_PLUGINS]/mouse3d
INSTALLS += target

LIBS += -L../../../lib -L../../../bin
QT += opengl network

include(../../../threed/threed_dep.pri)
