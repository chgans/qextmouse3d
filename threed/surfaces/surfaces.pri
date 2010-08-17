INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += \
    qglabstractsurface.h \
    qglframebufferobjectsurface.h \
    qglpixelbuffersurface.h \
    qglsubsurface.h \
    qglwidgetsurface.h
SOURCES += \
    qglabstractsurface.cpp \
    qglcontextsurface.cpp \
    qglframebufferobjectsurface.cpp \
    qglpaintersurface.cpp \
    qglpixelbuffersurface.cpp \
    qglsubsurface.cpp \
    qglwidgetsurface.cpp
PRIVATE_HEADERS += \
    qglcontextsurface_p.h \
    qglpaintersurface_p.h
