/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qstereopixmap.h"
#include <QtGui/qpainter.h>

QT_BEGIN_NAMESPACE

/*!
    \class QStereoPixmap
    \brief The QStereoPixmap class manages stereo image pairs using a QPixmap representation.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::enablers

    Stereo image pairs contain two versions of the same image, taken from
    the perspective of left and right eye viewpoints.  When presented to a
    viewer with a stereoscopic display, the image appears to have depth.

    QStereoPixmap simplifies the process of loading, managing, and saving
    stereo image pairs.  It complements the client-side QStereoImage class
    by providing facilities to upload the stereo image pair into the
    graphics server for faster drawing.

    \sa QStereoImage
*/

class QStereoPixmapPrivate
{
public:
    QStereoPixmapPrivate()
        : ref(1)
        , layout(QStereoImage::Separate) {}
    QStereoPixmapPrivate(int width, int height)
        : ref(1)
        , layout(QStereoImage::LeftRight)
        , size(width, height)
        , wholePixmap(width * 2, height)
    {
        calculateOffsets();
    }
    QStereoPixmapPrivate(const QStereoPixmapPrivate *other)
        : ref(1)
        , layout(other->layout)
        , size(other->size)
        , leftPixmap(other->leftPixmap)
        , rightPixmap(other->rightPixmap)
        , wholePixmap(other->wholePixmap)
        , leftOffset(other->leftOffset)
        , rightOffset(other->rightOffset) {}

    QAtomicInt ref;
    QStereoImage::Layout layout;
    QSize size;
    QPixmap leftPixmap;
    QPixmap rightPixmap;
    QPixmap wholePixmap;
    QPoint leftOffset;
    QPoint rightOffset;

    void calculateOffsets();
};

void QStereoPixmapPrivate::calculateOffsets()
{
    switch (layout) {
    case QStereoImage::Separate:
        leftOffset = rightOffset = QPoint(0, 0);
        break;
    case QStereoImage::LeftRight:
        leftOffset = QPoint(0, 0);
        rightOffset = QPoint(size.width(), 0);
        break;
    case QStereoImage::RightLeft:
        leftOffset = QPoint(size.width(), 0);
        rightOffset = QPoint(0, 0);
        break;
    case QStereoImage::TopBottom:
        leftOffset = QPoint(0, 0);
        rightOffset = QPoint(0, size.height());
        break;
    case QStereoImage::BottomTop:
        leftOffset = QPoint(0, size.height());
        rightOffset = QPoint(0, 0);
        break;
    }
}

/*!
    \fn QStereoPixmap::QStereoPixmap()

    Constructs a null stereo pixmap.
 */

/*!
    Constructs a shallow copy of \a other.
*/
QStereoPixmap::QStereoPixmap(const QStereoPixmap &other)
    : d_ptr(other.d_ptr)
{
    if (d_ptr)
        d_ptr->ref.deref();
}

/*!
    Constructs a stereo pixmap with left and right eye images
    of \a width x \a height pixels.
*/
QStereoPixmap::QStereoPixmap(int width, int height)
    : d_ptr(new QStereoPixmapPrivate(width, height))
{
}

/*!
    Constructs a stereo pixmap with left and right eye images
    of \a size pixels.
*/
QStereoPixmap::QStereoPixmap(const QSize &size)
    : d_ptr(new QStereoPixmapPrivate(size.width(), size.height()))
{
}

/*!
    Constructs a new stereo pixmap and loads its contents from \a fileName
    in the specified image \a format.  If the \a fileName could not be
    loaded, then the stereo pixmap will be set to null.

    The image is assumed to contain the left and right halves side
    by side according to \a layout in the same image unless the
    \a format specifies a known multi-image format such as MPO.
    The \a layout is ignored if the image data contains explicit
    information about layout.

    The \a flags are used to convert from QImage form to QPixmap
    form internally.

    \sa fromStereoImage(), isNull()
*/
QStereoPixmap::QStereoPixmap
        (const QString &fileName, const char *format,
         QStereoImage::Layout layout, Qt::ImageConversionFlags flags)
    : d_ptr(0)
{
    QStereoImage image(fileName, format, layout);
    if (!image.isNull())
        *this = fromStereoImage(image, flags);
}

/*!
    Destroys this stereo pixmap.
*/
QStereoPixmap::~QStereoPixmap()
{
    if (d_ptr && !d_ptr->ref.deref())
        delete d_ptr;
}

