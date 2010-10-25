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

#ifndef QSTEREOIMAGE_H
#define QSTEREOIMAGE_H

#include <QtGui/qimage.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QStereoImagePrivate;
class QPainter;

class QStereoImage
{
public:
    enum Layout
    {
        Separate,
        LeftRight,
        RightLeft,
        TopBottom,
        BottomTop
    };

    QStereoImage() : d_ptr(0) {}
    QStereoImage(const QStereoImage &other);
    QStereoImage(const QSize &size, QImage::Format format,
                 QStereoImage::Layout layout = LeftRight);
    QStereoImage(int width, int height, QImage::Format format,
                 QStereoImage::Layout layout = LeftRight);
    explicit QStereoImage(const QString &fileName, const char *format = 0,
                          QStereoImage::Layout layout = LeftRight);
    ~QStereoImage();

    QStereoImage &operator=(const QStereoImage &other);

    bool isNull() const { return d_ptr == 0; }
    QSize size() const;
    int width() const;
    int height() const;

    QStereoImage::Layout layout() const;
    void setLayout(QStereoImage::Layout layout);

    QImage leftImage() const;
    void setLeftImage(const QImage &image);

    QImage rightImage() const;
    void setRightImage(const QImage &image);

    QImage wholeImage() const;
    void setWholeImage
        (const QImage &image, QStereoImage::Layout layout = LeftRight);

    bool beginPaintingLeft(QPainter *painter);
    bool beginPaintingRight(QPainter *painter);

    bool load(QIODevice *device, const char *format,
              QStereoImage::Layout layout = LeftRight);
    bool load(const QString &fileName, const char *format = 0,
              QStereoImage::Layout layout = LeftRight);
    bool save(const QString &fileName, const char *format = 0, int quality = -1) const;
    bool save(QIODevice *device, const char *format = 0, int quality = -1) const;

    void drawLeft(QPainter *painter, const QRectF &targetRect,
                  const QRectF &sourceRect = QRectF(),
                  Qt::ImageConversionFlags flags = Qt::AutoColor);
    void drawRight(QPainter *painter, const QRectF &targetRect,
                   const QRectF &sourceRect = QRectF(),
                   Qt::ImageConversionFlags flags = Qt::AutoColor);

    QImage toAnaglyph();

private:
    mutable QStereoImagePrivate *d_ptr;

    QStereoImagePrivate *d_func();
    const QStereoImagePrivate *d_func() const { return d_ptr; }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
