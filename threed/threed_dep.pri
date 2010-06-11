QT += opengl

INCLUDEPATH += $$PWD/painting
INCLUDEPATH += $$PWD/enablers
INCLUDEPATH += $$PWD/effects
INCLUDEPATH += $$PWD/geometry
INCLUDEPATH += $$PWD/viewing
INCLUDEPATH += $$PWD/math3d
INCLUDEPATH += $$PWD/scene
INCLUDEPATH += $$PWD/graphicsview
INCLUDEPATH += $$PWD/animation
INCLUDEPATH += $$PWD/other

win32:CONFIG(debug, debug|release) {
    LIBS += -lQt3Dd
} else {
    LIBS += -lQt3D
}

# Locate the "lib" directory in the build tree and put it before
# the Qt "lib" directory in the library path so that we link
# against the libQt3D.so in the Qt/3D build tree, not the Qt one.
FIND_TOP=..
for(i,forever) {
    exists($$_PRO_FILE_PWD_/$$FIND_TOP/opengl.pro):break()
    FIND_TOP=../$$FIND_TOP
}
load(qt)
QMAKE_LIBDIR=$$OUT_PWD/$$FIND_TOP/lib $$QMAKE_LIBDIR
