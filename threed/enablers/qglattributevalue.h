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

#ifndef QGLATTRIBUTEVALUE_H
#define QGLATTRIBUTEVALUE_H

#include <QtOpenGL/qgl.h>
#include "qt3dglobal.h"
#include "qarray.h"
#include "qcustomdataarray.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLVertexBuffer;

class Q_QT3D_EXPORT QGLAttributeValue
{
public:
    QGLAttributeValue();
    QGLAttributeValue(const QArray<float>& array);
    QGLAttributeValue(const QArray<QVector2D>& array);
    QGLAttributeValue(const QArray<QVector3D>& array);
    QGLAttributeValue(const QArray<QVector4D>& array);
    QGLAttributeValue(const QArray<QColor4B>& array);
    QGLAttributeValue(const QCustomDataArray& array);
    QGLAttributeValue(int tupleSize, GLenum type, int stride, const void *data);
    QGLAttributeValue(int tupleSize, GLenum type, int stride, int offset);

    bool isNull() const;

    GLenum type() const;
    int sizeOfType() const;
    int tupleSize() const;
    int stride() const;
    int offset() const;
    const void *data() const;
    const float *floatData() const;

private:
    int m_tupleSize;
    GLenum m_type;
    int m_stride;
    const void *m_data;

    void setStride(int stride) { m_stride = stride; }
    void setOffset(int offset)
        { m_data = reinterpret_cast<const void *>(offset); }

    friend class QGLVertexBuffer;
};

inline QGLAttributeValue::QGLAttributeValue()
    : m_tupleSize(0), m_type(GL_FLOAT), m_stride(0), m_data(0)
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<float>& array)
    : m_tupleSize(1), m_type(GL_FLOAT), m_stride(0), m_data(array.constData())
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<QVector2D>& array)
    : m_tupleSize(2), m_type(GL_FLOAT), m_stride(0), m_data(array.constData())
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<QVector3D>& array)
    : m_tupleSize(3), m_type(GL_FLOAT), m_stride(0), m_data(array.constData())
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<QVector4D>& array)
    : m_tupleSize(4), m_type(GL_FLOAT), m_stride(0), m_data(array.constData())
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<QColor4B>& array)
    : m_tupleSize(4), m_type(GL_UNSIGNED_BYTE), m_stride(0), m_data(array.constData())
{
}

inline QGLAttributeValue::QGLAttributeValue
        (int tupleSize, GLenum type, int stride, const void *data)
    : m_tupleSize(tupleSize), m_type(type), m_stride(stride), m_data(data)
{
    Q_ASSERT(tupleSize >= 1 && tupleSize <= 4);
}

inline QGLAttributeValue::QGLAttributeValue
        (int tupleSize, GLenum type, int stride, int offset)
    : m_tupleSize(tupleSize), m_type(type), m_stride(stride),
      m_data(reinterpret_cast<const void *>(offset))
{
    Q_ASSERT(tupleSize >= 1 && tupleSize <= 4);
}

inline bool QGLAttributeValue::isNull() const
{
    return m_tupleSize == 0;
}

inline GLenum QGLAttributeValue::type() const
{
    return m_type;
}

inline int QGLAttributeValue::tupleSize() const
{
    return m_tupleSize;
}

inline int QGLAttributeValue::stride() const
{
    return m_stride;
}

inline int QGLAttributeValue::offset() const
{
    return int(reinterpret_cast<size_t>(m_data));
}

inline const void *QGLAttributeValue::data() const
{
    return m_data;
}

inline const float *QGLAttributeValue::floatData() const
{
    return reinterpret_cast<const float *>(m_data);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
