
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglabstracteffect.h \
    qgllightmodel.h \
    qgllightparameters.h \
    qglmaterial.h \
    qglpainter.h \
    qmatrix4x4stack.h

SOURCES += \
    qglabstracteffect.cpp \
    qgllightmodel.cpp \
    qgllightparameters.cpp \
    qglmaterial.cpp \
    qglpainter.cpp \
    qglpickcolors.cpp \
    qmatrix4x4stack.cpp

PRIVATE_HEADERS += \
    qglpainter_p.h \
    qglpickcolors_p.h \
    qglmaterial_p.h \
    qglabstracteffect_p.h \
    qmatrix4x4stack_p.h
