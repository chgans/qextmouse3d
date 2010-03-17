TEMPLATE = subdirs
SUBDIRS = teapot cube basket painter stereo qpainter geometry sphere
SUBDIRS += tutorials graphicsview planets displaylist cubeaccel
SUBDIRS += pvcolor
3ds {
    SUBDIRS += 3dscube
}


#contains(QT_CONFIG, multimedia): for(p, QMAKE_INCDIR_QT) {
#    exists("$${p}/QtMultimedia/qabstractvideosurface"): SUBDIRS += video
#}

