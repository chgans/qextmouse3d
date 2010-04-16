TEMPLATE = subdirs
SUBDIRS = teaservice shapes
!contains(QT_CONFIG, opengles1cl):!contains(QT_CONFIG, opengles1) {
    SUBDIRS += pageflip shaderwizard
}

contains(QT_CONFIG, declarative) {
    SUBDIRS += qml3d
}

3ds {
    SUBDIRS += modelviewer
}
