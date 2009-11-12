TEMPLATE = app
TARGET = teaservice
CONFIG += qt debug warn_on
QT += opengl
SOURCES = teaservice.cpp meshobject.cpp sceneobject.cpp
HEADERS = meshobject.h sceneobject.h
!contains(QT_CONFIG, opengles1cl):!contains(QT_CONFIG, opengles1) {
    SOURCES += perpixeleffect.cpp
    HEADERS += perpixeleffect.h
}
RESOURCES = teaservice.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
