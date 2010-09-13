TEMPLATE = lib
#DESTDIR = Qt/labs/threed
TARGET  = qthreedqmlplugin
CONFIG += qt plugin
symbian {
    CONFIG += epocallowdlldata
    contains(QT_EDITION, OpenSource) {
        TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData UserEnvironment WriteUserData
    } else {
        TARGET.CAPABILITY = All -Tcb
    }
}
QT += declarative opengl network
VERSION = 1.0.0

SOURCES += \
    threed.cpp \
    redcyaneffect.cpp \
    effect.cpp \
    item3d.cpp \
    mesh.cpp \
    qt3dnamespace.cpp \
    viewport.cpp
HEADERS += \
    redcyaneffect.h \
    effect.h \
    item3d.h \
    mesh.h \
    qt3dnamespace.h \
    viewport.h

!contains(QT_CONFIG, opengles1) {
    SOURCES += shaderprogram.cpp
    HEADERS += shaderprogram.h \
               shaderprogram_p.h
}

qdeclarativesources.files += \
    qmldir \
    DefaultNavigator.qml \
    Cube.qml \
    cube.obj

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt/labs/threed

target.path += $$[QT_INSTALL_IMPORTS]/Qt/labs/threed

INSTALLS += qdeclarativesources target

LIBS += -L../../lib -L../../bin
include(../../threed/threed_dep.pri)
