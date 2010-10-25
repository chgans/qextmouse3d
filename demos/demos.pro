TEMPLATE = subdirs
SUBDIRS = teaservice shapes modelviewer photobrowser3d cubehouse
!contains(QT_CONFIG, opengles1) {
    SUBDIRS += pageflip shaderwizard
}

contains(QT_CONFIG, declarative) {
    SUBDIRS += qml3d
}
