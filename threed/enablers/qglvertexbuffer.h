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

#ifndef QGLVERTEXBUFFER_H
#define QGLVERTEXBUFFER_H

#include <QtOpenGL/qglbuffer.h>
#include "qcustomdataarray.h"
#include "qglnamespace.h"
#include "qglattributevalue.h"
#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLVertexBufferPrivate;
class QGLPainter;
class QGLAbstractEffect;

class Q_QT3D_EXPORT QGLVertexBuffer
{
public:
    QGLVertexBuffer();
    QGLVertexBuffer(const QGLVertexBuffer& other);
    ~QGLVertexBuffer();

    QGLVertexBuffer& operator=(const QGLVertexBuffer& other);

    enum PackingHint
    {
        Interleave,
        Append
    };

    QGLVertexBuffer::PackingHint packingHint() const;
    void setPackingHint(QGLVertexBuffer::PackingHint value);

    QGLBuffer::UsagePattern usagePattern() const;
    void setUsagePattern(QGLBuffer::UsagePattern value);

    void addAttribute(QGL::VertexAttribute attribute,
                      const QArray<float>& value);
    void addAttribute(QGL::VertexAttribute attribute,
                      const QArray<QVector2D>& value);
    void addAttribute(QGL::VertexAttribute attribute,
                      const QArray<QVector3D>& value);
    void addAttribute(QGL::VertexAttribute attribute,
                      const QArray<QVector4D>& value);
    void addAttribute(QGL::VertexAttribute attribute,
                      const QArray<QColor4B>& value);
    void addAttribute(QGL::VertexAttribute attribute,
                      const QCustomDataArray& value);

    void replaceAttribute
        (QGL::VertexAttribute attribute, int index, int count,
         const QGLAttributeValue& value);

    QGLAttributeValue attributeValue(QGL::VertexAttribute attribute) const;

    int vertexCount() const;
    bool isEmpty() const { return vertexCount() == 0; }

    bool upload();
    bool isUploaded() const;

    QGLBuffer *buffer() const;

    bool bind() const;
    void release() const;

private:
    QGLVertexBufferPrivate *d_ptr;

    Q_DECLARE_PRIVATE(QGLVertexBuffer)

    friend class QGLPainter;

    void setOnEffect(QGLAbstractEffect *effect) const;
    QList<QGL::VertexAttribute> attributes() const;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
