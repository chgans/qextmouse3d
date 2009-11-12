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

#ifndef QGLARRAYDATA_H
#define QGLARRAYDATA_H

#include "qt3dglobal.h"
#include <QtCore/qatomic.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

// Used internally by QGLVertexArray and QGLIndexArray.
// Only suitable for storing primitive types like int, float, etc.

template <typename T, int PreallocSize = 16>
class QGLArrayData
{
public:
    inline QGLArrayData();
    inline QGLArrayData(const QGLArrayData<T, PreallocSize>& other);
    inline ~QGLArrayData();

    inline QGLArrayData<T, PreallocSize>& operator=
        (const QGLArrayData<T, PreallocSize>& other);

    inline int size() const;
    inline int count() const;
    inline int capacity() const;

    inline bool isEmpty() const;

    inline const T& operator[](int index) const;
    inline T& operator[](int index);

    inline void append(T value);
    inline void append(T value1, T value2);
    inline void append(T value1, T value2, T value3);
    inline void append(T value1, T value2, T value3, T value4);
    inline void append(const T *values, int count);
    inline void append(const QGLArrayData<T, PreallocSize>& other);

    inline void replace(int index, const T *values, int count);

    inline void resize(int size);
    inline void reserve(int size);

    inline void setRawData(const T *values, int count);

    inline T *data();
    inline const T *data() const;
    inline const T *constData() const;

    QGLArrayData<T, PreallocSize> extract
        (int index, int size, int stride) const;
    QGLArrayData<T, PreallocSize> interleaved
        (int thisStride, const QGLArrayData<T, PreallocSize>& other,
         int otherStride) const;

    bool operator==(const QGLArrayData<T, PreallocSize> &other) const;
    bool operator!=(const QGLArrayData<T, PreallocSize> &other) const;

private:
    struct Data
    {
        QBasicAtomicInt ref;
        T *data;
        T *end;
        T *limit;
    };

    struct HeapData : public Data
    {
        T array[1];
    };

    struct PreallocData : public Data
    {
        T array[PreallocSize];
    };

    Data *m_data;
    PreallocData m_prealloc;

