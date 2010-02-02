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

contains(QT_MAJOR_VERSION,4):!contains(QT_MINOR_VERSION,7) {
    INCLUDEPATH += $$PWD/qt-compat
}

LIBS += -lQt3d
