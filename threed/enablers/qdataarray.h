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

#ifndef QDATAARRAY_H
#define QDATAARRAY_H

#include <QtCore/qglobal.h>
#include <QtCore/qatomic.h>
#include <QtCore/qalgorithms.h>

#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

template <typename T, int PreallocSize = 8>
class QDataArray
{
public:
    QDataArray();
    QDataArray(int size, const T& value);
    QDataArray(const QDataArray<T, PreallocSize>& other);
    ~QDataArray();

    QDataArray<T, PreallocSize>& operator=
        (const QDataArray<T, PreallocSize>& other);

    int size() const;
    int count() const;
    int capacity() const;

    bool isEmpty() const;

    void clear();
    void reset();

    const T& at(int index) const;
    const T& operator[](int index) const;
    T& operator[](int index);

    void append(T value);
    void append(const T *values, int count);
    void append(const QDataArray<T, PreallocSize>& other);
    void replace(int index, const T *values, int count);
    void remove(int index);
    void remove(int index, int count);

    void resize(int size);
    void reserve(int size);
    void shrink(int size);
    void squeeze();

    T *data();
    const T *data() const;
    const T *constData() const;

    bool operator==(const QDataArray<T, PreallocSize> &other) const;
    bool operator!=(const QDataArray<T, PreallocSize> &other) const;

