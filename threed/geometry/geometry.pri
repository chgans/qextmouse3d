INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += qglcube.h \
    qglsphere.h \
    qglgeometry.h \
    qgldisplaylist.h \
    qglbeziergeometry.h \
    qglmaterialcollection.h \
    qglteapot.h \
    qgltexturemodel.h \
    qtest_helpers_p.h
SOURCES += qglcube.cpp \
    qglsphere.cpp \
    qglgeometry.cpp \
    qgldisplaylist.cpp \
    qglsection.cpp \
    qglbeziergeometry.cpp \
    qglmaterialcollection.cpp \
    qglteapot.cpp \
    qgltexturemodel.cpp \
    qlogicalvertex.cpp \
    qgeometrydata.cpp
PRIVATE_HEADERS += qglteapot_data_p.h \
    qglgeometry_p.h \
    qgeometrydata_p.h \
    qglsection_p.h \
    qlogicalvertex_p.h \
    qgldisplaylist_p.h
