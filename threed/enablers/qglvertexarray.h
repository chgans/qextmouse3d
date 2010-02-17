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

#ifndef QGLVERTEXARRAY_H
#define QGLVERTEXARRAY_H

#include <QtCore/qlist.h>
#include "qvector2d.h"
#include "qvector3d.h"
#include "qvector4d.h"
#include "qcolor4b.h"
#include "qarray.h"
#include "qglvertexdescription.h"
#include "qglattributevalue.h"
#include "qglreferencedbuffer.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class Q_QT3D_EXPORT QGLVertexArray
{
public:
    inline QGLVertexArray();
    inline QGLVertexArray(QGL::VertexAttribute attr, int size);
    inline QGLVertexArray(QGL::VertexAttribute attr1, int size1,
                          QGL::VertexAttribute attr2, int size2);
    inline QGLVertexArray(QGL::VertexAttribute attr1, int size1,
                          QGL::VertexAttribute attr2, int size2,
                          QGL::VertexAttribute attr3, int size3);
    inline QGLVertexArray(QGL::VertexAttribute attr1, int size1,
                          QGL::VertexAttribute attr2, int size2,
                          QGL::VertexAttribute attr3, int size3,
                          QGL::VertexAttribute attr4, int size4);
    inline QGLVertexArray(const QGLVertexArray& other);
    inline ~QGLVertexArray();

    inline QGLVertexArray& operator=(const QGLVertexArray& other);

    inline void addField(QGL::VertexAttribute attr, int size);
    inline int fieldCount() const;

    inline QGLVertexDescription fields() const;
    inline void setFields(const QGLVertexDescription& value);

    inline QGLAttributeValue attributeValue(int field) const;
    inline QGLAttributeValue attributeValue(QGL::VertexAttribute attr) const;

    inline int fieldIndex(QGL::VertexAttribute attr) const;

    inline int stride() const;

    inline int vertexCount() const;
    inline int componentCount() const;

    inline bool isEmpty() const;

    void resize(int size);
    void reserve(int size);

    inline void append(qreal x);
    inline void append(qreal x, qreal y);
    inline void append(qreal x, qreal y, qreal z);
    inline void append(qreal x, qreal y, qreal z, qreal w);
    inline void append(const QVector2D& value);
    inline void append(const QVector3D& value);
    inline void append(const QVector4D& value);
    inline void append(const QPoint& value);
    inline void append(const QPointF& value);
    inline void append(const QColor4B& value);
    void append(const QGLVertexArray& array);

    inline qreal floatAt(int vertex, int field) const;
    inline QVector2D vector2DAt(int vertex, int field) const;
    inline QVector3D vector3DAt(int vertex, int field) const;
    inline QVector4D vector4DAt(int vertex, int field) const;
    inline QColor4B color4bAt(int vertex, int field) const;

    inline void setAt(int vertex, int field, qreal x);
    inline void setAt(int vertex, int field, qreal x, qreal y);
    inline void setAt(int vertex, int field, qreal x, qreal y, qreal z);
    inline void setAt(int vertex, int field, qreal x, qreal y, qreal z, qreal w);
    inline void setAt(int vertex, int field, const QVector2D& value);
    inline void setAt(int vertex, int field, const QVector3D& value);
    inline void setAt(int vertex, int field, const QVector4D& value);
    inline void setAt(int vertex, int field, const QColor4B& value);

    QGLVertexArray extractField(int field) const;
    QGLVertexArray extractField(QGL::VertexAttribute attr) const;

    QGLVertexArray interleaved(const QGLVertexArray& other) const;

    inline float *data();
    inline const float *data() const;
    inline const float *constData() const;

    inline QGLVertexArray& operator+=(qreal value);
    inline QGLVertexArray& operator+=(const QVector2D& value);
    inline QGLVertexArray& operator+=(const QVector3D& value);
    inline QGLVertexArray& operator+=(const QVector4D& value);
    inline QGLVertexArray& operator+=(const QGLVertexArray& other);
    inline QGLVertexArray& operator+=(const QPoint& value);
    inline QGLVertexArray& operator+=(const QPointF& value);
    inline QGLVertexArray& operator+=(const QColor4B& value);

    inline QGLVertexArray& operator<<(qreal value);
    inline QGLVertexArray& operator<<(const QVector2D& value);
    inline QGLVertexArray& operator<<(const QVector3D& value);
    inline QGLVertexArray& operator<<(const QVector4D& value);
    inline QGLVertexArray& operator<<(const QGLVertexArray& other);
    inline QGLVertexArray& operator<<(const QPoint& value);
    inline QGLVertexArray& operator<<(const QPointF& value);
    inline QGLVertexArray& operator<<(const QColor4B& value);

    void setRawData(const float *data, int count);

    void replace(int vertex, const QGLVertexArray& data);

    inline bool isVertexBuffer() const;

    bool operator==(const QGLVertexArray &rhs) const;
    inline bool operator!=(const QGLVertexArray &rhs) const;

    bool bind() const;
    void release() const;

    bool upload() const;
    inline bool isUploaded() const;

private:
    QArray<float> m_data;
    QGLVertexDescription m_fields;
    mutable int m_currentField;
    mutable int m_warnings;
    mutable QGLReferencedBuffer *m_buffer;

    inline void detachBuffer();

    void typeCheckAppend(int size) const;
    void typeCheckRead(int field, int size) const;
    void typeCheckWrite(int field, int size) const;

    QGLVertexArray toBufferForm() const;

    friend class QGLAbstractEffect;
    friend class QGLPainter;
    friend class QGLPainterPrivate;
};