    QDataArray<T, PreallocSize>& operator+=(const T& value);
    QDataArray<T, PreallocSize>& operator+=(const QDataArray<T, PreallocSize>& array);
    QDataArray<T, PreallocSize>& operator<<(const T& value);
    QDataArray<T, PreallocSize>& operator<<(const QDataArray<T, PreallocSize>& array);

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
        union {
            char array[sizeof(T) * PreallocSize];
            qint64 q_for_alignment_1;
            double q_for_alignment_2;
        };
    };

    Data *m_data;
    PreallocData m_prealloc;

    void deleteElements();
    void free();
    void copyPrealloc(const PreallocData *prealloc);
    void grow(int needed);
    void detach();
};

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::deleteElements()
{
    if (QTypeInfo<T>::isComplex) {
        T *data = m_data->data;
        while (data != m_data->end) {
            data->~T();
            ++data;
        }
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::free()
{
    if (m_data == &m_prealloc) {
        deleteElements();
    } else if (!m_data->ref.deref()) {
        deleteElements();
        qFree(m_data);
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::QDataArray()
{
    m_data = &m_prealloc;
    m_prealloc.ref = 1;
    m_prealloc.data = reinterpret_cast<T *>(m_prealloc.array);
    m_prealloc.end = m_prealloc.data;
    m_prealloc.limit = m_prealloc.data + PreallocSize;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::QDataArray(const QDataArray<T, PreallocSize>& other)
{
    if (other.m_data != &other.m_prealloc) {
        m_data = other.m_data;
        other.m_data->ref.ref();
    } else {
        copyPrealloc(&other.m_prealloc);
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::~QDataArray()
{
    free();
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>& QDataArray<T, PreallocSize>::operator=(const QDataArray<T, PreallocSize>& other)
{
    if (m_data != other.m_data) {
        free();
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
Q_INLINE_TEMPLATE int QDataArray<T, PreallocSize>::size() const
{
    return m_data->end - m_data->data;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QDataArray<T, PreallocSize>::count() const
{
    return m_data->end - m_data->data;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QDataArray<T, PreallocSize>::capacity() const
{
    return m_data->limit - m_data->data;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE bool QDataArray<T, PreallocSize>::isEmpty() const
{
    return m_data->data == m_data->end;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::clear()
{
    free();
    m_data = &m_prealloc;
    m_prealloc.ref = 1;
    m_prealloc.data = reinterpret_cast<T *>(m_prealloc.array);
    m_prealloc.end = m_prealloc.data;
    m_prealloc.limit = m_prealloc.data + PreallocSize;
}

// Similar to clear(), but keeps the same memory if possible.
template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::reset()
{
    if (m_data->ref == 1) {
        deleteElements();
        m_data->end = m_data->data;
    } else {
        clear();
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T& QDataArray<T, PreallocSize>::operator[](int index) const
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QDataArray<T>::at", "index out of range");
    return m_data->data[index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T& QDataArray<T, PreallocSize>::at(int index) const
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QDataArray<T>::operator[]", "index out of range");
    return m_data->data[index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T& QDataArray<T, PreallocSize>::operator[](int index)
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QDataArray<T>::operator[]", "index out of range");
    if (m_data->ref != 1)
        detach();
    return m_data->data[index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(T value)
{
    if (m_data->end >= m_data->limit)
        grow(1);
    else if (m_data->ref != 1)
        detach();
    if (QTypeInfo<T>::isComplex)
        new ((m_data->end)++) T(value);
    else
        *((m_data->end)++) = value;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const T *values, int count)
{
    if (count <= 0)
        return;
    if ((m_data->end + count) > m_data->limit)
        grow(count);
    else if (m_data->ref != 1)
        detach();
    if (QTypeInfo<T>::isComplex) {
        while (count-- > 0)
            new ((m_data->end)++) T(*values++);
    } else {
        qMemCopy(m_data->end, values, count * sizeof(T));
        m_data->end += count;
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const QDataArray<T, PreallocSize>& other)
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
        if (QTypeInfo<T>::isStatic) {
            qMemCopy(data->data, m_data->data, size * sizeof(T));
            qMemCopy(data->data + size, m_data->data, size * sizeof(T));
        } else {
            qCopy(m_data->data, m_data->end, data->data);
            qCopy(m_data->data, m_data->end, data->data + size);
        }
        free();
        m_data = data;
    } else {
        // Appending another array to this one.
        append(other.constData(), other.m_data->end - other.m_data->data);
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::replace(int index, const T *values, int count)
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
    if (QTypeInfo<T>::isStatic)
        qMemCopy(m_data->data + index, values, count * sizeof(T));
    else
        qCopy(values, values + count, m_data->data + index);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::remove(int index)
{
    remove(index, 1);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::remove(int index, int count)
{
    int currentSize = size();
    if (index < 0) {
        count += index;
        index = 0;
    }
    if (count > 0 && (index + count) > currentSize)
        count = currentSize - index;
    if (count <= 0)
        return;
    if (m_data->ref != 1)
        detach();
    if (QTypeInfo<T>::isComplex) {
        qCopy(m_data->data + index + count, m_data->end, m_data->data + index);
        T *data = m_data->data + currentSize - count;
        while (data != m_data->end) {
            data->~T();
            ++data;
        }
    } else {
        int leftOver = currentSize - (index + count);
        if (leftOver > 0) {
            qMemMove(m_data->data + index, m_data->data + index + count,
                     leftOver * sizeof(T));
        }
    }
    m_data->end = m_data->data + currentSize - count;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::resize(int size)
{
    if (size < 0)
        return;
    reserve(size);
    int currentSize = m_data->end - m_data->data;
    m_data->end = m_data->data + size;
    if (currentSize >= size)
        return;
    if (QTypeInfo<T>::isComplex) {
        qFill(m_data->data + currentSize, m_data->data + size, T());
    } else {
        qMemSet(m_data->data + currentSize, 0,
                sizeof(T) * (size - currentSize));
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::reserve(int size)
{
    if (size > 0 && (m_data->data + size) > m_data->limit)
        grow(size - (m_data->end - m_data->data));
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::shrink(int size)
{
    if (size <= 0) {
        clear();
    } else {
        int capacity = m_data->limit - m_data->data;
        int oldSize = m_data->end - m_data->data;
        if (size < oldSize) {
            // Destroy the elements we will be discarding.
            if (QTypeInfo<T>::isComplex) {
                T *values = m_data->data + size;
                while (values != m_data->end) {
                    values->~T();
                    ++values;
                }
            }
            m_data->end = m_data->data + size;
            oldSize = size;
        }
        if (size < capacity) {
            // Allocate a new block on the heap and copy the data across.
            HeapData *data = reinterpret_cast<HeapData *>
                (qMalloc(sizeof(HeapData) + sizeof(T) * (size - 1)));
            data->ref = 1;
            data->data = data->array;
            data->end = data->array + size;
            data->limit = data->array + size;
            if (size > 0) {
                if (QTypeInfo<T>::isStatic) {
                    qMemCopy(data->data, m_data->data, sizeof(T) * size);
                } else {
                    const T *src = m_data->data;
                    T *dst = data->data;
                    while (size-- > 0)
                        new (dst++) T(*src++);
                }
            }

            // Free the old data block and switch to the new one.
            free();
            m_data = data;
        }
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::squeeze()
{
    shrink(size());
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T *QDataArray<T, PreallocSize>::data()
{
    if (m_data->ref != 1)
        detach();
    return m_data->data;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T *QDataArray<T, PreallocSize>::data() const
{
    return m_data->data;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T *QDataArray<T, PreallocSize>::constData() const
{
    return m_data->data;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::copyPrealloc(const PreallocData *prealloc)
{
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
    if (size > 0) {
        if (QTypeInfo<T>::isStatic) {
            qMemCopy(data->data, prealloc->data, size * sizeof(T));
        } else {
            const T *src = prealloc->data;
            T *dst = data->data;
            while (size-- > 0)
                new (dst++) T(*src++);
        }
    }
    m_data = data;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::grow(int needed)
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
    if (oldSize > 0) {
        if (QTypeInfo<T>::isStatic) {
            qMemCopy(data->data, m_data->data, sizeof(T) * oldSize);
        } else {
            const T *src = m_data->data;
            T *dst = data->data;
            while (oldSize-- > 0)
                new (dst++) T(*src++);
        }
    }

    // Free the old data block and switch to the new one.
    free();
    m_data = data;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::detach()
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
    if (size > 0) {
        if (QTypeInfo<T>::isStatic) {
            qMemCopy(data->data, m_data->data, sizeof(T) * size);
        } else {
            const T *src = m_data->data;
            T *dst = data->data;
            while (size-- > 0)
                new (dst++) T(*src++);
        }
    }

    // Free the old data block and switch to the new one.
    free();
    m_data = data;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE bool QDataArray<T, PreallocSize>::operator==
    (const QDataArray<T, PreallocSize> &other) const
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
Q_OUTOFLINE_TEMPLATE bool QDataArray<T, PreallocSize>::operator!=
    (const QDataArray<T, PreallocSize> &other) const
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

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>& QDataArray<T, PreallocSize>::operator+=(const T& value)
{
    append(value);
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>& QDataArray<T, PreallocSize>::operator+=(const QDataArray<T, PreallocSize>& array)
{
    append(array);
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>& QDataArray<T, PreallocSize>::operator<<(const T& value)
{
    append(value);
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>& QDataArray<T, PreallocSize>::operator<<(const QDataArray<T, PreallocSize>& array)
{
    append(array);
    return *this;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
