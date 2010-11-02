TEMPLATE = app
TARGET = teaservice
CONFIG += qt warn_on
QT += opengl network
SOURCES = teaservice.cpp meshobject.cpp sceneobject.cpp
HEADERS = meshobject.h sceneobject.h
!contains(QT_CONFIG, opengles1) {
    SOURCES += perpixeleffect.cpp
    HEADERS += perpixeleffect.h
}
RESOURCES = teaservice.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../src/threed/threed_dep.pri)
