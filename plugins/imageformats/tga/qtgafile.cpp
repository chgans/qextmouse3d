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

#include "qtgafile.h"

#include <QtCore/qiodevice.h>
#include <QtCore/qdebug.h>
#include <QtCore/qdatetime.h>

struct TgaReader
{
    virtual ~TgaReader() {}
    virtual QRgb operator()(QDataStream *s) const = 0;
    mutable quint8 r, g, b, a;
};

struct Tga16Reader : public TgaReader
{
    ~Tga16Reader() {}
    QRgb operator()(QDataStream *s) const
    {
        *s >> d;
        QRgb result = (d & 0x8000) ? 0xFF000000 : 0x00000000;
        result |= (d & 0x7C00 << 6) | (d & 0x03E0 << 3) | (d & 0x001F);
        return result;
    }
    mutable quint16 d;
};

struct Tga24Reader : public TgaReader
{
    QRgb operator()(QDataStream *s) const
    {
        *s >> b >> g >> r;
        return qRgb(r, g, b);
    }
};

struct Tga32Reader : public TgaReader
{
    QRgb operator()(QDataStream *s) const
    {
        *s >> b >> g >> r >> a;
        return qRgba(r, g, b, a);
    }
};

/*!
    \class QTgaFile
    \since 4.6
    \internal

    File data container for a TrueVision Graphics format file.

    Format is as described here:
    http://local.wasp.uwa.edu.au/~pbourke/dataformats/tga/
    http://netghost.narod.ru/gff2/graphics/summary/tga.htm

    Usage is:
    \code
    QTgaFile tga(myFile);
    QImage tgaImage;
    if (tga.isValid())
        tgaImage = tga.readImage();
    \endcode

    The class is designed to handle sequential and non-sequential
    sources, so during construction the mHeader is read.  Then during
    the readImage() call the rest of the data is read.

    After passing myFile to the constructor, if the QIODevice *myFile
    is read, or has seek() called, the results are undefined - so don't
    do that.
*/

/*!
    Construct a new QTgaFile object getting data from \a device.

    The object does not take ownership of the \a device, but until the
    object is destroyed do not do any non-const operations, eg seek or
    read on the device.
*/
QTgaFile::QTgaFile(QIODevice *device)
    : mDevice(device)
{
    ::memset(mHeader, 0, HeaderSize);
    if (!mDevice->isReadable())
    {
        mErrorMessage = QObject::tr("Could not read image data");
        return;
    }
    if (mDevice->isSequential())
    {
        mErrorMessage = QObject::tr("Sequential device (eg socket) for image read not supported");
        return;
    }
    if (!mDevice->seek(0))
    {
        mErrorMessage = QObject::tr("Seek file/device for image read failed");
        return;
    }
    int bytes = device->read((char*)mHeader, HeaderSize);
    if (bytes != HeaderSize)
    {
        mErrorMessage = QObject::tr("Image mHeader read failed");
        return;
    }
    if (mHeader[ImageType] != 2)
    {
        // TODO: should support other image types
        mErrorMessage = QObject::tr("Image type not supported");
        return;
    }
}

/*!
    \internal
    Destroy the device, recovering any resources.
*/
QTgaFile::~QTgaFile()
{
}

/*!
    \internal
    Reads an image file from the QTgaFile's device, and returns it.

    This method seeks to the absolute position of the image data in the file,
    so no assumptions are made about where the devices read pointer is when this
    method is called.  For this reason only random access devices are supported.

    If the constructor completed successfully, such that isValid() returns true,
    then this method is likely to succeed, unless the file is somehow corrupted.

    In the case that the read fails, the QImage returned will be null, such that
    QImage::isNull() will be true.
*/
QImage QTgaFile::readImage()
{
    if (!isValid())
        return QImage();

    int offset = mHeader[IdLength];  // Mostly always zero

    // Even in TrueColor files a color pallette may be present
    if (mHeader[ColorMapType] == 1)
        offset += littleEndianInt(&mHeader[CMapLength]) * littleEndianInt(&mHeader[CMapDepth]);

    mDevice->seek(HeaderSize + offset);

    // TGA file format is always little endian for its int's
    QDataStream s(mDevice);
    s.setByteOrder(QDataStream::LittleEndian);

    quint8 dummy;
    for (int i = 0; i < offset; ++i)
        s >> dummy;

    int bitsPerPixel = mHeader[PixelDepth];
    int imageWidth = width();
    int imageHeight = height();

    unsigned char desc = mHeader[ImageDescriptor];
    //unsigned char xCorner = desc & 0x10; // 0 = left, 1 = right
    unsigned char yCorner = desc & 0x20; // 0 = lower, 1 = upper

    QImage im(imageWidth, imageHeight, QImage::Format_ARGB32);
    TgaReader *reader = 0;
    if (bitsPerPixel == 16)
        reader = new Tga16Reader();
    else if (bitsPerPixel == 24)
        reader = new Tga24Reader();
    else if (bitsPerPixel == 32)
        reader = new Tga32Reader();
    TgaReader &read = *reader;

    // For now only deal with yCorner, since noone uses xCorner == 1
    // Also this is upside down, since Qt has the origin flipped
    if (yCorner)
    {
        for (int y = 0; y < imageHeight; ++y)
            for (int x = 0; x < imageWidth; ++x)
                im.setPixel(x, y, read(&s));
    }
    else
    {
        for (int y = imageHeight - 1; y >= 0; --y)
            for (int x = 0; x < imageWidth; ++x)
                im.setPixel(x, y, read(&s));
    }

    // TODO: add processing of TGA extension information - ie TGA 2.0 files
    return im;
}
