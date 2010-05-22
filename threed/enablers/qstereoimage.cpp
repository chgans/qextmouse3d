/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qstereoimage.h"
#include <QtCore/qatomic.h>
#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>
#include <QtGui/qpainter.h>
#include <QtGui/qimagereader.h>
#include <QtGui/qimagewriter.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QStereoImage
    \brief The QStereoImage class manages stereo image pairs.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers

    Stereo image pairs contain two versions of the same image, taken from
    the perspective of left and right eye viewpoints.  When presented to a
    viewer with a stereoscopic display, the image appears to have depth.

    QStereoImage simplifies the process of loading, managing, and saving
    stereo image pairs.

    Most stereo image file formats store the two images side by side
    as the left and right halves of a regular JPEG, PNG, or similar image.
    Sometimes a different file extension may be used; for example
    JPS or PNS.  Because of this, QImage will typically load stereo
    pairs as ordinary images that are double-sized without being aware
    of the stereo component.  QStereoImage provides an alternative
    interface that implicitly assumes stereo even when there is no
    explicit indication in the file contents.

    Some formats, such as MPO, encode the two images consecutively one after
    the other.  QStereoImage will assume that the format is side by side
    unless it recognizes the data as stored in a known consecutive format.
*/

/*!
    \enum QStereoImage::Layout
    This enum defines the layout of left and right eye images in a
    stereo pair.

    \value Separate Left and right eye images are stored separately.
    \value LeftRight Left and right eye images are stored in a single
           contiguous image, with the left eye image on the left.
    \value RightLeft Left and right eye images are stored in a single
           contiguous image, with the left eye image on the right.
    \value TopBottom Left and right eye images are stored in a single
           contiguous image, with the left eye image on the top.
    \value BottomTop Left and right eye images are stored in a single
           contiguous image, with the left eye image on the bottom.
*/

static QSize adjustSizeForStereo
    (int width, int height, QStereoImage::Layout layout)
{
    if (layout == QStereoImage::TopBottom || layout == QStereoImage::BottomTop)
        return QSize(width, height * 2);
    else
        return QSize(width * 2, height);
}

static QSize unadjustSizeForStereo
    (int width, int height, QStereoImage::Layout layout)
{
    if (layout == QStereoImage::TopBottom || layout == QStereoImage::BottomTop)
        return QSize(width, height / 2);
    else
        return QSize(width / 2, height);
}

class QStereoImagePrivate
{
public:
    QStereoImagePrivate()
        : ref(1)
        , layout(QStereoImage::Separate) {}
    QStereoImagePrivate
            (int width, int height, QImage::Format format,
             QStereoImage::Layout _layout)
        : ref(1)
        , layout(_layout)
        , size(width, height)
    {
        if (layout == QStereoImage::Separate) {
            leftImage = QImage(width, height, format);
            rightImage = QImage(width, height, format);
        } else {
            wholeImage =
                QImage(adjustSizeForStereo(width, height, layout), format);
        }
        calculateOffsets();
    }
    QStereoImagePrivate(const QStereoImagePrivate *other)
        : ref(1)
        , layout(other->layout)
        , size(other->size)
        , wholeImage(other->wholeImage)
        , leftOffset(other->leftOffset)
        , rightOffset(other->rightOffset)
        , leftImage(other->leftImage)
        , rightImage(other->rightImage) {}

    QAtomicInt ref;
    QStereoImage::Layout layout;
    QSize size;
    QImage wholeImage;
    QPoint leftOffset;
    QPoint rightOffset;
    QImage leftImage;
    QImage rightImage;

    // Work around lack of QImage::constBits() in Qt 4.6.
    const uchar *wholeImageBits() const { return wholeImage.bits(); }

    void calculateOffsets();
    QStereoImagePrivate *changeLayout(QStereoImage::Layout layout);
};

void QStereoImagePrivate::calculateOffsets()
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

