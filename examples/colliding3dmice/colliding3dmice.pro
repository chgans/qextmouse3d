HEADERS += \
	mouse.h
SOURCES += \
	main.cpp \
        mouse.cpp

RESOURCES += \
	mice.qrc

QT += opengl

LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin
