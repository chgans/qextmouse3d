INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += \
    qglattributevalue.h \
    qglcontextscope.h \
    qglcontextwatcher.h \
    qglindexbuffer.h \
    qgltexture2d.h \
    qgltexturecube.h \
    qglvertexbuffer.h \
    qarray.h \
    qcustomdataarray.h \
    qvectorarray.h
SOURCES += qglattributevalue.cpp \
    qglcontextscope.cpp \
    qglcontextwatcher.cpp \
    qglindexbuffer.cpp \
    qgltexture2d.cpp \
    qgltexturecube.cpp \
    qgltextureutils.cpp \
    qglvertexbuffer.cpp \
    qarray.cpp \
    qareaallocator.cpp \
    qcustomdataarray.cpp \
    qvectorarray.cpp
PRIVATE_HEADERS += \
    qgltexture2d_p.h \
    qgltextureutils_p.h \
    qareaallocator_p.h
