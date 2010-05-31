
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglabstracteffect.h \
    qglblendoptions.h \
    qgllightmodel.h \
    qgllightparameters.h \
    qglmaterial.h \
    qglmatrixstack.h \
    qglnamespace.h \
    qglpainter.h

SOURCES += \
    qglabstracteffect.cpp \
    qglblendoptions.cpp \
    qgllightmodel.cpp \
    qgllightparameters.cpp \
    qglmaterial.cpp \
    qglmatrixstack.cpp \
    qglnamespace.cpp \
    qglpainter.cpp \
    qglpickcolors.cpp

PRIVATE_HEADERS += \
    qglpainter_p.h \
    qglpickcolors_p.h \
    qglmaterial_p.h \
    qglabstracteffect_p.h
