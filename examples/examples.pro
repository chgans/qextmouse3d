TEMPLATE = subdirs
SUBDIRS = teapot cube basket painter stereo qpainter geometry sphere
SUBDIRS += tutorials graphicsview planets displaylist cubehouse
SUBDIRS += pvcolor nesting pie colliding3dmice stereophoto


#contains(QT_CONFIG, multimedia): for(p, QMAKE_INCDIR_QT) {
#    exists("$${p}/QtMultimedia/qabstractvideosurface"): SUBDIRS += video
#}

