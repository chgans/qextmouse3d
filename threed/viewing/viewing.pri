
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglcamera.h \
    qglview.h \
    qmouse3devent.h \
    qmouse3deventprovider.h

SOURCES += \
    qglcamera.cpp \
    qglview.cpp \
    qmouse3ddevice.cpp \
    qmouse3ddeviceplugin.cpp \
    qmouse3devent.cpp \
    qmouse3deventprovider.cpp

PRIVATE_HEADERS += \
    qmouse3ddevice_p.h \
    qmouse3ddeviceplugin_p.h
