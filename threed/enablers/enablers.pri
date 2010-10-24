INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += \
    qglattributedescription.h \
    qglattributevalue.h \
    qglindexbuffer.h \
    qglvertexbundle.h \
    qarray.h \
    qcolor4ub.h \
    qcustomdataarray.h \
    qvector2darray.h \
    qvector3darray.h \
    qvector4darray.h \
    qt3dglobal.h
SOURCES += \
    qglattributedescription.cpp \
    qglattributevalue.cpp \
    qglext.cpp \
    qglindexbuffer.cpp \
    qglvertexbundle.cpp \
    qarray.cpp \
    qcolor4ub.cpp \
    qcustomdataarray.cpp \
    qvector2darray.cpp \
    qvector3darray.cpp \
    qvector4darray.cpp
PRIVATE_HEADERS += \
#    qglext_p.h \    # stop moc from running on this file
    qglvertexbundle_p.h
