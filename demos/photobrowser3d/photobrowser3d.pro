TEMPLATE = app
TARGET = photobrowser3d
CONFIG += qt \
    debug \
    warn_on
QT += opengl
SOURCES += main.cpp\
        photobrowser3dview.cpp \
    cube3dnode.cpp \
    skybox.cpp

HEADERS  += photobrowser3dview.h \
    cube3dnode.h \
    skybox.h

RESOURCES += \
    photobrowser3d.qrc

LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