/*!
    Assigns a shallow copy of \a other to this object.
*/
QStereoPixmap &QStereoPixmap::operator=(const QStereoPixmap &other)
{
    if (d_ptr != other.d_ptr) {
        if (other.d_ptr)
            other.d_ptr->ref.ref();
        if (d_ptr && !d_ptr->ref.deref())
            delete d_ptr;
        d_ptr = other.d_ptr;
    }
    return *this;
}

/*!
    \fn bool QStereoPixmap::isNull() const

    Returns true if this stereo pixmap is null; false otherwise.
*/

/*!
    Returns the size of one eye image from this stereo pixmap, or a zero
    size if this stereo image is null.  Both eye images have the same size.

    \sa width(), height()
*/
QSize QStereoPixmap::size() const
{
    Q_D(const QStereoPixmap);
    return d ? d->size : QSize(0, 0);
}

/*!
    Returns the width of one eye image from this stereo pixmap, or zero
    if this pixmap image is null.  Both eye images have the same size.

    \sa height(), size()
*/
int QStereoPixmap::width() const
{
    Q_D(const QStereoPixmap);
    return d ? d->size.width() : 0;
}

/*!
    Returns the height of one eye image from this stereo pixmap, or zero
    if this pixmap image is null.  Both eye images have the same size.

    \sa width(), size()
*/
int QStereoPixmap::height() const
{
    Q_D(const QStereoPixmap);
    return d ? d->size.height() : 0;
}

/*!
    Returns the layout of this pixmap, if the left and right eye images
    are stored in a single contiguous wholePixmap(); QStereoImage::Separate
    if the eye images are stored separately in leftPixmap() and rightPixmap()
    or this pixmap is null.

    \sa wholePixmap()
*/
QStereoImage::Layout QStereoPixmap::layout() const
{
    Q_D(const QStereoPixmap);
    return d ? d->layout : QStereoImage::Separate;
}

/*!
    Returns the left eye image from the stereo pixmap if layout() is
    QStereoImage::Separate; a null pixmap if layout() is something else.

    \sa rightPixmap(), wholePixmap()
*/
QPixmap QStereoPixmap::leftPixmap() const
{
    Q_D(const QStereoPixmap);
    return d ? d->leftPixmap : QPixmap();
}

/*!
    Returns the right eye image from the stereo pixmap if layout() is
    QStereoImage::Separate; a null pixmap if layout() is something else.

    \sa leftPixmap(), wholePixmap()
*/
QPixmap QStereoPixmap::rightPixmap() const
{
    Q_D(const QStereoPixmap);
    return d ? d->rightPixmap : QPixmap();
}

/*!
    Returns the whole stereo pixmap, if the left and right eye images
    are stored in the same pixmap.  Returns a null pixmap if the
    left and right eye images are stored separately or this stereo
    pixmap is null.

    This function can be used by texture upload routines to determine
    if the whole stereo pixmap can be uploaded as a single entity, or if
    leftPixmap() and rightPixmap() must be handled separately.

    \sa leftPixmap(), rightPixmap(), layout()
*/
QPixmap QStereoPixmap::wholePixmap() const
{
    Q_D(const QStereoPixmap);
    return d ? d->wholePixmap : QPixmap();
}

/*!
    Begins painting into the left eye image of this stereo pixmap.
    Returns true if painting has begun; false otherwise.

    The \a painter is initialized to clip painting operations to the
    left eye image of the stereo pixmap, and translate the co-ordinate
    system so that the origin is at the top-left of the left eye image.

    \sa beginPaintingRight()
*/
bool QStereoPixmap::beginPaintingLeft(QPainter *painter)
{
    Q_D(QStereoPixmap);
    if (d->layout == QStereoImage::Separate)
        return painter->begin(&d->leftPixmap);
    if (!painter->begin(&d->wholePixmap))
        return false;
    painter->setClipRect(QRect(d->leftOffset, d->size));
    painter->translate(d->leftOffset);
    return true;
}

/*!
    Begins painting into the right eye image of this stereo pixmap.
    Returns true if painting has begun; false otherwise.

    The \a painter is initialized to clip painting operations to the
    right eye image of the stereo pixmap, and translate the co-ordinate
    system so that the origin is at the top-left of the right eye image.

    \sa beginPaintingLeft()
*/
bool QStereoPixmap::beginPaintingRight(QPainter *painter)
{
    Q_D(QStereoPixmap);
    if (d->layout == QStereoImage::Separate)
        return painter->begin(&d->rightPixmap);
    if (!painter->begin(&d->wholePixmap))
        return false;
    painter->setClipRect(QRect(d->rightOffset, d->size));
    painter->translate(d->rightOffset);
    return true;
}

