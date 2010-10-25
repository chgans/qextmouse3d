
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglcamera.h \
    qglcameraanimation.h \
    qglview.h \
    qmouse3devent.h \
    qmouse3deventprovider.h

SOURCES += \
    qglcamera.cpp \
    qglcameraanimation.cpp \
    qglview.cpp \
    qmouse3ddevice.cpp \
    qmouse3ddevicelist.cpp \
    qmouse3ddeviceplugin.cpp \
    qmouse3devent.cpp \
    qmouse3deventprovider.cpp

PRIVATE_HEADERS += \
    qmouse3ddevice_p.h \
    qmouse3ddevicelist_p.h \
    qmouse3ddeviceplugin_p.h
