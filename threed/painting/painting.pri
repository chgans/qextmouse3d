
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglabstracteffect.h \
    qgllightmodel.h \
    qgllightparameters.h \
    qglmaterial.h \
    qglnamespace.h \
    qglpainter.h

SOURCES += \
    qglabstracteffect.cpp \
    qgllightmodel.cpp \
    qgllightparameters.cpp \
    qglmaterial.cpp \
    qglnamespace.cpp \
    qglpainter.cpp \
    qglpickcolors.cpp

PRIVATE_HEADERS += \
    qglpainter_p.h \
    qglpickcolors_p.h \
    qglmaterial_p.h \
    qglabstracteffect_p.h
