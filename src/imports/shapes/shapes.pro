# No C++ sources to build, so use the "subdirs" template without
# any SUBDIRS to disable trying to build a C++ app, lib, or plugin.
TEMPLATE = subdirs

qdeclarativesources.files += \
    qmldir \
    Cube.qml \
    cube.obj \
    Teapot.qml \
    teapot.bez

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes

target.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes

#INSTALLS += qdeclarativesources target
INSTALLS += qdeclarativesources

LIBS += -L../../lib -L../../bin
include(../../threed/threed_dep.pri)
