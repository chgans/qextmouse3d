
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglcube.h \
    qglsphere.h \
    qglgeometry.h \
    qglbeziergeometry.h \
    qglmaterialcollection.h \
    qglteapot.h

SOURCES += \
    qglcube.cpp \
    qglsphere.cpp \
    qglgeometry.cpp \
    qglbeziergeometry.cpp \
    qglmaterialcollection.cpp \
    qglteapot.cpp

PRIVATE_HEADERS += \
    qglteapot_data_p.h \
    qtest_helpers.h
