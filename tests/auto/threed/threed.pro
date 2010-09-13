TEMPLATE = subdirs
SUBDIRS = \
    qareaallocator \
    qarray \
    qcolor4ub \
    qcustomdataarray \
    qgraphicstransform3d \
    qvectorarray \
    qglabstractsurface \
    qglcamera \
    qglcube \
    qglindexbuffer \
    qgllightmodel \
    qgllightparameters \
    qglmaterial \
    qglmaterialcollection \
    qglvertexbundle \
    qglpainter \
    qglbuilder \
    qglsection \
    qglsphere \
    qgeometrydata \
    qglpickcolors \
    qstereoimage \
    qglrender

# Qt 4.8 has QGLFunctions, but Qt 4.7 doesn't.
contains(QT_MAJOR_VERSION,4):contains(QT_MINOR_VERSION,7) {
    SUBDIRS += qglfunctions
}
