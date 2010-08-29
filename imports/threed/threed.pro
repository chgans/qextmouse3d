TEMPLATE = lib
#DESTDIR = Qt/labs/threed
TARGET  = qthreedqmlplugin
CONFIG += qt plugin
QT += declarative opengl network
VERSION = 1.0.0

SOURCES += \
    threed.cpp \
    redcyaneffect.cpp \
    effect.cpp \
    inlinemesh.cpp \
    item3d.cpp \
    mesh.cpp \
    qt3dnamespace.cpp \
    shaderprogram.cpp \
    viewport.cpp
HEADERS += \
    redcyaneffect.h \
    effect.h \
    inlinemesh.h \
    item3d.h \
    mesh.h \
    qt3dnamespace.h \
    shaderprogram.h \
    shaderprogram_p.h \
    viewport.h

qdeclarativesources.files += \
    qmldir \
    DefaultNavigator.qml \
    Cube.qml

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt/labs/threed

target.path += $$[QT_INSTALL_IMPORTS]/Qt/labs/threed

INSTALLS += qdeclarativesources target

LIBS += -L../../lib -L../../bin
include(../../threed/threed_dep.pri)