/*!
    Draws the left eye image of this stereo pixmap onto \a painter at
    \a targetRect.

    If \a sourceRect is not empty, then it indicates the subregion of the
    left eye image to draw at \a targetRect.

    \sa drawRight()
*/
void QStereoPixmap::drawLeft
    (QPainter *painter, const QRectF &targetRect, const QRectF &sourceRect)
{
    Q_D(const QStereoPixmap);
    if (!d)
        return;
    if (d->layout == QStereoImage::Separate) {
        painter->drawPixmap(targetRect, d->leftPixmap, sourceRect);
        return;
    }
    QRectF sr(sourceRect);
    if (sr.isEmpty()) {
        sr = QRectF(d->leftOffset.x(), d->leftOffset.y(),
                    d->size.width(), d->size.height());
    } else {
        sr = sr.intersected(QRectF(0, 0, d->size.width(), d->size.height()));
        sr.translate(d->leftOffset.x(), d->leftOffset.y());
    }
    painter->drawPixmap(targetRect, d->wholePixmap, sr);
}

/*!
    Draws the right eye image of this stereo pixmap onto \a painter at
    \a targetRect.

    If \a sourceRect is not empty, then it indicates the subregion of the
    right eye image to draw at \a targetRect.

    \sa drawLeft()
*/
void QStereoPixmap::drawRight
    (QPainter *painter, const QRectF &targetRect, const QRectF &sourceRect)
{
    Q_D(const QStereoPixmap);
    if (!d)
        return;
    if (d->layout == QStereoImage::Separate) {
        painter->drawPixmap(targetRect, d->rightPixmap, sourceRect);
        return;
    }
    QRectF sr(sourceRect);
    if (sr.isEmpty()) {
        sr = QRectF(d->rightOffset.x(), d->rightOffset.y(),
                    d->size.width(), d->size.height());
    } else {
        sr = sr.intersected(QRectF(0, 0, d->size.width(), d->size.height()));
        sr.translate(d->rightOffset.x(), d->rightOffset.y());
    }
    painter->drawPixmap(targetRect, d->wholePixmap, sr);
}

/*!
    Returns a stereo pixmap that contains the contents of \a image;
    or a null stereo pixmap if \a image is null.

    The \a flags are used to convert from QImage form to QPixmap
    form internally.

    \sa toStereoImage()
*/
QStereoPixmap QStereoPixmap::fromStereoImage
    (const QStereoImage &image, Qt::ImageConversionFlags flags)
{
    if (image.isNull())
        return QStereoPixmap();
    QStereoPixmapPrivate *d = new QStereoPixmapPrivate();
    QStereoImage::Layout layout = image.layout();
    if (layout == QStereoImage::Separate) {
        d->leftPixmap = QPixmap::fromImage(image.leftImage(), flags);
        d->rightPixmap = QPixmap::fromImage(image.rightImage(), flags);
        d->size = image.size();
    } else {
        d->wholePixmap = QPixmap::fromImage(image.wholeImage(), flags);
        d->size = image.size();
        d->layout = layout;
        d->calculateOffsets();
    }
    return QStereoPixmap(d);
}

/*!
    Returns the contents of this stereo pixmap as a QStereoImage.

    \sa fromStereoImage()
*/
QStereoImage QStereoPixmap::toStereoImage() const
{
    Q_D(const QStereoPixmap);
    if (!d)
        return QStereoImage();
    QStereoImage image;
    if (d->layout == QStereoImage::Separate) {
        image.setLeftImage(d->leftPixmap.toImage());
        image.setRightImage(d->rightPixmap.toImage());
    } else {
        image.setWholeImage(d->wholePixmap.toImage(), d->layout);
    }
    return image;
}

/*!
    \internal
*/
QStereoPixmapPrivate *QStereoPixmap::d_func()
{
    if (!d_ptr) {
        d_ptr = new QStereoPixmapPrivate();
    } else if (d_ptr->ref != 1) {
        QStereoPixmapPrivate *d = new QStereoPixmapPrivate(d_ptr);
        d_ptr->ref.deref();
        d_ptr = d;
    }
    return d_ptr;
}

QT_END_NAMESPACE