inline void QGLVertexArray::detachBuffer()
{
    if (m_buffer && !m_buffer->deref())
        delete m_buffer;
    m_buffer = 0;
}

inline QGLVertexArray::QGLVertexArray()
    : m_currentField(0), m_warnings(0), m_buffer(0)
{
}

inline QGLVertexArray::QGLVertexArray(QGL::VertexAttribute attr, int size)
    : m_currentField(0), m_warnings(0), m_buffer(0)
{
    m_fields.addField(attr, size);
}

inline QGLVertexArray::QGLVertexArray
        (QGL::VertexAttribute attr1, int size1,
         QGL::VertexAttribute attr2, int size2)
    : m_currentField(0), m_warnings(0), m_buffer(0)
{
    m_fields.addField(attr1, size1);
    m_fields.addField(attr2, size2);
}

inline QGLVertexArray::QGLVertexArray
        (QGL::VertexAttribute attr1, int size1,
         QGL::VertexAttribute attr2, int size2,
         QGL::VertexAttribute attr3, int size3)
    : m_currentField(0), m_warnings(0), m_buffer(0)
{
    m_fields.addField(attr1, size1);
    m_fields.addField(attr2, size2);
    m_fields.addField(attr3, size3);
}

inline QGLVertexArray::QGLVertexArray
        (QGL::VertexAttribute attr1, int size1,
         QGL::VertexAttribute attr2, int size2,
         QGL::VertexAttribute attr3, int size3,
         QGL::VertexAttribute attr4, int size4)
    : m_currentField(0), m_warnings(0), m_buffer(0)
{
    m_fields.addField(attr1, size1);
    m_fields.addField(attr2, size2);
    m_fields.addField(attr3, size3);
    m_fields.addField(attr4, size4);
}

inline QGLVertexArray::QGLVertexArray(const QGLVertexArray& other)
    : m_data(other.m_data), m_fields(other.m_fields),
      m_currentField(other.m_currentField),
      m_warnings(other.m_warnings),
      m_buffer(other.m_buffer)
{
    if (m_buffer)
        m_buffer->ref();
}

inline QGLVertexArray::~QGLVertexArray()
{
    if (m_buffer && !m_buffer->deref())
        delete m_buffer;
}

