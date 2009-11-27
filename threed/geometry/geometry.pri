INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += qglcube.h \
    qglsphere.h \
    qglgeometry.h \
    qgldisplaylist.h \
    qglsection.h \
    qglbeziergeometry.h \
    qglmaterialcollection.h \
    qglteapot.h \
    qgltexturespecifier.h
SOURCES += qglcube.cpp \
    qglsphere.cpp \
    qglgeometry.cpp \
    qgldisplaylist.cpp \
    qglsection.cpp \
    qglbeziergeometry.cpp \
    qglmaterialcollection.cpp \
    qglteapot.cpp \
    qgltexturespecifier.cpp
PRIVATE_HEADERS += qglteapot_data_p.h \
    qglgeometry_p.h
