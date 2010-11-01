TEMPLATE = subdirs
SUBDIRS = \
    qareaallocator \
    qarray \
    qbox3d \
    qcolor4ub \
    qcustomdataarray \
    qgeometrydata \
    qglabstractsurface \
    qglattributedescription \
    qglattributevalue \
    qglbezierpatches \
    qglbuilder \
    qglcamera \
    qglcameraanimation \
    qglcube \
    qglindexbuffer \
    qgllightmodel \
    qgllightparameters \
    qglmaterial \
    qglmaterialcollection \
    qglpainter \
    qglpickcolors \
    qglrender \
    qglscenenode \
    qglsection \
    qglsharedresource \
    qglsphere \
    qglvertexbundle \
    qgraphicstransform3d \
    qmouse3devent \
    qopenglfunctions \
    qplane3d \
    qray3d \
    qglcylinder \
    qsphere3d \
    qstereoimage \
    qtriangle3d \
    qvectorarray

!contains(QT_CONFIG, opengles1): SUBDIRS += qglcolladafxeffectfactory
