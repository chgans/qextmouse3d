TEMPLATE = lib
TARGET = Qt3D
CONFIG += dll \
    warn_on
QT += opengl \
    network
DESTDIR = ../lib
win32 {
    CONFIG += debug_and_release debug_and_release_config
    DLLDESTDIR = ../bin
    !static:DEFINES += QT_MAKEDLL

    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }

}
include(threed.pri)
RESOURCES += shaders.qrc
PUBLIC_HEADERS = $$HEADERS
HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_QT3D_LIB

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

target.path += $$[QT_INSTALL_LIBS]
INSTALLS += target

install_headers.path = $$[QT_INSTALL_HEADERS]/Qt3D
install_headers.files = $$PUBLIC_HEADERS
INSTALLS += install_headers
