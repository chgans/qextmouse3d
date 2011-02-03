TARGET = qtga
include(../../qpluginbase.pri)
HEADERS += qtgahandler.h \
    qtgafile.h
SOURCES += main.cpp \
    qtgahandler.cpp \
    qtgafile.cpp
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/imageformats
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target

symbian {
    qtgaDeployment.sources = qtga.dll
    qtgaDeployment.path = $${QT_PLUGINS_BASE_DIR}/imageformats
    DEPLOYMENT += qtgaDeployment
}
