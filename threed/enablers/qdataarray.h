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
        int used;
        int capacity;
        T array[1];
    };

    // If m_start is null, then it indicates that the data is in
    // m_data and the reference count is greater than or equal to 1.
    // If m_start is not null, then the data is either in m_prealloc
    // or m_data with a reference count of 1.
    // This supports fast append() when the array is not shared,
    // and a quick way (m_start == NULL) to determine if the
    // array may need to be copied on write.
    mutable T *m_start;
    mutable T *m_end;
    mutable T *m_limit;
    mutable Data *m_data;
    union {
        char m_prealloc[sizeof(T) * PreallocSize];
        qint64 q_for_alignment_1;
        double q_for_alignment_2;
    };

    void free();
    void reallocate(int capacity);
    void detachForWrite(int needed = 0);
    void detachForCopy(int needed = 0) const;
    void grow(int needed);
};

extern int qAllocMore(int alloc, int extra); // in qbytearray.cpp

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::free()
{
    if (m_data && !m_data->ref.deref())
        qFree(m_data);
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::reallocate(int capacity)
{
    Data *data = reinterpret_cast<Data *>
        (qRealloc(m_data, sizeof(Data) + sizeof(T) * (capacity - 1)));
    Q_CHECK_PTR(data);
    m_data = data;
    m_data->capacity = capacity;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::detachForWrite(int needed)
{
    // Allocate a new block on the heap and copy the data across.
    int oldSize = m_data->used;
    int size = needed ? (qAllocMore(oldSize, needed) + needed) : oldSize;
    Data *data = reinterpret_cast<Data *>
        (qMalloc(sizeof(Data) + sizeof(T) * (size - 1)));
    Q_CHECK_PTR(data);
    data->ref = 1;
    data->used = oldSize;
    data->capacity = size;
    if (oldSize > 0)
        qMemCopy(data->array, m_data->array, oldSize * sizeof(T));
    m_data->ref.deref();
    m_data = data;

    // Update the append pointers for faster future updates.
    m_start = m_data->array;
    m_end = m_start + m_data->used;
    m_limit = m_start + m_data->capacity;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::detachForCopy(int needed) const
{
    if (m_data) {
        // Data is already on the heap: update the used size.
        m_data->used = m_end - m_start;
    } else {
        // Copy preallocated data to the heap.
        int capacity = qAllocMore(m_limit - m_start, needed) + needed;
        Data *data = reinterpret_cast<Data *>
            (qMalloc(sizeof(Data) + sizeof(T) * (capacity - 1)));
        Q_CHECK_PTR(data);
        m_data = data;
        m_data->ref = 1;
        m_data->used = m_end - m_start;
        m_data->capacity = capacity;
        qMemCopy(m_data->array, m_start, m_data->used * sizeof(T));
    }

    // Force copy-on-write the next time an append is done.
    m_start = 0;
    m_end = 0;
    m_limit = 0;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::grow(int needed)
{
    // Make sure the data is in the heap, up to date, and not shared.
    if (m_start) {
        if (needed <= (m_limit - m_end))
            return; // There is enough capacity already.
        if (m_data)
            m_data->used = m_end - m_start;
        else
            detachForCopy(needed);
    } else if (m_data->ref != 1) {
        detachForWrite(needed);
    }

    // Reallocate the array if necessary.
    if ((m_data->used + needed) > m_data->capacity) {
        int size = m_data->used;
        int capacity = qAllocMore(size, needed) + needed;
        reallocate(capacity);
    }

    // Update the append pointers for faster future updates.
    m_start = m_data->array;
    m_end = m_start + m_data->used;
    m_limit = m_start + m_data->capacity;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::QDataArray()
{
    m_start = reinterpret_cast<T *>(m_prealloc);
    m_end = m_start;
    m_limit = m_start + PreallocSize;
    m_data = 0;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::QDataArray(const QDataArray<T, PreallocSize>& other)
{
    if (other.m_start)
        other.detachForCopy();
    m_start = 0;
    m_end = 0;
    m_limit = 0;
    m_data = other.m_data;
    m_data->ref.ref();
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::~QDataArray()
{
    free();
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>& QDataArray<T, PreallocSize>::operator=(const QDataArray<T, PreallocSize>& other)
{
    if (other.m_start) {
        if (other.m_start != other.m_end) {
            other.detachForCopy();
        } else {
            clear();
            return *this;
        }
    } else if (other.m_data == m_data) {
        return *this;
    }
    free();
    m_start = 0;
    m_end = 0;
    m_limit = 0;
    m_data = other.m_data;
    m_data->ref.ref();
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QDataArray<T, PreallocSize>::size() const
{
    if (m_start)
        return m_end - m_start;
    else
        return m_data->used;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QDataArray<T, PreallocSize>::count() const
{
    if (m_start)
        return m_end - m_start;
    else
        return m_data->used;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QDataArray<T, PreallocSize>::capacity() const
{
    if (m_start)
        return m_limit - m_start;
    else
        return m_data->capacity;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE bool QDataArray<T, PreallocSize>::isEmpty() const
{
    if (m_start)
        return m_start == m_end;
    else
        return m_data->used == 0;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::clear()
{
    free();
    m_start = reinterpret_cast<T *>(m_prealloc);
    m_end = m_start;
    m_limit = m_start + PreallocSize;
    m_data = 0;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T& QDataArray<T, PreallocSize>::operator[](int index) const
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QDataArray<T>::at", "index out of range");
    if (m_start)
        return m_start[index];
    else
        return m_data->array[index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T& QDataArray<T, PreallocSize>::at(int index) const
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QDataArray<T>::operator[]", "index out of range");
    if (m_start)
        return m_start[index];
    else
        return m_data->array[index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T& QDataArray<T, PreallocSize>::operator[](int index)
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QDataArray<T>::operator[]", "index out of range");
    if (m_start) {
        return m_start[index];
    } else if (m_data->ref == 1) {
        return m_data->array[index];
    } else {
        detachForWrite();
        return m_data->array[index];
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(T value)
{
    if (m_end >= m_limit)
        grow(1);
    *m_end++ = value;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const T *values, int count)
{
    if (count <= 0)
        return;
    if (!m_start || (m_end + count) > m_limit)
        grow(count);
    qMemCopy(m_end, values, count * sizeof(T));
    m_end += count;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const QDataArray<T, PreallocSize>& other)
{
    if (&other == this || (!m_start && other.m_data == m_data))
        grow(size());   // Appending to ourselves: make some room.
    append(other.constData(), other.size());
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::replace(int index, const T *values, int count)
{
    if (index < 0 || count <= 0)
        return;
    int replaceSize = index + count;
    if (replaceSize > size())
        resize(replaceSize);
    qMemCopy(data() + index, values, count * sizeof(T));
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::remove(int index)
{
    remove(index, 1);
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::remove(int index, int count)
{
    // Truncate the range to be removed.
    int currentSize = size();
    if (index < 0) {
        count += index;
        index = 0;
    }
    if (count > 0 && (index + count) > currentSize)
        count = currentSize - index;
    if (count <= 0)
        return;

    // Perform the removal.
    if (index == 0 && count >= currentSize) {
        clear();
        return;
    }
    if (m_start) {
        qMemMove(m_start + index, m_start + index + count,
                 (currentSize - (index + count)) * sizeof(T));
    } else {
        if (m_data->ref != 1)
            detachForWrite();
        qMemMove(m_data->array + index, m_data->array + index + count,
                 (currentSize - (index + count)) * sizeof(T));
    }
    resize(currentSize - count);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::resize(int size)
{
    if (size < 0)
        return;
    int currentSize = count();
    if (size < currentSize) {
        if (m_start) {
            //deleteElements(m_start + size, m_end);
            m_end = m_start + size;
        } else {
            if (m_data->ref != 1)
                detachForWrite();
            //deleteElements(m_data->array + size, m_data->array + m_data->used);
            m_data->used = size;
        }
    } else if (size > currentSize) {
        grow(size - currentSize);
        qMemSet(m_start + currentSize, 0, sizeof(T) * (size - currentSize));
        m_end = m_start + size;
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::reserve(int size)
{
    int cap = capacity();
    if (size > 0 && size < capacity)
        grow(size - capacity);
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::shrink(int size)
{
    if (size <= 0) {
        clear();
        return;
    }

    // Bail out if the capacity is already smaller than the shrink size.
    if (size >= capacity())
        return;

    // Delete unneeded elements.
    if (m_start) {
        if (size < (m_end - m_start))
            m_end = m_start + size;
    } else {
        if (m_data->ref != 1)
            detachForWrite();
        if (size < m_data->used)
            m_data->used = size;
    }

    // If the array is in the preallocated area, then no point shrinking.
    if (!m_data)
        return;

    // Reallocate the array on the heap to the smaller size.
    if (m_start)
        m_data->used = m_end - m_start;
    reallocate(size);

    // Force copy-on-write the next time an append is done.
    m_start = 0;
    m_end = 0;
    m_limit = 0;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::squeeze()
{
    shrink(size());
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T *QDataArray<T, PreallocSize>::data()
{
    if (m_data && m_data->ref != 1)
        detachForWrite();
    if (m_start)
        return m_start;
    else
        return m_data->array;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T *QDataArray<T, PreallocSize>::data() const
{
    if (m_start)
        return m_start;
    else
        return m_data->array;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T *QDataArray<T, PreallocSize>::constData() const
{
    if (m_start)
        return m_start;
    else
        return m_data->array;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE bool QDataArray<T, PreallocSize>::operator==
    (const QDataArray<T, PreallocSize> &other) const
{
    if (this == &other)
        return true;
    const T *thisData = constData();
    const T *otherData = other.constData();
    if (thisData == otherData)
        return true;
    int thisCount = count();
    int otherCount = other.count();
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
Q_INLINE_TEMPLATE bool QDataArray<T, PreallocSize>::operator!=
    (const QDataArray<T, PreallocSize> &other) const
{
    return !(*this == other);
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
