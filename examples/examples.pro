TEMPLATE = subdirs
SUBDIRS = teapot cube basket painter stereo qpainter geometry sphere
SUBDIRS += tutorials spot graphicsview planets cubeaccel
3ds {
    SUBDIRS += 3dscube
}


#contains(QT_CONFIG, multimedia): for(p, QMAKE_INCDIR_QT) {
#    exists("$${p}/QtMultimedia/qabstractvideosurface"): SUBDIRS += video
#}

