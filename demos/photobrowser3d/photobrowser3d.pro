TEMPLATE = app
TARGET = photobrowser3d
CONFIG += qt \
    debug \
    warn_on
QT += opengl
SOURCES += main.cpp\
        photobrowser3dview.cpp \
    imagedisplay.cpp \
    skybox.cpp \
    imagemanager.cpp \
    imageloader.cpp \
    qframesscene.cpp \
    qfocusadaptor.cpp \
    thumbnailableimage.cpp \
    qatlas.cpp \
    thumbnailnode.cpp \
    thumbnaileffect.cpp \
    filescanner.cpp \
    bytereader.cpp \
    threadpool.cpp

HEADERS  += photobrowser3dview.h \
    imagedisplay.h \
    skybox.h \
    imagemanager.h \
    imageloader.h \
    qframesscene.h \
    qfocusadaptor.h \
    thumbnailableimage.h \
    qatlas.h \
    thumbnailnode.h \
    thumbnaileffect.h \
    filescanner.h \
    bytereader.h \
    threadpool.h

# Uncomment this line to force all file loading is done in the gui thread
# instead of a background thread - useful for debugging.
DEFINES += QT_NO_THREADED_FILE_LOAD

# Uncomment this to use test images instead of scanning and loading.
DEFINES += QT_USE_TEST_IMAGES

RESOURCES += \
    photobrowser3d.qrc

LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)

OTHER_FILES += \
    shaders/replace_texture.fsh \
    shaders/replace_texture.vsh
