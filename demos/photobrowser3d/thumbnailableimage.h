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

#ifndef THUMBNAILABLEIMAGE_H
#define THUMBNAILABLEIMAGE_H

#include <QRectF>
#include <QMetaType>

#include "qarray.h"
#include "qgeometrydata.h"

class QImage;
class ThumbnailableImagePrivate;
class QGLPainter;

class ThumbnailableImage
{
public:
    ThumbnailableImage();
    ThumbnailableImage(const ThumbnailableImage&);
    ~ThumbnailableImage();

    ThumbnailableImage &operator=(const ThumbnailableImage &);

    void setThumbnailed(bool enable);
    bool isThumbnailed() const;

    QImage data() const;
    void setData(QImage data);

    QUrl url() const;
    void setUrl(const QUrl &url);

    QRectF frame() const;
    qreal scale() const;

    void minimize();
    bool isMinimized() const;

    QGL::IndexArray indices() const;
    void setIndices(const QGL::IndexArray &indices);

    bool isNull() const { return d  == 0; }

    ThumbnailableImagePrivate *priv() const { return d; } /// debug = remove me

private:
    void detach();

    ThumbnailableImagePrivate *d;
};

Q_DECLARE_METATYPE(ThumbnailableImage);

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const ThumbnailableImage &image);
#endif

#endif // THUMBNAILABLEIMAGE_H
