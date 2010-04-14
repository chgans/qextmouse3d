TEMPLATE = lib
#DESTDIR = Qt/labs/threed
TARGET  = qthreedqmlplugin
CONFIG += qt plugin
QT += declarative opengl network
VERSION = 1.0.0

SOURCES += \
    threed.cpp \
    redcyaneffect.cpp \
    cube.cpp \
    effect.cpp \
    item3d.cpp \
    mesh.cpp \
    rotation3d.cpp \
    shaderprogram.cpp \
    viewport.cpp
HEADERS += \
    redcyaneffect.h \
    cube.h \
    effect.h \
    item3d.h \
    mesh.h \
    rotation3d.h \
    shaderprogram.h \
    viewport.h

qdeclarativesources.files += \
    qmldir

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt/labs/threed

target.path += $$[QT_INSTALL_IMPORTS]/Qt/labs/threed

INSTALLS += qdeclarativesources target

LIBS += -L../../lib -L../../bin
include(../../threed/threed_dep.pri)