inline QGLVertexArray& QGLVertexArray::operator=(const QGLVertexArray& other)
{
    if (this != &other) {
        m_data = other.m_data;
        m_fields = other.m_fields;
        m_currentField = other.m_currentField;
        m_warnings = other.m_warnings;
        if (m_buffer != other.m_buffer) {
            if (m_buffer && !m_buffer->deref())
                delete m_buffer;
            m_buffer = other.m_buffer;
            if (m_buffer)
                m_buffer->ref();
        }
    }
    return *this;
}

inline void QGLVertexArray::addField(QGL::VertexAttribute attr, int size)
{
    m_fields.addField(attr, size);
}

inline int QGLVertexArray::fieldCount() const
{
    return m_fields.fieldCount();
}

inline QGLVertexDescription QGLVertexArray::fields() const
{
    return m_fields;
}

inline void QGLVertexArray::setFields(const QGLVertexDescription& value)
{
    m_fields = value;
    m_currentField = 0;
    m_warnings = 0;
}

inline QGLAttributeValue QGLVertexArray::attributeValue(int field) const
{
    if (field >= 0 && field < m_fields.m_fields.count()) {
        if (m_fields.m_fields[field].size == 1 &&
            m_fields.m_fields[field].attr == QGL::Color)
        {
            // special case for QColor4B
            return QGLAttributeValue(4, GL_UNSIGNED_BYTE, m_fields.m_stride * sizeof(float),
                                     m_data.constData() + m_fields.m_fields[field].offset);
        }
        return QGLAttributeValue
            (m_fields.m_fields[field].size, GL_FLOAT,
             m_fields.m_stride * sizeof(float),
             m_data.constData() + m_fields.m_fields[field].offset);
    } else {
        return QGLAttributeValue();
    }
}

inline QGLAttributeValue QGLVertexArray::attributeValue
            (QGL::VertexAttribute attr) const
{
    return attributeValue(m_fields.indexOf(attr));
}

inline int QGLVertexArray::fieldIndex(QGL::VertexAttribute attr) const
{
    return m_fields.indexOf(attr);
}

inline int QGLVertexArray::stride() const
{
    return m_fields.stride();
}

inline int QGLVertexArray::vertexCount() const
{
    if (m_fields.stride())
        return m_data.count() / m_fields.stride();
    else
        return m_data.count();
}

inline int QGLVertexArray::componentCount() const
{
    return m_data.count();
}

inline bool QGLVertexArray::isEmpty() const
{
    return m_data.isEmpty();
}

inline void QGLVertexArray::append(qreal x)
{
#ifndef QT_NO_DEBUG
    typeCheckAppend(1);
#endif
    detachBuffer();
    m_data.append(x);
}

inline void QGLVertexArray::append(qreal x, qreal y)
{
#ifndef QT_NO_DEBUG
    typeCheckAppend(2);
#endif
    detachBuffer();
    m_data.append(x, y);
}

inline void QGLVertexArray::append(qreal x, qreal y, qreal z)
{
#ifndef QT_NO_DEBUG
    typeCheckAppend(3);
#endif
    detachBuffer();
    m_data.append(x, y, z);
}

inline void QGLVertexArray::append(qreal x, qreal y, qreal z, qreal w)
{
#ifndef QT_NO_DEBUG
    typeCheckAppend(4);
#endif
    detachBuffer();
    m_data.append(x, y, z, w);
}

inline void QGLVertexArray::append(const QVector2D& value)
{
#ifndef QT_NO_DEBUG
    typeCheckAppend(2);
#endif
    detachBuffer();
    m_data.append(value.x(), value.y());
}

inline void QGLVertexArray::append(const QVector3D& value)
{
#ifndef QT_NO_DEBUG
    typeCheckAppend(3);
#endif
    detachBuffer();
    m_data.append(value.x(), value.y(), value.z());
}

