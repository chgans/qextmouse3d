TARGET  = qmouse3dwin32input
include(../../qpluginbase.pri)

HEADERS += qmouse3dwin32inputdevice.h \
		   qmouse3dwin32info.h \
		   qmouse3dwin32handler.h

SOURCES += main.cpp \
		   qmouse3dwin32info.cpp \
		   qmouse3dwin32handler.cpp \		   
           qmouse3dwin32inputdevice.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/mouse3d
target.path += $$[QT_INSTALL_PLUGINS]/mouse3d
INSTALLS += target

LIBS += -L../../../../lib -L../../../../bin
QT += opengl network

include(../../../../src/threed/threed_dep.pri)
