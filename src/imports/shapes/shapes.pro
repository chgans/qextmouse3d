TEMPLATE = lib
TARGET  = qshapesqmlplugin
CONFIG += qt plugin

win32 {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

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
    shapes.cpp \
    cylinder.cpp \
    capsule.cpp \
    line.cpp \
    point.cpp
HEADERS += \
    sphere.h \
    cylinder.h \
    capsule.h \
    line.h \
    point.h

qdeclarativesources.files += \
    qmldir \
    Cube.qml \
    cube.obj \
    Quad.qml \
    quad.obj \
    Teapot.qml \
    teapot.bez

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes

target.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes

INSTALLS += qdeclarativesources target

LIBS += -L../../../lib -L../../../bin
include(../../quick3d/quick3d_dep.pri)

symbian {
    qshapesqmlpluginDeployment.sources = qshapesqmlplugin.dll $${qdeclarativesources.files}
    qshapesqmlpluginDeployment.path = $$QT_IMPORTS_BASE_DIR/Qt3D/Shapes
    DEPLOYMENT += qshapesqmlpluginDeployment
}