TARGET  = qsceneobj
include(../../qpluginbase.pri)

HEADERS += qglobjscene.h \
           qglobjscenehandler.h
SOURCES += main.cpp \
           qglobjscene.cpp \
           qglobjscenehandler.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/sceneformats
target.path += $$[QT_INSTALL_PLUGINS]/sceneformats
INSTALLS += target

LIBS += -L../../../../lib -L../../../../bin
QT += opengl network

include(../../../../src/threed/threed_dep.pri)

symbian {
    qsceneobjDeployment.sources = qsceneobj.dll
    qsceneobjDeployment.path = $${QT_PLUGINS_BASE_DIR}/sceneformats
    DEPLOYMENT += qsceneobjDeployment
}