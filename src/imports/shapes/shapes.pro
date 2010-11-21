TEMPLATE = lib
TARGET  = qshapesqmlplugin
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

SOURCES += \
    sphere.cpp \
    shapes.cpp
HEADERS += \
    sphere.h

qdeclarativesources.files += \
    qmldir \
    Cube.qml \
    cube.obj \
    Teapot.qml \
    teapot.bez

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes

target.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes

INSTALLS += qdeclarativesources target

LIBS += -L../../lib -L../../bin
include(../../quick3d/quick3d_dep.pri)
