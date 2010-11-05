TEMPLATE = lib
TARGET  = qstereo2dqmlplugin
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
    floatingitem.cpp \
    redcyaneffect.cpp \
    stereo2d.cpp
HEADERS += \
    floatingitem.h \
    redcyaneffect.h

qdeclarativesources.files += \
    qmldir

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Stereo2D

target.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Stereo2D

INSTALLS += qdeclarativesources target
