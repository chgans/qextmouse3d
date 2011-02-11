
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qt3dquickglobal.h \
    qdeclarativeitem3d.h

SOURCES += \
    qdeclarativeeffect.cpp \
    qdeclarativeitem3d.cpp \
    qdeclarativemesh.cpp \
    qsgpretransformnode.cpp \
    qsgstereocontext.cpp

PRIVATE_HEADERS += \
    qdeclarativeeffect_p.h \
    qdeclarativemesh_p.h \
    qdeclarativeviewport_p.h \
    qsgpretransformnode_p.h \
    qsgstereocontext_p.h
