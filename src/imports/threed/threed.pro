TEMPLATE = lib
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
#VERSION = 1.0.0

SOURCES += \
    threed.cpp \
    qt3dnamespace.cpp \
    scale3d.cpp \
    viewport.cpp \
    floatingitem.cpp \
    floatingitemeffect.cpp \
    stereoview.cpp \
    stereovieweffect.cpp
HEADERS += \
    qt3dnamespace.h \
    scale3d.h \
    viewport.h \
    floatingitem.h \
    floatingitemeffect.h \
    stereoview.h \
    stereovieweffect.h

!contains(QT_CONFIG, opengles1) {
    SOURCES += shaderprogram.cpp
    HEADERS += shaderprogram.h \
               shaderprogram_p.h
}

qdeclarativesources.files += \
    qmldir

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D

target.path += $$[QT_INSTALL_IMPORTS]/Qt3D

INSTALLS += qdeclarativesources target

LIBS += -L../../lib -L../../bin
include(../../quick3d/quick3d_dep.pri)
