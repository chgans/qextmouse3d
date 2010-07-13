HEADERS += \
	mouse.h \
        teapotitem.h \
        stereoeffect.h \
        stereographicsview.h
SOURCES += \
	main.cpp \
        mouse.cpp \
        teapotitem.cpp \
        stereoeffect.cpp \
        stereographicsview.cpp

RESOURCES += \
	mice.qrc

QT += opengl

LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
