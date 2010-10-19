
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglflatcoloreffect.h \
    qglflattextureeffect.h \
    qgllitmaterialeffect.h \
    qgllittextureeffect.h \
    qglshaderprogrameffect.h

SOURCES += \
    qglflatcoloreffect.cpp \
    qglflattextureeffect.cpp \
    qgllitmaterialeffect.cpp \
    qgllittextureeffect.cpp \
    qglshaderprogrameffect.cpp

!contains(QT_CONFIG,opengles1) {

HEADERS += \
    qglcolladafxeffectfactory.h \
    qglcolladafxeffect.h \
    qglcolladafxeffectloader.h

PRIVATE_HEADERS += \
    qglcolladafxeffect_p.h

SOURCES += \
    qglcolladafxeffect.cpp \
    qglcolladafxeffectfactory.cpp \
    qglcolladafxeffectloader.cpp

}