QStereoImagePrivate *QStereoImagePrivate::changeLayout
    (QStereoImage::Layout layout)
{
    QStereoImagePrivate *d;
    if (ref == 1) {
        d = this;
    } else {
        d = new QStereoImagePrivate(this);
        ref.deref();
    }
    QImage whole(adjustSizeForStereo
                    (d->size.width(), d->size.height(), layout),
                 (d->wholeImage.isNull() ? d->leftImage.format()
                                         : d->wholeImage.format()));
    QPainter painter(&whole);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    QPoint leftOffset = d->leftOffset;
    QPoint rightOffset = d->rightOffset;
    QStereoImage::Layout prevLayout = d->layout;
    d->layout = layout;
    d->calculateOffsets();
    if (prevLayout == QStereoImage::Separate) {
        painter.drawImage(d->leftOffset.x(), d->leftOffset.y(), d->leftImage);
        painter.drawImage(d->rightOffset.x(), d->rightOffset.y(), d->rightImage);
    } else {
        painter.drawImage(d->leftOffset.x(), d->leftOffset.y(), d->wholeImage,
                          leftOffset.x(), leftOffset.y(),
                          d->size.width(), d->size.height());
        painter.drawImage(d->rightOffset.x(), d->rightOffset.y(), d->wholeImage,
                          rightOffset.x(), rightOffset.y(),
                          d->size.width(), d->size.height());
    }
    painter.end();
    d->wholeImage = whole;
    d->leftImage = QImage();
    d->rightImage = QImage();
    return d;
}

/*!
    \fn QStereoImage::QStereoImage()

    Constructs a null stereo image.
*/

/*!
    Constructs a shallow copy of \a other.
*/
QStereoImage::QStereoImage(const QStereoImage &other)
    : d_ptr(other.d_ptr)
{
    if (d_ptr)
        d_ptr->ref.ref();
}

/*!
    Constructs a new stereo image that is \a size pixels in dimension,
    and where the pixels use the specified \a format.

    The left and right eye images will be laid out in the new image
    according to \a layout.

    The contents of the stereo image after construction are undefined.
*/
QStereoImage::QStereoImage(const QSize &size, QImage::Format format,
                           QStereoImage::Layout layout)
    : d_ptr(new QStereoImagePrivate
                (size.width(), size.height(), format, layout))
{
}

/*!
    Constructs a new stereo image that is \a width x \a height pixels in
    dimension, and where the pixels use the specified \a format.

    The left and right eye images will be laid out in the new image
    according to \a layout.

    The contents of the stereo image after construction are undefined.
*/
QStereoImage::QStereoImage(int width, int height, QImage::Format format,
                           QStereoImage::Layout layout)
    : d_ptr(new QStereoImagePrivate(width, height, format, layout))
{
}

/*!
    Constructs a new stereo image and loads its contents from \a fileName
    in the specified image \a format.  If the \a fileName could not be
    loaded, then the stereo image will be set to null.

    \sa load(), isNull()
*/
QStereoImage::QStereoImage(const QString &fileName, const char *format,
                           QStereoImage::Layout layout)
    : d_ptr(new QStereoImagePrivate)
{
    if (!load(fileName, format, layout)) {
        delete d_ptr;
        d_ptr = 0;
    }
}

/*!
    Destroys this stereo image.
*/
QStereoImage::~QStereoImage()
{
    if (d_ptr && !d_ptr->ref.deref())
        delete d_ptr;
}

/*!
    Assigns a shallow copy of \a other to this object.
*/
QStereoImage &QStereoImage::operator=(const QStereoImage &other)
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
    \fn bool QStereoImage::isNull() const

    Returns true if this stereo image is null.
*/

/*!
    Returns the size of one eye image from this stereo image, or a zero
    size if this stereo image is null.  Both eye images have the same size.

    \sa width(), height()
*/
QSize QStereoImage::size() const
{
    Q_D(const QStereoImage);
    return d ? d->size : QSize(0, 0);
}

/*!
    Returns the width of one eye image from this stereo image, or zero
    if this stereo image is null.  Both eye images have the same size.

    \sa height(), size()
*/
int QStereoImage::width() const
{
    Q_D(const QStereoImage);
    return d ? d->size.width() : 0;
}

/*!
    Returns the height of this stereo image, or zero if this stereo image
    is null.  Both eye images have the same size.

    \sa width(), size()
*/
int QStereoImage::height() const
{
    Q_D(const QStereoImage);
    return d ? d->size.height() : 0;
}

/*!
    Returns the layout of this image, if the left and right eye images
    are stored in a single contiguous image; QStereoImage::Separate if
    the eye images are stored separately or this image is null.

    \sa setLayout(), setWholeImage()
*/
QStereoImage::Layout QStereoImage::layout() const
{
    Q_D(const QStereoImage);
    return d ? d->layout : QStereoImage::Separate;
}

