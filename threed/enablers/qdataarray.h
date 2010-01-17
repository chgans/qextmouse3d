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
#include <string.h>

#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

template <typename T, int PreallocSize>
class QDataArrayRef;

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

    T *extend(int size);

    void append(const T& value);
    void append(const T& value1, const T& value2);
    void append(const T& value1, const T& value2, const T& value3);
    void append(const T& value1, const T& value2, const T& value3, const T& value4);
    void append(const T *values, int count);
    void append(const QDataArray<T, PreallocSize>& other);
    void replace(int index, const T *values, int count);
    void remove(int index);
    void remove(int index, int count);

    void resize(int size);
    void reserve(int size);
    void shrink(int size);

    QDataArrayRef<T, PreallocSize> mid(int index, int length = -1) const;
    QDataArrayRef<T, PreallocSize> left(int length) const;
    QDataArrayRef<T, PreallocSize> right(int length) const;

    T *data();
    const T *data() const;
    const T *constData() const;

    static QDataArray<T, PreallocSize> fromRawData(const T *data, int size);

    bool operator==(const QDataArray<T, PreallocSize> &other) const;
    bool operator!=(const QDataArray<T, PreallocSize> &other) const;

    QDataArray<T, PreallocSize>& operator+=(const T& value);
    QDataArray<T, PreallocSize>& operator+=(const QDataArray<T, PreallocSize>& other);
    QDataArray<T, PreallocSize>& operator<<(const T& value);
    QDataArray<T, PreallocSize>& operator<<(const QDataArray<T, PreallocSize>& other);

private:
    struct Data
    {
        QBasicAtomicInt ref;
        int used;
        int capacity;
        T *array;   // Usually points to "data" unless a raw data array.
        T data[1];
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

    QDataArray(const T *data, int size);

    void reallocate(int capacity);
    void detachForWrite(int needed = 0);
    void detachForCopy(int needed = 0) const;
    void grow(int needed);
};

template <typename T, int PreallocSize = 8>
class QDataArrayRef
{
public:
    QDataArrayRef();
    explicit QDataArrayRef(QDataArray<T, PreallocSize> *array);
    QDataArrayRef(QDataArray<T, PreallocSize> *array, int offset, int size);

    bool isNull() const { return m_array == 0; }

    int size() const { return m_size; }
    int count() const { return m_size; }
    bool isEmpty() const { return m_size == 0; }

    const T& at(int index) const;
    const T& operator[](int index) const;
    T& operator[](int index);

    T *data();
    const T *data() const;
    const T *constData() const;

    QDataArray<T, PreallocSize> *dataArray() const { return m_array; }
    int offset() const { return m_offset; }

    QDataArray<T, PreallocSize> toDataArray() const;

    bool operator==(const QDataArrayRef<T, PreallocSize> &other) const;
    bool operator!=(const QDataArrayRef<T, PreallocSize> &other) const;

private:
    QDataArray<T, PreallocSize> *m_array;
    int m_offset, m_size;
};

