!isEmpty(QMAKE_INCDIR_3DS):INCLUDEPATH += $$QMAKE_INCDIR_3DS
!isEmpty(QMAKE_LIBDIR_3DS):LIBS += -L$$QMAKE_LIBDIR_3DS
TARGET = qscene3ds
include(../../qpluginbase.pri)
HEADERS += qgl3dsloader.h \
    qgl3dsmesh.h \
    qgl3dsscene.h \
    qgl3dsscenehandler.h \
    qgl3dslist.h
SOURCES += main.cpp \
    qgl3dsloader.cpp \
    qgl3dsmesh.cpp \
    qgl3dsscene.cpp \
    qgl3dsscenehandler.cpp \
    qgl3dslist.cpp
DEFINES += Q_USE_DISPLAYLIST
!isEmpty(QMAKE_LIBS_3DS):LIBS += -l$$QMAKE_LIBS_3DS
else { 
    win32:LIBS += -llib3ds-1_3
    else:LIBS += -l3ds
}
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/sceneformats
target.path += $$[QT_INSTALL_PLUGINS]/sceneformats
INSTALLS += target
LIBS += -L../../../lib \
    -L../../../bin
include(../../../threed/threed_dep.pri)
