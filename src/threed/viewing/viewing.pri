
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qmouse3devent.h \
    qmouse3deventprovider.h

SOURCES += \
    qmouse3ddevice.cpp \
    qmouse3ddevicelist.cpp \
    qmouse3ddeviceplugin.cpp \
    qmouse3devent.cpp \
    qmouse3deventprovider.cpp

PRIVATE_HEADERS += \
    qmouse3ddevice_p.h \
    qmouse3ddevicelist_p.h \
    qmouse3ddeviceplugin_p.h