/*!
    Sets the \a layout of this stereo image, rearranging the internal
    storage as necessary.  The request is ignored if the stereo image
    is null.

    \sa layout()
*/
void QStereoImage::setLayout(QStereoImage::Layout layout)
{
    if (!d_ptr)
        return;
    Q_D(QStereoImage);
    if (d->layout == layout)
        return;
    if (layout != QStereoImage::Separate) {
        d->changeLayout(layout);
    } else {
        d->leftImage = d->wholeImage.copy(QRect(d->leftOffset, d->size));
        d->rightImage = d->wholeImage.copy(QRect(d->rightOffset, d->size));
        d->wholeImage = QImage();
        d->layout = QStereoImage::Separate;
        d->calculateOffsets();
    }
}

/*!
    Returns the left eye image from the stereo image.

    \sa setLeftImage(), rightImage(), wholeImage()
*/
QImage QStereoImage::leftImage() const
{
    Q_D(const QStereoImage);
    if (!d)
        return QImage();
    if (d->layout == QStereoImage::Separate)
        return d->leftImage;
    int offset = d->leftOffset.y() * d->wholeImage.bytesPerLine() +
                 d->leftOffset.x() * d->wholeImage.depth() / 8;
    return QImage(d->wholeImageBits() + offset,
                  d->size.width(), d->size.height(),
                  d->wholeImage.bytesPerLine(), d->wholeImage.format());
}

/*!
    Sets the left eye image of the stereo image to \a image.

    \sa leftImage(), setRightImage(), setWholeImage()
*/
void QStereoImage::setLeftImage(const QImage &image)
{
    Q_D(QStereoImage);
    if (d->layout == QStereoImage::Separate) {
        d->leftImage = image;
        d->size = QSize(qMax(d->leftImage.width(), d->rightImage.width()),
                        qMax(d->leftImage.height(), d->rightImage.height()));
    } else {
        QPainter painter(&d->wholeImage);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawImage(d->leftOffset, image);
    }
}

/*!
    Returns the right eye image of the stereo image.

    \sa setRightImage(), leftImage(), wholeImage()
*/
QImage QStereoImage::rightImage() const
{
    Q_D(const QStereoImage);
    if (!d)
        return QImage();
    if (d->layout == QStereoImage::Separate)
        return d->rightImage;
    int offset = d->rightOffset.y() * d->wholeImage.bytesPerLine() +
                 d->rightOffset.x() * d->wholeImage.depth() / 8;
    return QImage(d->wholeImageBits() + offset,
                  d->size.width(), d->size.height(),
                  d->wholeImage.bytesPerLine(), d->wholeImage.format());
}

/*!
    Sets the right eye image of the stereo image to \a image.

    \sa rightImage(), setLeftImage(), setWholeImage()
*/
void QStereoImage::setRightImage(const QImage &image)
{
    Q_D(QStereoImage);
    if (d->layout == QStereoImage::Separate) {
        d->rightImage = image;
        d->size = QSize(qMax(d->leftImage.width(), d->rightImage.width()),
                        qMax(d->leftImage.height(), d->rightImage.height()));
    } else {
        QPainter painter(&d->wholeImage);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawImage(d->rightOffset, image);
    }
}

/*!
    Returns the whole stereo image, if the leftImage() and rightImage()
    are stored in the same QImage buffer.  Returns a null QImage if the
    leftImage() and rightImage() are stored separately or this stereo
    image is null.

    This function can be used by texture upload routines to determine
    if the whole stereo image can be uploaded as a single entity, or if
    leftImage() and rightImage() must be handled separately.

    \sa setWholeImage(), leftImage(), rightImage(), layout()
*/
QImage QStereoImage::wholeImage() const
{
    Q_D(const QStereoImage);
    return (d && d->layout != Separate) ? d->wholeImage : QImage();
}

/*!
    Sets the whole stereo image to \a image, with leftImage() taken from
    one half of \a image, and rightImage() taken from the other half of
    \a image, as defined by \a layout.

    If \a layout is QStereoImage::Separate, it will be converted into
    QStereoImage::LeftRight.

    \sa wholeImage(), setLeftImage(), setRightImage(), layout()
*/
void QStereoImage::setWholeImage
    (const QImage &image, QStereoImage::Layout layout)
{
    Q_D(QStereoImage);
    if (layout == Separate)
        layout = LeftRight;
    d->wholeImage = image;
    d->leftImage = QImage();
    d->rightImage = QImage();
    d->layout = layout;
    d->size = unadjustSizeForStereo(image.width(), image.height(), layout);
    d->calculateOffsets();
}

