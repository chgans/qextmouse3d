include(painting/painting.pri)
include(enablers/enablers.pri)
include(effects/effects.pri)
include(geometry/geometry.pri)
include(viewing/viewing.pri)
include(math3d/math3d.pri)
include(scene/scene.pri)
include(graphicsview/graphicsview.pri)
include(animation/animation.pri)

contains(QT_MAJOR_VERSION,4):!contains(QT_MINOR_VERSION,7) {
    include(qt-compat/qt-compat.pri)
}
