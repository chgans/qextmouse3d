INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += qglabstractscene.h \
    qglsceneformatplugin.h \
    qglsceneobject.h \
    qglscenenode.h \
    scene/qglpicknode.h
SOURCES += qglabstractscene.cpp \
    qglsceneformatplugin.cpp \
    qglsceneobject.cpp \
    qglscenenode.cpp \
    scene/qglpicknode.cpp
PRIVATE_HEADERS += qglscenenode_p.h \
    qglsceneobject_p.h
