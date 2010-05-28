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

#ifndef QGLINDEXBUFFER_H
#define QGLINDEXBUFFER_H

#include <QtOpenGL/qgl.h>
#include <QtOpenGL/qglbuffer.h>
#include "qglnamespace.h"
#include "qarray.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLIndexBufferPrivate;
class QGLPainter;

class Q_QT3D_EXPORT QGLIndexBuffer
{
public:
    QGLIndexBuffer();
    QGLIndexBuffer(const QGLIndexBuffer& other);
    ~QGLIndexBuffer();

    QGLIndexBuffer& operator=(const QGLIndexBuffer& other);

    QGLBuffer::UsagePattern usagePattern() const;
    void setUsagePattern(QGLBuffer::UsagePattern value);

    const QArray<ushort> &indices() const;

    void setIndices(const QArray<ushort>& values);
    void replaceIndices(int index, const QArray<ushort>& values);

#if !defined(QT_OPENGL_ES) || defined(qdoc)
    void setIndices(const QArray<int>& values);
    void replaceIndices(int index, const QArray<int>& values);
#endif

    GLenum elementType() const;

    int indexCount() const;
    bool isEmpty() const { return indexCount() == 0; }

    bool upload();
    bool isUploaded() const;

    QGLBuffer buffer() const;

    bool bind() const;
    void release() const;

private:
    QGLIndexBufferPrivate *d_ptr;

    Q_DECLARE_PRIVATE(QGLIndexBuffer)

    friend class QGLPainter;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
