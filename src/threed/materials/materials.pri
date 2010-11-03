
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglabstractmaterial.h \
    qglabstractwrappedmaterial.h \
    qglblendmaterial.h \
    qglcolormaterial.h \
    qglmaterial.h \
    qgltwosidedmaterial.h

SOURCES += \
    qglabstractmaterial.cpp \
    qglabstractwrappedmaterial.cpp \
    qglblendmaterial.cpp \
    qglcolormaterial.cpp \
    qglmaterial.cpp \
    qgltwosidedmaterial.cpp

PRIVATE_HEADERS += \
    qglmaterial_p.h