    void copyPrealloc(const PreallocData *prealloc);
    void grow(int needed);
    void detach();
};

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QGLArrayData<T, PreallocSize>::QGLArrayData()
{
    m_data = &m_prealloc;
    m_prealloc.ref = 1;
    m_prealloc.data = m_prealloc.array;
    m_prealloc.end = m_prealloc.array;
    m_prealloc.limit = m_prealloc.array + PreallocSize;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QGLArrayData<T, PreallocSize>::QGLArrayData(const QGLArrayData<T, PreallocSize>& other)
{
    if (other.m_data != &other.m_prealloc) {
        m_data = other.m_data;
        other.m_data->ref.ref();
    } else {
        copyPrealloc(&other.m_prealloc);
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QGLArrayData<T, PreallocSize>::~QGLArrayData()
{
    if (m_data != &m_prealloc && !m_data->ref.deref())
        qFree(m_data);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QGLArrayData<T, PreallocSize>& QGLArrayData<T, PreallocSize>::operator=(const QGLArrayData<T, PreallocSize>& other)
{
    if (m_data != other.m_data) {
        if (m_data != &m_prealloc && !m_data->ref.deref())
            qFree(m_data);
        if (other.m_data != &other.m_prealloc) {
            m_data = other.m_data;
            m_data->ref.ref();
        } else {
            copyPrealloc(&other.m_prealloc);
        }
    }
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QGLArrayData<T, PreallocSize>::size() const
{
    return m_data->end - m_data->data;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QGLArrayData<T, PreallocSize>::count() const
{
    return m_data->end - m_data->data;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QGLArrayData<T, PreallocSize>::capacity() const
{
    return m_data->limit - m_data->data;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE bool QGLArrayData<T, PreallocSize>::isEmpty() const
{
    return m_data->data == m_data->end;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T& QGLArrayData<T, PreallocSize>::operator[](int index) const
{
    return m_data->data[index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T& QGLArrayData<T, PreallocSize>::operator[](int index)
{
    if (m_data->ref != 1)
        detach();
    return m_data->data[index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::append(T value)
{
    if (m_data->end >= m_data->limit)
        grow(1);
    else if (m_data->ref != 1)
        detach();
    *((m_data->end)++) = value;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::append(T value1, T value2)
{
    if ((m_data->end + 2) > m_data->limit)
        grow(2);
    else if (m_data->ref != 1)
        detach();
    m_data->end[0] = value1;
    m_data->end[1] = value2;
    m_data->end += 2;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::append(T value1, T value2, T value3)
{
    if ((m_data->end + 3) > m_data->limit)
        grow(3);
    else if (m_data->ref != 1)
        detach();
    m_data->end[0] = value1;
    m_data->end[1] = value2;
    m_data->end[2] = value3;
    m_data->end += 3;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::append(T value1, T value2, T value3, T value4)
{
    if ((m_data->end + 4) > m_data->limit)
        grow(4);
    else if (m_data->ref != 1)
        detach();
    m_data->end[0] = value1;
    m_data->end[1] = value2;
    m_data->end[2] = value3;
    m_data->end[3] = value4;
    m_data->end += 4;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::append(const T *values, int count)
{
    if (count <= 0)
        return;
    if ((m_data->end + count) > m_data->limit)
        grow(count);
    else if (m_data->ref != 1)
        detach();
    qMemCopy(m_data->end, values, count * sizeof(T));
    m_data->end += count;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::append(const QGLArrayData<T, PreallocSize>& other)
{
    if (other.m_data->data == other.m_data->end) {
        // Appending an empty array.
        return;
    } else if (m_data->data == m_data->end) {
        // Appending to an empty array.
        *this = other;
    } else if (m_data == other.m_data) {
        // Appending the array to itself.
        int size = m_data->end - m_data->data;
        int limit = (m_data->limit - m_data->data) * 2;
        HeapData *data = reinterpret_cast<HeapData *>
            (qMalloc(sizeof(HeapData) + sizeof(T) * (limit - 1)));
        data->ref = 1;
        data->data = data->array;
        data->end = data->array + size * 2;
        data->limit = data->array + limit;
        qMemCopy(data->data, m_data->data, size * sizeof(T));
        qMemCopy(data->data + size, m_data->data, size * sizeof(T));
        if (m_data != &m_prealloc && !m_data->ref.deref())
            qFree(m_data);
        m_data = data;
    } else {
        // Appending another array to this one.
        append(other.constData(), other.m_data->end - other.m_data->data);
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::replace(int index, const T *values, int count)
{
    if (index < 0 || count <= 0)
        return;
    int replaceSize = index + count;
    int currentSize = m_data->end - m_data->data;
    if ((m_data->data + replaceSize) > m_data->limit)
        grow(replaceSize - currentSize);
    else if (m_data->ref != 1)
        detach();
    if (replaceSize > currentSize)
        m_data->end = m_data->data + replaceSize;
    qMemCopy(m_data->data + index, values, count * sizeof(T));
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::resize(int size)
{
    if (size < 0)
        return;
    reserve(size);
    int currentSize = m_data->end - m_data->data;
    m_data->end = m_data->data + size;
    if (currentSize < size) {
        qMemSet(m_data->data + currentSize, 0,
                sizeof(T) * (size - currentSize));
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::reserve(int size)
{
    if (size > 0 && (m_data->data + size) > m_data->limit)
        grow(size - (m_data->end - m_data->data));
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::setRawData(const T *values, int count)
{
    if (m_data != &m_prealloc && !m_data->ref.deref())
        qFree(m_data);
    m_prealloc.ref = 0;
    m_prealloc.data = const_cast<T *>(values);
    m_prealloc.end = m_prealloc.data + count;
    m_prealloc.limit = m_prealloc.end;
    m_data = &m_prealloc;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T *QGLArrayData<T, PreallocSize>::data()
{
    if (m_data->ref != 1)
        detach();
    return m_data->data;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T *QGLArrayData<T, PreallocSize>::data() const
{
    return m_data->data;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T *QGLArrayData<T, PreallocSize>::constData() const
{
    return m_data->data;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::copyPrealloc(const PreallocData *prealloc)
{
    if (prealloc->data == prealloc->array) {
        // We copy the preallocated data from the source onto the heap.
        // If it is copied once, it may be copied again, so reference
        // counting a pointer is more efficient over the long term.
        int size = prealloc->end - prealloc->data;
        int limit = prealloc->limit - prealloc->data;
        HeapData *data = reinterpret_cast<HeapData *>
            (qMalloc(sizeof(HeapData) + sizeof(T) * (limit - 1)));
        data->ref = 1;
        data->data = data->array;
        data->end = data->array + size;
        data->limit = data->array + limit;
        if (size > 0)
            qMemCopy(data->data, prealloc->data, size * sizeof(T));
        m_data = data;
    } else {
        // Copying a raw data array - don't make a copy on the heap.
        m_prealloc = *prealloc;
        m_data = &m_prealloc;
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::grow(int needed)
{
    // Determine the new size.
    int oldSize = m_data->end - m_data->data;
    int size = oldSize + needed;
    int twiceOldSize = oldSize * 2;
    if (size < twiceOldSize)
        size = twiceOldSize;
    if (size < PreallocSize)
        size = PreallocSize;

    // Allocate a new block on the heap and copy the data across.
    HeapData *data = reinterpret_cast<HeapData *>
        (qMalloc(sizeof(HeapData) + sizeof(T) * (size - 1)));
    data->ref = 1;
    data->data = data->array;
    data->end = data->array + oldSize;
    data->limit = data->array + size;
    if (oldSize > 0)
        qMemCopy(data->data, m_data->data, sizeof(T) * oldSize);

    // Free the old data block and switch to the new one.
    if (m_data != &m_prealloc && !m_data->ref.deref())
        qFree(m_data);
    m_data = data;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QGLArrayData<T, PreallocSize>::detach()
{
    // Allocate a new block on the heap and copy the data across.
    int size = m_data->end - m_data->data;
    int limit = m_data->limit - m_data->data;
    if (limit < PreallocSize)
        limit = PreallocSize;
    HeapData *data = reinterpret_cast<HeapData *>
        (qMalloc(sizeof(HeapData) + sizeof(T) * (limit - 1)));
    data->ref = 1;
    data->data = data->array;
    data->end = data->array + size;
    data->limit = data->array + limit;
    if (size > 0)
        qMemCopy(data->data, m_data->data, sizeof(T) * size);

    // Free the old data block and switch to the new one.
    if (m_data != &m_prealloc && !m_data->ref.deref())
        qFree(m_data);
    m_data = data;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE QGLArrayData<T, PreallocSize> QGLArrayData<T, PreallocSize>::extract
    (int index, int size, int stride) const
{
    Q_ASSERT((index + size) <= stride);
    QGLArrayData<T, PreallocSize> result;
    int count = (m_data->end - m_data->data) / stride;
    result.reserve(count * size);
    result.m_data->end = result.m_data->data + count * size;
    T *data = result.m_data->data;
    for (int item = 0; item < count; ++item) {
        for (int component = 0; component < size; ++component)
            data[component] = m_data->data[index + component];
        data += size;
        index += stride;
    }
    return result;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE QGLArrayData<T, PreallocSize> QGLArrayData<T, PreallocSize>::interleaved
    (int thisStride, const QGLArrayData<T, PreallocSize>& other, int otherStride) const
{
    QGLArrayData<T, PreallocSize> result;

    // Determine the size of the new array.
    int thisCount = (m_data->end - m_data->data) / thisStride;
    int otherCount = (other.m_data->end - other.m_data->data) / otherStride;
    int maxCount = qMax(thisCount, otherCount);
    int size = maxCount * (thisStride + otherStride);

    // Reserve space for the interleaved version.
    result.reserve(size);
    result.m_data->end = result.m_data->data + size;

    // Copy across the elements from the source arrays, padding with
    // zeroes if one of the source arrays is shorter than the other.
    const T *thisData = m_data->data;
    const T *otherData = other.m_data->data;
    T *data = result.m_data->data;
    int component;
    for (int index = 0; index < maxCount; ++index) {
        if (index < thisCount) {
            for (component = 0; component < thisStride; ++component)
                data[component] = thisData[component];
        } else {
            for (component = 0; component < thisStride; ++component)
                data[component] = T(0);
        }
        thisData += thisStride;
        data += thisStride;
        if (index < otherCount) {
            for (component = 0; component < otherStride; ++component)
                data[component] = otherData[component];
        } else {
            for (component = 0; component < otherStride; ++component)
                data[component] = T(0);
        }
        otherData += otherStride;
        data += otherStride;
    }
    return result;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE bool QGLArrayData<T, PreallocSize>::operator==
    (const QGLArrayData<T, PreallocSize> &other) const
{
    if (this == &other)
        return true;
    const T *thisData = m_data->data;
    const T *otherData = other.m_data->data;
    if (thisData == otherData)
        return true;
    int thisCount = m_data->end - m_data->data;
    int otherCount = other.m_data->end - other.m_data->data;
    if (thisCount == 0 && otherCount == 0)
        return true;
    if (thisCount != otherCount)
        return false;
    for (int index = 0; index < thisCount; ++index, ++thisData, ++otherData)
        if (*thisData != *otherData)
            return false;
    return true;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE bool QGLArrayData<T, PreallocSize>::operator!=
    (const QGLArrayData<T, PreallocSize> &other) const
{
    if (this == &other)
        return false;
    const T *thisData = m_data->data;
    const T *otherData = other.m_data->data;
    if (thisData == otherData)
        return false;
    int thisCount = m_data->end - m_data->data;
    int otherCount = other.m_data->end - other.m_data->data;
    if (thisCount == 0 && otherCount == 0)
        return false;
    if (thisCount != otherCount)
        return true;
    for (int index = 0; index < thisCount; ++index, ++thisData, ++otherData)
        if (*thisData == *otherData)
            return false;
    return true;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
