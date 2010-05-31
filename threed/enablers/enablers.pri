INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += \
    qglattributevalue.h \
    qglindexbuffer.h \
    qgltexture2d.h \
    qgltexturecube.h \
    qglvertexbuffer.h \
    qarray.h \
    qcustomdataarray.h \
    qstereoimage.h \
    qstereopixmap.h \
    qvectorarray.h \
    qt3dglobal.h
SOURCES += qglattributevalue.cpp \
    qglindexbuffer.cpp \
    qgltexture2d.cpp \
    qgltexturecube.cpp \
    qgltextureutils.cpp \
    qglvertexbuffer.cpp \
    qarray.cpp \
    qareaallocator.cpp \
    qcustomdataarray.cpp \
    qstereoimage.cpp \
    qstereopixmap.cpp \
    qvectorarray.cpp
PRIVATE_HEADERS += \
    qgltexture2d_p.h \
    qgltextureutils_p.h \
    qglvertexbuffer_p.h \
    qareaallocator_p.h
