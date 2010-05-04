HEADERS += \
	mouse.h \
        teapotitem.h \
        redcyaneffect.h
SOURCES += \
	main.cpp \
        mouse.cpp \
        teapotitem.cpp \
        redcyaneffect.cpp

RESOURCES += \
	mice.qrc

QT += opengl

LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
