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

#ifndef QCLIMAGE_H
#define QCLIMAGE_H

#include "qclmemoryobject.h"
#include "qclimageformat.h"
#include "qclevent.h"
#include <QtCore/qrect.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(CL)

class Q_CL_EXPORT QCLImage2D : public QCLMemoryObject
{
public:
    QCLImage2D() {}
    QCLImage2D(QCLContext *context, cl_mem id)
        : QCLMemoryObject(context, id) {}
    QCLImage2D(const QCLImage2D& other)
        : QCLMemoryObject(other.context(), other.id()) {}

    QCLImage2D& operator=(const QCLImage2D& other)
    {
        setId(other.context(), other.id());
        return *this;
    }

    QCLImageFormat format() const;

    int width() const;
    int height() const;

    int bytesPerElement() const;
    int bytesPerLine() const;

    bool read(void *data, const QRect& rect, int bytesPerLine = 0);
    bool read(void *data, const QRect& rect,
              const QVector<QCLEvent>& after, int bytesPerLine = 0);

    QCLEvent readAsync(void *data, const QRect& rect, int bytesPerLine = 0);
    QCLEvent readAsync(void *data, const QRect& rect,
                       const QVector<QCLEvent>& after, int bytesPerLine = 0);

    bool write(const void *data, const QRect& rect, int bytesPerLine = 0);
    bool write(const void *data, const QRect& rect,
               const QVector<QCLEvent>& after, int bytesPerLine = 0);

    QCLEvent writeAsync
        (const void *data, const QRect& rect, int bytesPerLine = 0);
    QCLEvent writeAsync
        (const void *data, const QRect& rect,
         const QVector<QCLEvent>& after, int bytesPerLine = 0);
};

class Q_CL_EXPORT QCLImage3D : public QCLMemoryObject
{
public:
    QCLImage3D() {}
    QCLImage3D(QCLContext *context, cl_mem id)
        : QCLMemoryObject(context, id) {}
    QCLImage3D(const QCLImage3D& other)
        : QCLMemoryObject(other.context(), other.id()) {}

    QCLImage3D& operator=(const QCLImage3D& other)
    {
        setId(other.context(), other.id());
        return *this;
    }

    QCLImageFormat format() const;

    int width() const;
    int height() const;
    int depth() const;

    int bytesPerElement() const;
    int bytesPerLine() const;
    int bytesPerSlice() const;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
