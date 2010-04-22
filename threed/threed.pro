TEMPLATE = lib
TARGET = Qt3d
CONFIG += dll \
    warn_on
QT += opengl \
    network
win32 { 
    DESTDIR = ../bin
    !static:DEFINES += QT_MAKEDLL
}
else:DESTDIR = ../lib
include(threed.pri)
RESOURCES += shaders.qrc
HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_QT3D_LIB

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

target.path += $$[QT_INSTALL_LIBS]
INSTALLS += target
