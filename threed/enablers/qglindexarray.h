/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QGLINDEXARRAY_H
#define QGLINDEXARRAY_H

#include "qdataarray.h"
#include "qglreferencedbuffer.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class Q_QT3D_EXPORT QGLIndexArray
{
public:
#if defined(QT_OPENGL_ES) && !defined(Q_QDOC)
    typedef ushort ElementType;
#else
    typedef int ElementType;
#endif

    inline QGLIndexArray();
    inline QGLIndexArray(const QGLIndexArray& other);
    inline ~QGLIndexArray();

    inline QGLIndexArray& operator=(const QGLIndexArray& other);

    inline int size() const;
    inline bool isEmpty() const;

    void reserve(int size);

    inline void append(int value);
    inline void append(int value1, int value2);
    inline void append(int value1, int value2, int value3);
    inline void append(int value1, int value2, int value3, int value4);
    void append(const QGLIndexArray& array);

    void replace(int index, const QGLIndexArray& data);

    inline int operator[](int index) const;
    inline ElementType& operator[](int index);

    inline int type() const;
    inline const ElementType *data() const;
    inline const ElementType *constData() const;

    inline QGLIndexArray& operator+=(int value);
    inline QGLIndexArray& operator<<(int value);

    static QGLIndexArray fromRawData(const ushort *data, int count);
    static QGLIndexArray fromRawData(const int *data, int count);
    static inline QGLIndexArray fromRawData(const uint *data, int count);

    bool operator==(const QGLIndexArray &other) const;

    bool bind() const;
    void release() const;

    bool upload() const;
    inline bool isUploaded() const;

private:
    QArray<ElementType> m_data;
    mutable QGLReferencedBuffer *m_buffer;

    inline void detachBuffer();
};

inline void QGLIndexArray::detachBuffer()
{
    if (m_buffer && !m_buffer->deref())
        delete m_buffer;
    m_buffer = 0;
}

inline QGLIndexArray::QGLIndexArray() : m_buffer(0) {}

inline QGLIndexArray::QGLIndexArray(const QGLIndexArray& other)
    : m_data(other.m_data), m_buffer(other.m_buffer)
{
    if (m_buffer)
        m_buffer->ref();
}

inline QGLIndexArray::~QGLIndexArray()
{
    if (m_buffer && !m_buffer->deref())
        delete m_buffer;
}

inline QGLIndexArray& QGLIndexArray::operator=(const QGLIndexArray& other)
{
    m_data = other.m_data;
    if (m_buffer != other.m_buffer) {
        if (m_buffer && !m_buffer->deref())
            delete m_buffer;
        m_buffer = other.m_buffer;
        if (m_buffer)
            m_buffer->ref();
    }
    return *this;
}

inline int QGLIndexArray::size() const
{
    return m_data.count();
}

inline bool QGLIndexArray::isEmpty() const
{
    return m_data.isEmpty();
}

inline void QGLIndexArray::append(int value)
{
    detachBuffer();
    m_data.append(ElementType(value));
}

inline void QGLIndexArray::append(int value1, int value2)
{
    detachBuffer();
    m_data.append(ElementType(value1), ElementType(value2));
}

inline void QGLIndexArray::append(int value1, int value2, int value3)
{
    detachBuffer();
    m_data.append(ElementType(value1), ElementType(value2),
                  ElementType(value3));
}

inline void QGLIndexArray::append(int value1, int value2, int value3, int value4)
{
    detachBuffer();
    m_data.append(ElementType(value1), ElementType(value2),
                  ElementType(value3), ElementType(value4));
}

inline int QGLIndexArray::operator[](int index) const
{
    Q_ASSERT(index >= 0 && index < m_data.count());
    return m_data[index];
}

inline QGLIndexArray::ElementType& QGLIndexArray::operator[](int index)
{
    Q_ASSERT(index >= 0 && index < m_data.count());
    detachBuffer();
    return m_data[index];
}

inline int QGLIndexArray::type() const
{
#if defined(QT_OPENGL_ES)
    return 0x1403; // GL_UNSIGNED_SHORT
#else
    return 0x1405; // GL_UNSIGNED_INT
#endif
}

inline const QGLIndexArray::ElementType *QGLIndexArray::data() const
{
    if (isEmpty())
        return 0;
    else
        return m_data.constData();
}

inline const QGLIndexArray::ElementType *QGLIndexArray::constData() const
{
    if (isEmpty())
        return 0;
    else
        return m_data.constData();
}

inline QGLIndexArray& QGLIndexArray::operator+=(int value)
{
    append(value);
    return *this;
}

inline QGLIndexArray& QGLIndexArray::operator<<(int value)
{
    append(value);
    return *this;
}

inline QGLIndexArray QGLIndexArray::fromRawData(const uint *data, int count)
{
    return fromRawData(reinterpret_cast<const int *>(data), count);
}

inline bool QGLIndexArray::operator==(const QGLIndexArray &other) const
{
    return m_data == other.m_data;
}

inline bool QGLIndexArray::isUploaded() const
{
    return m_buffer != 0;
}

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLIndexArray &indices);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
