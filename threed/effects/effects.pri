
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglflatcoloreffect.h \
    qglflattextureeffect.h \
    qgllitmaterialeffect.h \
    qgllittextureeffect.h

SOURCES += \
    qglflatcoloreffect.cpp \
    qglflattextureeffect.cpp \
    qgllitmaterialeffect.cpp \
    qgllittextureeffect.cpp

!contains(QT_CONFIG,opengles1) {

HEADERS += \
    qglcolladafxeffectfactory.h \
    qglcolladafxeffect.h \
    qglcolladafxeffectloader.h \
    qglshaderprogrameffect.h \
    qglshadereffect.h

PRIVATE_HEADERS += \
    qglcolladafxeffect_p.h

SOURCES += \
    qglcolladafxeffect.cpp \
    qglcolladafxeffectfactory.cpp \
    qglcolladafxeffectloader.cpp \
    qglshaderprogrameffect.cpp \
    qglshadereffect.cpp

}