/*!
    Begins painting into the left eye image of this stereo image.
    Returns true if painting has begun; false otherwise.

    The \a painter is initialized to clip painting operations to the
    left eye image of the stereo image, and translate the co-ordinate
    system so that the origin is at the top-left of the left eye image.

    \sa beginPaintingRight()
*/
bool QStereoImage::beginPaintingLeft(QPainter *painter)
{
    Q_D(QStereoImage);
    if (d->layout == QStereoImage::Separate)
        return painter->begin(&d->leftImage);
    if (!painter->begin(&d->wholeImage))
        return false;
    painter->setClipRect(QRect(d->leftOffset, d->size));
    painter->translate(d->leftOffset);
    return true;
}

/*!
    Begins painting into the right eye image of this stereo image.
    Returns true if painting has begun; false otherwise.

    The \a painter is initialized to clip painting operations to the
    right eye image of the stereo image, and translate the co-ordinate
    system so that the origin is at the top-left of the right eye image.

    \sa beginPaintingLeft()
*/
bool QStereoImage::beginPaintingRight(QPainter *painter)
{
    Q_D(QStereoImage);
    if (d->layout == QStereoImage::Separate)
        return painter->begin(&d->rightImage);
    if (!painter->begin(&d->wholeImage))
        return false;
    painter->setClipRect(QRect(d->rightOffset, d->size));
    painter->translate(d->rightOffset);
    return true;
}

/*!
    Loads a stereo image from \a device in the specified \a format.
    Returns true if the image could be loaded; false otherwise.

    The image is assumed to contain the left and right halves side
    by side according to \a layout in the same image unless the
    \a format specifies a known multi-image format such as MPO.
    The \a layout is ignored if the image data contains explicit
    information about layout.

    If \a format is null, then it will be derived from the data
    that is read from \a device.

    \sa save()
*/
bool QStereoImage::load(QIODevice *device, const char *format,
                        QStereoImage::Layout layout)
{
    Q_D(QStereoImage);
    if (!device)
        return false;

    // Translate known stereo formats into their ordinary counterparts
    // so that QImageReader can find the correct plug-in to use.
    if (!qstricmp(format, "jps") || !qstricmp(format, "mpo"))
        format = "jpeg";
    else if (!qstricmp(format, "pns"))
        format = "png";

    // Read an image from the device.
    QImageReader reader(device, format);
    if (!reader.read(&d->wholeImage))
        return false;

    // TODO: need some way to read the PNG "sTER" chunk to determine if
    // the image is in right-left (mode 0) or left-right (mode 1) order.
    // May need a new option in the QImageIOHandler::ImageOption enum.

    // Peek ahead in the device to see if we have another JFIF header.
    // If so, then assume that we are loading an MPO image.
    QByteArray peek;
    peek = device->peek(512);
    int index;
    static char const jfifTag[2] = {char(0xFF), char(0xD8)};
    if ((index = peek.indexOf(jfifTag)) >= 0) {
        device->read(index);    // Skip padding bytes.
        QImage right = reader.read();
        if (!right.isNull()) {
            d->leftImage = d->wholeImage;
            d->rightImage = right;
            d->wholeImage = QImage();
            d->size = d->leftImage.size();
            d->layout = QStereoImage::Separate;
            d->calculateOffsets();
            return true;
        }
    }

    // Single image with side by side stereo halves.
    if (layout == QStereoImage::Separate)
        layout = QStereoImage::LeftRight;
    if (layout == QStereoImage::LeftRight || layout == QStereoImage::RightLeft)
        d->size = QSize(d->wholeImage.width() / 2, d->wholeImage.height());
    else
        d->size = QSize(d->wholeImage.width(), d->wholeImage.height() / 2);
    d->layout = layout;
    d->leftImage = QImage();
    d->rightImage = QImage();
    d->calculateOffsets();
    return true;
}

/*!
    Loads a stereo image from \a fileName in the specified \a format.
    Returns true if the image could be loaded; false otherwise.

    The image is assumed to contain the left and right halves side
    by side according to \a layout in the same image unless the
    \a format specifies a known multi-image format such as MPO.
    The \a layout is ignored if the image data contains explicit
    information about layout.

    If \a format is null, then it will be derived from the file contents.

    \sa save()
*/
bool QStereoImage::load(const QString &fileName, const char *format,
                        QStereoImage::Layout layout)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    return load(&file, format, layout);
}

