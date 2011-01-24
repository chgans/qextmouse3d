TEMPLATE = lib
TARGET  = qthreedqmlplugin
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
#VERSION = 1.0.0

SOURCES += \
    threed.cpp \
    qt3dnamespace.cpp \
    scale3d.cpp \
    viewport.cpp \
    floatingitem.cpp \
    floatingitemeffect.cpp \
    stereoview.cpp \
    stereovieweffect.cpp \
    qgraphicslookattransform.cpp
HEADERS += \
    qt3dnamespace.h \
    scale3d.h \
    viewport.h \
    floatingitem.h \
    floatingitemeffect.h \
    stereoview.h \
    stereovieweffect.h \
    qgraphicslookattransform.h

!contains(QT_CONFIG, opengles1) {
    SOURCES += shaderprogram.cpp
    HEADERS += shaderprogram.h \
               shaderprogram_p.h
}

scenegraph {
    SOURCES += \
        stereoview_sg.cpp \
        floatingitem_sg.cpp \
        floatingitemnode_sg.cpp \
        viewport_sg.cpp \
        viewportfbonode_sg.cpp
    HEADERS += \
        stereoview_sg.h \
        floatingitem_sg.h \
        floatingitemnode_sg.h \
        viewport_sg.h \
        viewportfbonode_sg.h
    !isEmpty(SCENEGRAPH_PATH) {
        INCLUDEPATH += $$SCENEGRAPH_PATH/src/scenegraph/coreapi
        INCLUDEPATH += $$SCENEGRAPH_PATH/src/scenegraph/convenience
        INCLUDEPATH += $$SCENEGRAPH_PATH/src/adaptationlayers
        INCLUDEPATH += $$SCENEGRAPH_PATH/src/qml
        INCLUDEPATH += $$SCENEGRAPH_PATH/src
        LIBS += -L$$SCENEGRAPH_PATH/lib -lQtSceneGraph
        DEFINES += QT_USE_SCENEGRAPH
        DEFINES += QML_RUNTIME_TESTING
    } else {
        error("SCENEGRAPH_PATH is not set")
    }
}

qdeclarativesources.files += \
    qmldir

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D

target.path += $$[QT_INSTALL_IMPORTS]/Qt3D

INSTALLS += qdeclarativesources target

LIBS += -L../../../lib -L../../../bin
include(../../quick3d/quick3d_dep.pri)
