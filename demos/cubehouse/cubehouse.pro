TEMPLATE = app
TARGET = cubehouse
CONFIG += qt warn_on
QT += opengl
SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
win32:DEFINES+=_CRT_SECURE_NO_WARNINGS
!contains(QT_CONFIG, opengles1) {
    SOURCES += projectivetextureeffect.cpp
    HEADERs += projectivetextureeffect.h
}
RESOURCES = cube.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../src/threed/threed_dep.pri)

OTHER_FILES += \
    shaders/objectlineartexgen.frag \
    shaders/objectlineartexgen.vert