inline void QGLVertexArray::append(const QVector4D& value)
{
#ifndef QT_NO_DEBUG
    typeCheckAppend(4);
#endif
    detachBuffer();
    m_data.append(value.x(), value.y(), value.z(), value.w());
}

inline void QGLVertexArray::append(const QPoint& value)
{
#ifndef QT_NO_DEBUG
    typeCheckAppend(2);
#endif
    detachBuffer();
    m_data.append(value.x(), value.y());
}

inline void QGLVertexArray::append(const QPointF& value)
{
#ifndef QT_NO_DEBUG
    typeCheckAppend(2);
#endif
    detachBuffer();
    m_data.append(value.x(), value.y());
}

inline void QGLVertexArray::append(const QColor4B& value)
{
#ifndef QT_NO_DEBUG
    typeCheckAppend(1); // 1 float component == 4 byte components.
#endif
    detachBuffer();
    int count = m_data.count();
    m_data.resize(count + 1);
    *reinterpret_cast<QColor4B *>(m_data.data() + count) = value;
}

inline qreal QGLVertexArray::floatAt(int vertex, int field) const
{
#ifndef QT_NO_DEBUG
    typeCheckRead(field, 1);
#endif
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && offset < m_data.count())
        return m_data[offset];
    else
        return 0.0f;
}

inline QVector2D QGLVertexArray::vector2DAt(int vertex, int field) const
{
#ifndef QT_NO_DEBUG
    typeCheckRead(field, 2);
#endif
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && (offset + 1) < m_data.count()) {
        return QVector2D(m_data[offset], m_data[offset + 1]);
    } else {
        return QVector2D();
    }
}

inline QVector3D QGLVertexArray::vector3DAt(int vertex, int field) const
{
#ifndef QT_NO_DEBUG
    typeCheckRead(field, 3);
#endif
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && (offset + 2) < m_data.count()) {
        return QVector3D
            (m_data[offset], m_data[offset + 1], m_data[offset + 2]);
    } else {
        return QVector3D();
    }
}

inline QVector4D QGLVertexArray::vector4DAt(int vertex, int field) const
{
#ifndef QT_NO_DEBUG
    typeCheckRead(field, 4);
#endif
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && (offset + 3) < m_data.count()) {
        return QVector4D
            (m_data[offset], m_data[offset + 1],
             m_data[offset + 2], m_data[offset + 3]);
    } else {
        return QVector4D();
    }
}

inline QColor4B QGLVertexArray::color4bAt(int vertex, int field) const
{
#ifndef QT_NO_DEBUG
    typeCheckRead(field, 1); // 1 float component == 4 byte components.
#endif
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && offset < m_data.count())
        return *reinterpret_cast<const QColor4B *>(&(m_data[offset]));
    else
        return QColor4B();
}

inline void QGLVertexArray::setAt(int vertex, int field, qreal x)
{
#ifndef QT_NO_DEBUG
    typeCheckWrite(field, 1);
#endif
    detachBuffer();
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && offset < m_data.count())
        m_data[offset] = x;
}

inline void QGLVertexArray::setAt(int vertex, int field, qreal x, qreal y)
{
#ifndef QT_NO_DEBUG
    typeCheckWrite(field, 2);
#endif
    detachBuffer();
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && (offset + 1) < m_data.count()) {
        m_data[offset] = x;
        m_data[offset + 1] = y;
    }
}

inline void QGLVertexArray::setAt(int vertex, int field, qreal x, qreal y, qreal z)
{
#ifndef QT_NO_DEBUG
    typeCheckWrite(field, 3);
#endif
    detachBuffer();
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && (offset + 2) < m_data.count()) {
        m_data[offset] = x;
        m_data[offset + 1] = y;
        m_data[offset + 2] = z;
    }
}

