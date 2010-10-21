TARGET  = qmouse3dlinuxinput
include(../../qpluginbase.pri)

HEADERS += qmouse3dlinuxinputdevice.h \
           qmouse3dlcdscreen.h \
           qmouse3dhaldevice.h
SOURCES += main.cpp \
           qmouse3dlinuxinputdevice.cpp \
           qmouse3dlcdscreen.cpp \
           qmouse3dhaldevice.cpp
RESOURCES += linuxinput.qrc

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/mouse3d
target.path += $$[QT_INSTALL_PLUGINS]/mouse3d
INSTALLS += target

LIBS += -L../../../lib -L../../../bin
QT += opengl network dbus

DEFINES += QT_HAVE_LIBUSB
LIBS += -lusb

include(../../../threed/threed_dep.pri)
