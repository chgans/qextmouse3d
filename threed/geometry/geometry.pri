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
    qgltexturespecifier.h \
    qlogicalvertex.h
SOURCES += qglcube.cpp \
    qglsphere.cpp \
    qglgeometry.cpp \
    qgldisplaylist.cpp \
    qglsection.cpp \
    qglbeziergeometry.cpp \
    qglmaterialcollection.cpp \
    qglteapot.cpp \
    qgltexturespecifier.cpp \
    qvector3dmapped.cpp \
    qlogicalvertex.cpp \
    qgeometrydata.cpp
PRIVATE_HEADERS += qglteapot_data_p.h \
    qglgeometry_p.h \
    qvector3dmapped_p.h \
    qgeometrydata_p.h \
    qgldisplaylist_p.h
