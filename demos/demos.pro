TEMPLATE = subdirs
SUBDIRS = teaservice shapes
!contains(QT_CONFIG, opengles1cl):!contains(QT_CONFIG, opengles1) {
    SUBDIRS += pageflip shaderwizard
}


SUBDIRS += qml3d

