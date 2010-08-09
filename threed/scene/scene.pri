INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += qglabstractscene.h \
    qglsceneformatplugin.h \
    qglscenenode.h \
    qglpicknode.h \
    scene/qglrendersequencer.h \
    scene/qglrenderorder.h \
    scene/qglrenderorderrepository.h \
    scene/qglrenderstate.h
SOURCES += qglabstractscene.cpp \
    qglsceneformatplugin.cpp \
    qglscenenode.cpp \
    qglpicknode.cpp \
    scene/qglrendersequencer.cpp \
    scene/qglrenderorder.cpp \
    scene/qglrenderorderrepository.cpp \
    scene/qglrenderstate.cpp
PRIVATE_HEADERS += qglscenenode_p.h
