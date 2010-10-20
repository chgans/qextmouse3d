TEMPLATE = app
TARGET = cubehouse
CONFIG += qt debug warn_on
QT += opengl
SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
!contains(QT_CONFIG, opengles1) {
    SOURCES += projectivetextureeffect.cpp
    HEADERs += projectivetextureeffect.h
}
RESOURCES = cube.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)

OTHER_FILES += \
    shaders/objectlineartexgen.frag \
    shaders/objectlineartexgen.vert