/*!
    Saves this stereo image to \a fileName with the specified \a format
    and \a quality parameters.  Returns true if the image could be
    saved; false otherwise.

    The resulting data will contain the left and right halves side by side
    in the same image unless the \a format specifies a known multi-image
    format such as MPO.

    If \a format is null, then it will be derived from the \a fileName
    extension.

    This function may change the layout() to a different value if it isn't
    possible to write the image as \a format in the current layout().
    This will make a temporary copy of the image data, which will have an
    impact on memory consumption.

    \sa load()
*/
bool QStereoImage::save(const QString &fileName, const char *format, int quality) const
{
    QByteArray f(format);
    if (f.isEmpty()) {
        QFileInfo fi(fileName);
        f = fi.suffix().toLatin1();
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    return save(&file, f.constData(), quality);
}

/*!
    Saves this stereo image to \a device with the specified \a format
    and \a quality parameters.  Returns true if the image could be
    saved; false otherwise.

    The resulting data will contain the left and right halves side by side
    in the same image unless the \a format specifies a known multi-image
    format such as MPO.

    If \a format is null, then it will be set to JPS (side by side JPEG).

    This function may change the layout() to a different value if it isn't
    possible to write the image as \a format in the current layout().
    This will make a temporary copy of the image data, which will have an
    impact on memory consumption.

    \sa load()
*/
bool QStereoImage::save(QIODevice *device, const char *format, int quality) const
{
    // Bail out if the parameters are incorrect.
    if (!device || !d_ptr)
        return false;

    // Translate known stereo formats into their ordinary counterparts
    // so that QImageWriter can find the correct plug-in to use.
    bool sideBySide = true;
    if (!format || !qstricmp(format, "jps")) {
        format = "jpeg";
    } else if (!qstricmp(format, "pns")) {
        // TODO: need some way to write out the "sTER" chunk.
        format = "png";
    } else if (!qstricmp(format, "mpo")) {
        format = "jpeg";
        sideBySide = false;
    }

    // Get the private d-pointer and force the data into LeftRight
    // order if we need it to be that way.  TODO: "sTER" RightLeft mode.
    QStereoImagePrivate *d = d_ptr;
    if (sideBySide && d->layout != QStereoImage::LeftRight)
        d = d_ptr = d->changeLayout(QStereoImage::LeftRight);

    // Write out the image.
    QImageWriter writer(device, format);
    writer.setQuality(quality);
    if (sideBySide) {
        return writer.write(d->wholeImage);
    } else {
        // TODO: MPO extension headers for tagging the image data.
        if (!writer.write(leftImage()))
            return false;
        qint64 pos = device->pos();
        if ((pos % 64) != 0) {
            // MPO's appear to align images on a 64-byte boundary.
            static char const padding[64] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            };
            device->write(padding, 64 - (pos % 64));
        }
        return writer.write(rightImage());
    }
}

/*!
    Draws the left eye image of this stereo image onto \a painter at
    \a targetRect.

    If \a sourceRect is not empty, then it indicates the subregion of the
    left eye image to draw at \a targetRect.  The \a flags indicate the image
    conversion options to use when drawing the left eye image.

    This function may be more efficient than calling QPainter::drawImage()
    on the result of leftImage().

    \sa drawRight()
*/
void QStereoImage::drawLeft
    (QPainter *painter, const QRectF &targetRect,
     const QRectF &sourceRect, Qt::ImageConversionFlags flags)
{
    Q_D(const QStereoImage);
    if (!d)
        return;
    if (d->layout == QStereoImage::Separate) {
        painter->drawImage(targetRect, d->leftImage, sourceRect, flags);
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
    painter->drawImage(targetRect, d->wholeImage, sr, flags);
}

/*!
    Draws the right eye image of this stereo image onto \a painter at
    \a targetRect.

    If \a sourceRect is not empty, then it indicates the subregion of the
    right eye image to draw at \a targetRect.  The \a flags indicate the image
    conversion options to use when drawing the right eye image.

    This function may be more efficient than calling QPainter::drawImage()
    on the result of rightImage().

    \sa drawLeft()
*/
void QStereoImage::drawRight
    (QPainter *painter, const QRectF &targetRect,
     const QRectF &sourceRect, Qt::ImageConversionFlags flags)
{
    Q_D(const QStereoImage);
    if (!d)
        return;
    if (d->layout == QStereoImage::Separate) {
        painter->drawImage(targetRect, d->rightImage, sourceRect, flags);
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
    painter->drawImage(targetRect, d->wholeImage, sr, flags);
}

/*!
    \internal
*/
QStereoImagePrivate *QStereoImage::d_func()
{
    if (!d_ptr) {
        d_ptr = new QStereoImagePrivate();
    } else if (d_ptr->ref != 1) {
        QStereoImagePrivate *d = new QStereoImagePrivate(d_ptr);
        d_ptr->ref.deref();
        d_ptr = d;
    }
    return d_ptr;
}

QT_END_NAMESPACE