int Q_QT3D_EXPORT qDataArrayAllocMore(int alloc, int extra);

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::reallocate(int capacity)
{
    if (m_data->array == m_data->data) {
        Data *data = reinterpret_cast<Data *>
            (qRealloc(m_data, sizeof(Data) + sizeof(T) * (capacity - 1)));
        Q_CHECK_PTR(data);
        m_data = data;
        m_data->capacity = capacity;
        m_data->array = m_data->data;
    } else {
        // Reallocating raw data: copy it to the heap.
        T *array = m_data->array;
        Data *data = reinterpret_cast<Data *>
            (qRealloc(m_data, sizeof(Data) + sizeof(T) * (capacity - 1)));
        Q_CHECK_PTR(data);
        m_data = data;
        m_data->capacity = capacity;
        m_data->array = m_data->data;
        if (m_data->used > 0)
            qMemCopy(m_data->array, array, m_data->used * sizeof(T));
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::detachForWrite(int needed)
{
    // Allocate a new block on the heap and copy the data across.
    int oldSize = m_data->used;
    int size = qDataArrayAllocMore(oldSize, needed);
    Data *data = reinterpret_cast<Data *>
        (qMalloc(sizeof(Data) + sizeof(T) * (size - 1)));
    Q_CHECK_PTR(data);
    data->ref = 1;
    data->used = oldSize;
    data->capacity = size;
    data->array = data->data;
    if (oldSize > 0)
        qMemCopy(data->array, m_data->array, oldSize * sizeof(T));
    if (!m_data->ref.deref())
        delete m_data;  // May be last reference if it contained raw data.
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
        int capacity = qDataArrayAllocMore(m_limit - m_start, needed);
        Data *data = reinterpret_cast<Data *>
            (qMalloc(sizeof(Data) + sizeof(T) * (capacity - 1)));
        Q_CHECK_PTR(data);
        m_data = data;
        m_data->ref = 1;
        m_data->used = m_end - m_start;
        m_data->capacity = capacity;
        m_data->array = m_data->data;
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
    } else if (m_data->ref != 1 || m_data->array != m_data->data) {
        detachForWrite(needed);
    }

    // Reallocate the array if necessary.
    if ((m_data->used + needed) > m_data->capacity) {
        int size = m_data->used;
        int capacity = qDataArrayAllocMore(size, needed);
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
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::QDataArray(int size, const T& value)
{
    if (size <= PreallocSize) {
        m_start = reinterpret_cast<T *>(m_prealloc);
        m_end = m_start;
        m_limit = m_start + PreallocSize;
        m_data = 0;
    } else {
        int capacity = qDataArrayAllocMore(size, 0);
        Data *data = reinterpret_cast<Data *>
            (qMalloc(sizeof(Data) + sizeof(T) * (capacity - 1)));
        Q_CHECK_PTR(data);
        m_data = data;
        m_data->ref = 1;
        m_data->used = size;
        m_data->capacity = capacity;
        m_data->array = m_data->data;
        m_start = m_data->array;
        m_end = m_start;
        m_limit = m_start + capacity;
    }
    while (size-- > 0)
        *m_end++ = value;
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
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::QDataArray(const T *data, int size)
{
    // Constructing a raw data array.
    if (size > 0) {
        Data *newData = reinterpret_cast<Data *>(qMalloc(sizeof(Data)));
        Q_CHECK_PTR(newData);
        newData->ref = 1;
        newData->used = size;
        newData->capacity = size;
        newData->array = const_cast<T *>(data);
        m_start = 0;
        m_end = 0;
        m_limit = 0;
        m_data = newData;
    } else {
        m_start = reinterpret_cast<T *>(m_prealloc);
        m_end = m_start;
        m_limit = m_start + PreallocSize;
        m_data = 0;
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::~QDataArray()
{
    if (m_data && !m_data->ref.deref())
        qFree(m_data);
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
    if (m_data && !m_data->ref.deref())
        qFree(m_data);
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
    if (m_data && !m_data->ref.deref())
        qFree(m_data);
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
    } else if (m_data->ref == 1 && m_data->array == m_data->data) {
        return m_data->array[index];
    } else {
        detachForWrite();
        return m_data->array[index];
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T *QDataArray<T, PreallocSize>::extend(int size)
{
    Q_ASSERT(size > 0);
    if ((m_end + size) >= m_limit)
        grow(size);
    T *end = m_end;
    m_end += size;
    return end;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const T& value)
{
    if (m_end >= m_limit)
        grow(1);
    *m_end++ = value;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const T& value1, const T& value2)
{
    if ((m_end + 1) >= m_limit)
        grow(2);
    m_end[0] = value1;
    m_end[1] = value2;
    m_end += 2;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const T& value1, const T& value2, const T& value3)
{
    if ((m_end + 2) >= m_limit)
        grow(3);
    m_end[0] = value1;
    m_end[1] = value2;
    m_end[2] = value3;
    m_end += 3;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const T& value1, const T& value2, const T& value3, const T& value4)
{
    if ((m_end + 3) >= m_limit)
        grow(3);
    m_end[0] = value1;
    m_end[1] = value2;
    m_end[2] = value3;
    m_end[3] = value4;
    m_end += 4;
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
    if (m_data && (m_data->ref != 1 || m_data->array != m_data->data))
        detachForWrite();
    if (m_start) {
        ::memmove(m_start + index, m_start + index + count,
                  (currentSize - (index + count)) * sizeof(T));
    } else {
        ::memmove(m_data->array + index, m_data->array + index + count,
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
            m_end = m_start + size;
        } else {
            if (m_data->ref != 1 || m_data->array != m_data->data)
                detachForWrite();
            if (m_start)
                m_end = m_start + size;
            else
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
    if (size > cap)
        grow(size - this->size());
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
        if (m_data->ref != 1 || m_data->array != m_data->data) {
            detachForWrite();
            if (size < m_data->used)
                m_end = m_start + size;
        } else {
            if (size < m_data->used)
                m_data->used = size;
        }
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
Q_INLINE_TEMPLATE QDataArrayRef<T, PreallocSize> QDataArray<T, PreallocSize>::mid(int index, int length) const
{
    return QDataArrayRef<T, PreallocSize>
        (const_cast<QDataArray<T, PreallocSize> *>(this), index, length);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArrayRef<T, PreallocSize> QDataArray<T, PreallocSize>::left(int length) const
{
    return QDataArrayRef<T, PreallocSize>
        (const_cast<QDataArray<T, PreallocSize> *>(this), 0, length);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArrayRef<T, PreallocSize> QDataArray<T, PreallocSize>::right(int length) const
{
    int size = count();
    if (length < 0 || length >= size)
        length = size;
    return QDataArrayRef<T, PreallocSize>
        (const_cast<QDataArray<T, PreallocSize> *>(this),
         size - length, length);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T *QDataArray<T, PreallocSize>::data()
{
    if (m_start) {
        return m_start;
    } else {
        if (m_data->ref != 1 || m_data->array != m_data->data)
            detachForWrite();
        return m_data->array;
    }
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
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize> QDataArray<T, PreallocSize>::fromRawData(const T *data, int size)
{
    return QDataArray<T, PreallocSize>(data, size);
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
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>& QDataArray<T, PreallocSize>::operator+=(const QDataArray<T, PreallocSize>& other)
{
    append(other);
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>& QDataArray<T, PreallocSize>::operator<<(const T& value)
{
    append(value);
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>& QDataArray<T, PreallocSize>::operator<<(const QDataArray<T, PreallocSize>& other)
{
    append(other);
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArrayRef<T, PreallocSize>::QDataArrayRef()
    : m_array(0), m_offset(0), m_size(0) {}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArrayRef<T, PreallocSize>::QDataArrayRef(QDataArray<T, PreallocSize> *array)
    : m_array(array), m_offset(0), m_size(array ? array->size() : 0) {}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArrayRef<T, PreallocSize>::QDataArrayRef(QDataArray<T, PreallocSize> *array, int offset, int size)
    : m_array(array), m_offset(offset)
{
    Q_ASSERT(array);
    int count = array->size();
    Q_ASSERT(offset >= 0 && offset <= count);
    if (size < 0 || (offset + size) > count)
        size = count - offset;
    m_size = size;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T& QDataArrayRef<T, PreallocSize>::at(int index) const
{
    Q_ASSERT(m_array && index >= 0 && index < m_size);
    return m_array->at(m_offset + index);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T& QDataArrayRef<T, PreallocSize>::operator[](int index) const
{
    Q_ASSERT(m_array && index >= 0 && index < m_size);
    return m_array->at(m_offset + index);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T& QDataArrayRef<T, PreallocSize>::operator[](int index)
{
    Q_ASSERT(m_array && index >= 0 && index < m_size);
    return (*m_array)[m_offset + index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T *QDataArrayRef<T, PreallocSize>::data()
{
    if (m_array)
        return m_array->data() + m_offset;
    else
        return 0;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T *QDataArrayRef<T, PreallocSize>::data() const
{
    if (m_array)
        return m_array->constData() + m_offset;
    else
        return 0;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T *QDataArrayRef<T, PreallocSize>::constData() const
{
    if (m_array)
        return m_array->constData() + m_offset;
    else
        return 0;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE QDataArray<T, PreallocSize> QDataArrayRef<T, PreallocSize>::toDataArray() const
{
    if (!m_array) {
        return QDataArray<T, PreallocSize>();
    } else if (m_offset == 0 && m_size == m_array->size()) {
        return *m_array;
    } else {
        QDataArray<T, PreallocSize> result;
        if (m_size > 0) {
            T *dst = result.extend(m_size);
            qMemCopy(dst, m_array->constData() + m_offset,
                     sizeof(T) * m_size);
        }
        return result;
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE bool QDataArrayRef<T, PreallocSize>::operator==(const QDataArrayRef<T, PreallocSize> &other) const
{
    if (m_size != other.m_size)
        return false;
    else if (!m_array)
        return other.m_size == 0;
    else if (!other.m_array)
        return m_size == 0;
    else if (this == &other)
        return true;
    for (int index = 0; index < m_size; ++index) {
        if (m_array->at(m_offset + index) !=
                other.m_array->at(other.m_offset + index))
            return false;
    }
    return true;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE bool QDataArrayRef<T, PreallocSize>::operator!=(const QDataArrayRef<T, PreallocSize> &other) const
{
    return !(*this == other);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
