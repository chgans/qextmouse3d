
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglabstracteffect.h \
    qglblendoptions.h \
    qgldepthbufferoptions.h \
    qglfogparameters.h \
    qgllightmodel.h \
    qgllightparameters.h \
    qglmaterial.h \
    qglmatrixstack.h \
    qglnamespace.h \
    qglpainter.h \
    qglstencilbufferoptions.h

SOURCES += \
    qglabstracteffect.cpp \
    qglblendoptions.cpp \
    qgldepthbufferoptions.cpp \
    qglfogparameters.cpp \
    qgllightmodel.cpp \
    qgllightparameters.cpp \
    qglmaterial.cpp \
    qglmatrixstack.cpp \
    qglnamespace.cpp \
    qglpainter.cpp \
    qglpickcolors.cpp \
    qglstencilbufferoptions.cpp

PRIVATE_HEADERS += \
    qglpainter_p.h \
    qglpickcolors_p.h \
    qglmaterial_p.h
