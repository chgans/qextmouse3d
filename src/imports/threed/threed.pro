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
        floatingitem_sg.cpp \
        floatingitemnode_sg.cpp \
        stereoinfo_sg.cpp \
        viewport_sg.cpp \
        viewportfbonode_sg.cpp
    HEADERS += \
        floatingitem_sg.h \
        floatingitemnode_sg.h \
        stereoinfo_sg.h \
        viewport_sg.h \
        viewportfbonode_sg.h
}

qdeclarativesources.files += \
    qmldir

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D

target.path += $$[QT_INSTALL_IMPORTS]/Qt3D

INSTALLS += qdeclarativesources target

LIBS += -L../../../lib -L../../../bin
include(../../quick3d/quick3d_dep.pri)

symbian {
    qthreedqmlpluginDeployment.sources = qthreedqmlplugin.dll $${qdeclarativesources.files}
    qthreedqmlpluginDeployment.path = $$QT_IMPORTS_BASE_DIR/Qt3D
    DEPLOYMENT += qthreedqmlpluginDeployment
}
