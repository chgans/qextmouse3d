include(painting/painting.pri)
include(enablers/enablers.pri)
include(effects/effects.pri)
include(geometry/geometry.pri)
include(viewing/viewing.pri)
include(math3d/math3d.pri)
include(scene/scene.pri)
include(graphicsview/graphicsview.pri)
include(animation/animation.pri)
include(other/other.pri)

# Qt 4.8 has QGLFunctions, but Qt 4.7 doesn't.
contains(QT_MAJOR_VERSION,4):contains(QT_MINOR_VERSION,7) {
    include(api/api.pri)
}