inline void QGLVertexArray::setAt(int vertex, int field, qreal x, qreal y, qreal z, qreal w)
{
#ifndef QT_NO_DEBUG
    typeCheckWrite(field, 4);
#endif
    detachBuffer();
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && (offset + 3) < m_data.count()) {
        m_data[offset] = x;
        m_data[offset + 1] = y;
        m_data[offset + 2] = z;
        m_data[offset + 3] = w;
    }
}

inline void QGLVertexArray::setAt(int vertex, int field, const QVector2D& value)
{
#ifndef QT_NO_DEBUG
    typeCheckWrite(field, 2);
#endif
    detachBuffer();
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && (offset + 1) < m_data.count()) {
        m_data[offset] = value.x();
        m_data[offset + 1] = value.y();
    }
}

inline void QGLVertexArray::setAt(int vertex, int field, const QVector3D& value)
{
#ifndef QT_NO_DEBUG
    typeCheckWrite(field, 3);
#endif
    detachBuffer();
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && (offset + 2) < m_data.count()) {
        m_data[offset] = value.x();
        m_data[offset + 1] = value.y();
        m_data[offset + 2] = value.z();
    }
}

inline void QGLVertexArray::setAt(int vertex, int field, const QVector4D& value)
{
#ifndef QT_NO_DEBUG
    typeCheckWrite(field, 4);
#endif
    detachBuffer();
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && (offset + 3) < m_data.count()) {
        m_data[offset] = value.x();
        m_data[offset + 1] = value.y();
        m_data[offset + 2] = value.z();
        m_data[offset + 3] = value.w();
    }
}

inline void QGLVertexArray::setAt(int vertex, int field, const QColor4B& value)
{
#ifndef QT_NO_DEBUG
    typeCheckWrite(field, 1); // 1 float component == 4 byte components.
#endif
    detachBuffer();
    int offset = vertex * m_fields.stride() + m_fields.fieldOffset(field);
    if (offset >= 0 && offset < m_data.count())
        *reinterpret_cast<QColor4B *>(&(m_data[offset])) = value;
}

inline float *QGLVertexArray::data()
{
    if (isEmpty())
        return 0;
    else
        return m_data.data();
}

inline const float *QGLVertexArray::data() const
{
    if (isEmpty())
        return 0;
    else
        return m_data.constData();
}

inline const float *QGLVertexArray::constData() const
{
    if (isEmpty())
        return 0;
    else
        return m_data.constData();
}

inline QGLVertexArray& QGLVertexArray::operator+=(qreal value)
{
    append(value);
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator+=(const QVector2D& value)
{
    append(value);
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator+=(const QVector3D& value)
{
    append(value);
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator+=(const QVector4D& value)
{
    append(value);
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator+=(const QGLVertexArray& other)
{
    append(other);
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator+=(const QPoint& value)
{
    append(value.x(), value.y());
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator+=(const QPointF& value)
{
    append(value.x(), value.y());
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator+=(const QColor4B& value)
{
    append(value);
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator<<(qreal value)
{
    append(value);
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator<<(const QVector2D& value)
{
    append(value);
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator<<(const QVector3D& value)
{
    append(value);
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator<<(const QVector4D& value)
{
    append(value);
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator<<(const QGLVertexArray& other)
{
    append(other);
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator<<(const QPoint& value)
{
    append(value.x(), value.y());
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator<<(const QPointF& value)
{
    append(value.x(), value.y());
    return *this;
}

inline QGLVertexArray& QGLVertexArray::operator<<(const QColor4B& value)
{
    append(value);
    return *this;
}

inline bool QGLVertexArray::isVertexBuffer() const
{
    return m_data.constData() == 0;
}

inline bool QGLVertexArray::operator!=(const QGLVertexArray &other) const
{
    return !(*this == other);
}

inline bool QGLVertexArray::isUploaded() const
{
    return m_buffer != 0;
}

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLVertexArray &vertices);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
