
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglshaderprogrameffect.h

SOURCES += \
    qglflatcoloreffect.cpp \
    qglflattextureeffect.cpp \
    qgllitmaterialeffect.cpp \
    qgllittextureeffect.cpp \
    qglshaderprogrameffect.cpp

PRIVATE_HEADERS += \
    qglflatcoloreffect_p.h \
    qglflattextureeffect_p.h \
    qgllitmaterialeffect_p.h \
    qgllittextureeffect_p.h

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
