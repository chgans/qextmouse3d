load(qttest_p4.prf)
TEMPLATE=app
QT += testlib opengl
CONFIG += unittest warn_on

INCLUDEPATH += ../../../../src/threed/textures
VPATH += ../../../../src/threed/textures

HEADERS += qglsharedresource_p.h
SOURCES += tst_qglsharedresource.cpp qglsharedresource.cpp
