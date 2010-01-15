INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += qglcube.h \
    qglsphere.h \
    qglgeometry.h \
    qgeometrydata.h \
    qlogicalvertex.h \
    qgldisplaylist.h \
    qglprimitive.h \
    qglbeziergeometry.h \
    qglmaterialcollection.h \
    qglteapot.h \
    qgloperation.h
SOURCES += qglcube.cpp \
    qglsphere.cpp \
    qglgeometry.cpp \
    qgeometrydata.cpp \
    qgldisplaylist.cpp \
    qglsection.cpp \
    qglbeziergeometry.cpp \
    qglmaterialcollection.cpp \
    qglteapot.cpp \
    qlogicalvertex.cpp \
    qgloperation.cpp \
    qglprimitive.cpp
PRIVATE_HEADERS += qglteapot_data_p.h \
    qglgeometry_p.h \
    qgldisplaylist_p.h \
    qglsection_p.h \
    qgldisplaylist_p.h \
    qtest_helpers_p.h
