TARGET  = qstereoscenegraph
include(../../qpluginbase.pri)

SOURCES += main.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/scenegraph
target.path += $$[QT_INSTALL_PLUGINS]/scenegraph
INSTALLS += target

QT += declarative

include(../../../../src/quick3d/quick3d_dep.pri)
