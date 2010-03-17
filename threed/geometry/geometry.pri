INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += qglcube.h \
    qglsphere.h \
    qgeometrydata.h \
    qlogicalvertex.h \
    qgldisplaylist.h \
    qglprimitive.h \
    qglbezierpatches.h \
    qglmaterialcollection.h \
    qglteapot.h \
    qgloperation.h
SOURCES += qglcube.cpp \
    qglsphere.cpp \
    qgeometrydata.cpp \
    qgldisplaylist.cpp \
    qglsection.cpp \
    qglbezierpatches.cpp \
    qglmaterialcollection.cpp \
    qglteapot.cpp \
    qlogicalvertex.cpp \
    qgloperation.cpp \
    qglprimitive.cpp
PRIVATE_HEADERS += qglteapot_data_p.h \
    qgldisplaylist_p.h \
    qglsection_p.h \
    qgldisplaylist_p.h \
    qglteapot_data_p.h \
    qtest_helpers_p.h \
    qvector_utils_p.h

