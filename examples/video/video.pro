QT        += phonon multimedia opengl

HEADERS   += videoplayer.h \
             videoview.h \
	     fbovideosurface.h

SOURCES   += main.cpp \
             videoplayer.cpp \
             videoview.cpp \
             fbovideosurface.cpp

include(../../src/threed/threed_dep.pri)
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin
