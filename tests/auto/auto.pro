TEMPLATE = subdirs
SUBDIRS = \
    math3d \
    threed \
    compareimage

!contains(QT_CONFIG, opengles1): SUBDIRS += qglcolladafxeffectfactory
