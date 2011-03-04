/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
