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
#include <new>

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
    bool isDetached() const;

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
    void squeeze();
    void squeeze(int size);

    void reverse();
    QDataArray<T, PreallocSize> reversed() const;

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

    typedef T *iterator;
    typedef const T *const_iterator;
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef ptrdiff_t difference_type;
    typedef int size_type;

    inline iterator begin() { return data(); }
    inline const_iterator begin() const { return constData(); }
    inline const_iterator constBegin() const { return constData(); }
    inline iterator end() { return data() + size(); }
    inline const_iterator end() const { return constData() + size(); }
    inline const_iterator constEnd() const { return constData() + size(); }

private:
    struct Data
    {
        QBasicAtomicInt ref;
        int reserved;   // For future use as flag bits.
        T array[1];
    };

    // Invariants:
    // 1. If the data is not shared, then m_appendLimit >= m_end.
    // 2. If the data is shared, then m_appendLimit == m_end, even if
    // that is less than the true capacity.  This triggers the range
    // check in append() to call grow(), which will copy-on-write.
    // 3. If the data is not shared, but previously was, then
    // m_appendLimit == m_end.  This will trigger grow(), which may
    // then notice that it doesn't have to copy-on-write.  In that
    // case, m_appendLimit is set back to m_limit.
    // 4. If m_data is null, then m_start is either the same as
    // m_prealloc, or it points at const raw data.
    T *m_start;
    T *m_end;
    T *m_limit;
    mutable T *m_appendLimit;
    Data *m_data;
    union {
        char
//#if defined(Q_DECL_ALIGN) && defined(Q_ALIGNOF)
            //Q_DECL_ALIGN(Q_ALIGNOF(T))
//#endif
            m_prealloc[sizeof(T) * PreallocSize];
        qint64 q_for_alignment_1;
        double q_for_alignment_2;
    };

    QDataArray(const T *data, int size);

    void initialize(T *data, int count);
    void free(T *data, int count);
    void release();
    void copy(T *dst, const T *src, int count);
    void move(T *dst, const T *src, int count);
    void copyReplace(T *dst, const T *src, int count);
    void reallocate(int capacity);
    void detachForWrite(int needed = 0);
    void assign(const QDataArray<T, PreallocSize>& other);
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

    typedef T *iterator;
    typedef const T *const_iterator;
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef ptrdiff_t difference_type;
    typedef int size_type;

    inline iterator begin() { return data(); }
    inline const_iterator begin() const { return constData(); }
    inline const_iterator constBegin() const { return constData(); }
    inline iterator end() { return data() + size(); }
    inline const_iterator end() const { return constData() + size(); }
    inline const_iterator constEnd() const { return constData() + size(); }

private:
    QDataArray<T, PreallocSize> *m_array;
    int m_offset, m_size;
};

int Q_QT3D_EXPORT qDataArrayAllocMore(int alloc, int extra);

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::initialize(T *data, int count)
{
    while (count-- > 0)
        new (data++) T();
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::free(T *data, int count)
{
    while (count-- > 0) {
        data->~T();
        ++data;
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::release()
{
    if (m_data) {
        if (!m_data->ref.deref()) {
            if (QTypeInfo<T>::isComplex)
                free(m_start, m_end - m_start);
            qFree(m_data);
        }
    } else if (m_start == reinterpret_cast<T *>(m_prealloc)) {
        if (QTypeInfo<T>::isComplex)
            free(m_start, m_end - m_start);
    }
}

// Copy values to uninitialized memory.
template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::copy(T *dst, const T *src, int count)
{
    while (count-- > 0)
        new (dst++) T(*src++);
}

// Move values to a new location in memory, optimizing for non-static types.
template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::move(T *dst, const T *src, int count)
{
    if (!QTypeInfo<T>::isStatic) {
        qMemCopy(dst, src, count * sizeof(T));
    } else {
        while (count-- > 0)
            new (dst++) T(*src++);
    }
}

// Copy values to initialized memory, replacing previous values.
template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::copyReplace(T *dst, const T *src, int count)
{
    if (!QTypeInfo<T>::isStatic) {
        ::memmove(dst, src, count * sizeof(T));
    } else {
        while (count-- > 0)
            *dst++ = *src++;
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::reallocate(int capacity)
{
    int size = m_end - m_start;
    if (!QTypeInfo<T>::isStatic) {
        Data *data = reinterpret_cast<Data *>
            (qRealloc(m_data, sizeof(Data) + sizeof(T) * (capacity - 1)));
        Q_CHECK_PTR(data);
        m_data = data;
    } else {
        Data *data = reinterpret_cast<Data *>
            (qMalloc(sizeof(Data) + sizeof(T) * (capacity - 1)));
        Q_CHECK_PTR(data);
        data->ref = 1;
        data->reserved = 0;
        copy(data->array, m_data->array, size);
        free(m_data->array, size);
        qFree(m_data);
        m_data = data;
    }
    m_start = m_data->array;
    m_end = m_start + size;
    m_appendLimit = m_limit = m_start + capacity;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::detachForWrite(int needed)
{
    // Allocate a new block on the heap and copy the data across.
    int size = m_end - m_start;
    int capacity = qDataArrayAllocMore(size, needed);
    Data *data = reinterpret_cast<Data *>
        (qMalloc(sizeof(Data) + sizeof(T) * (capacity - 1)));
    Q_CHECK_PTR(data);
    data->ref = 1;
    data->reserved = 0;
    if (size > 0)
        copy(data->array, m_start, size);
    m_data->ref.deref();
    m_data = data;

    // Update the start/end/append pointers for faster updates.
    m_start = m_data->array;
    m_end = m_start + size;
    m_appendLimit = m_limit = m_start + capacity;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::assign(const QDataArray<T, PreallocSize>& other)
{
    if (other.m_data) {
        m_start = other.m_start;
        m_end = other.m_end;
        m_limit = other.m_limit;
        m_data = other.m_data;
        m_data->ref.ref();

        // We set the append limit of both objects to m_end, which forces
        // the next append() in either object to copy-on-write.
        other.m_appendLimit = m_appendLimit = m_end;
    } else if (other.m_start == reinterpret_cast<const T *>(other.m_prealloc)) {
        // Copy preallocated data.
        int size = other.m_end - other.m_start;
        m_start = reinterpret_cast<T *>(m_prealloc);
        m_end = m_start + size;
        m_appendLimit = m_limit = m_start + PreallocSize;
        m_data = 0;
        if (size > 0)
            copy(m_start, other.m_start, size);
    } else {
        // Copy const raw data.
        m_start = other.m_start;
        m_end = other.m_end;
        m_limit = other.m_limit;
        m_appendLimit = other.m_appendLimit;
        m_data = 0;
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::grow(int needed)
{
    int size = m_end - m_start;
    int capacity = qDataArrayAllocMore(m_limit - m_start, needed);
    if (!m_data || m_data->ref != 1) {
        // Copy preallocated, raw, or shared data and expand the capacity.
        Data *data = reinterpret_cast<Data *>
            (qMalloc(sizeof(Data) + sizeof(T) * (capacity - 1)));
        Q_CHECK_PTR(data);
        data->ref = 1;
        data->reserved = 0;
        move(data->array, m_start, size);
        if (m_data) {
            m_data->ref.deref();
        } else if (QTypeInfo<T>::isStatic &&
                        m_start == reinterpret_cast<T *>(m_prealloc)) {
            free(m_start, size);
        }
        m_data = data;
        m_start = data->array;
        m_end = m_start + size;
        m_appendLimit = m_limit = m_start + capacity;
    } else if ((size + needed) > (m_limit - m_start)) {
        // Reallocate to create more capacity.
        reallocate(capacity);
    } else {
        // We have enough capacity - just fix the append limit.
        // This can happen when an array is copied and then the
        // copy is removed.
        m_appendLimit = m_limit;
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::QDataArray()
{
    m_start = reinterpret_cast<T *>(m_prealloc);
    m_end = m_start;
    m_appendLimit = m_limit = m_start + PreallocSize;
    m_data = 0;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::QDataArray(int size, const T& value)
{
    if (size <= PreallocSize) {
        m_start = reinterpret_cast<T *>(m_prealloc);
        m_end = m_start;
        m_appendLimit = m_limit = m_start + PreallocSize;
        m_data = 0;
    } else {
        int capacity = qDataArrayAllocMore(size, 0);
        Data *data = reinterpret_cast<Data *>
            (qMalloc(sizeof(Data) + sizeof(T) * (capacity - 1)));
        Q_CHECK_PTR(data);
        m_data = data;
        m_data->ref = 1;
        m_data->reserved = 0;
        m_start = m_data->array;
        m_end = m_start;
        m_appendLimit = m_limit = m_start + capacity;
    }
    while (size-- > 0)
        new (m_end++) T(value);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::QDataArray(const QDataArray<T, PreallocSize>& other)
{
    assign(other);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::QDataArray(const T *data, int size)
{
    // Constructing a raw data array.
    if (size > 0) {
        m_start = const_cast<T *>(data);
        m_appendLimit = m_limit = m_end = m_start + size;
        m_data = 0;
    } else {
        m_start = reinterpret_cast<T *>(m_prealloc);
        m_end = m_start;
        m_appendLimit = m_limit = m_start + PreallocSize;
        m_data = 0;
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>::~QDataArray()
{
    release();
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QDataArray<T, PreallocSize>& QDataArray<T, PreallocSize>::operator=(const QDataArray<T, PreallocSize>& other)
{
    if (other.m_data && m_data == other.m_data)
        return *this;
    release();
    assign(other);
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QDataArray<T, PreallocSize>::size() const
{
    return m_end - m_start;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QDataArray<T, PreallocSize>::count() const
{
    return m_end - m_start;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QDataArray<T, PreallocSize>::capacity() const
{
    return m_limit - m_start;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE bool QDataArray<T, PreallocSize>::isEmpty() const
{
    return m_start == m_end;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE bool QDataArray<T, PreallocSize>::isDetached() const
{
    return !m_data || m_data->ref == 1;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::clear()
{
    release();
    m_start = reinterpret_cast<T *>(m_prealloc);
    m_end = m_start;
    m_appendLimit = m_limit = m_start + PreallocSize;
    m_data = 0;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T& QDataArray<T, PreallocSize>::operator[](int index) const
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QDataArray<T>::at", "index out of range");
    return m_start[index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T& QDataArray<T, PreallocSize>::at(int index) const
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QDataArray<T>::operator[]", "index out of range");
    return m_start[index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T& QDataArray<T, PreallocSize>::operator[](int index)
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QDataArray<T>::operator[]", "index out of range");
    return data()[index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T *QDataArray<T, PreallocSize>::extend(int size)
{
    Q_ASSERT(size > 0);
    if ((m_end + size) >= m_appendLimit)
        grow(size);
    T *end = m_end;
    m_end += size;  // Note: new elements are not initialized.
    return end;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const T& value)
{
    if (m_end >= m_appendLimit)
        grow(1);
    new (m_end++) T(value);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const T& value1, const T& value2)
{
    if ((m_end + 1) >= m_appendLimit)
        grow(2);
    new (m_end) T(value1);
    new (m_end + 1) T(value2);
    m_end += 2;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const T& value1, const T& value2, const T& value3)
{
    if ((m_end + 2) >= m_appendLimit)
        grow(3);
    new (m_end) T(value1);
    new (m_end + 1) T(value2);
    new (m_end + 2) T(value3);
    m_end += 3;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const T& value1, const T& value2, const T& value3, const T& value4)
{
    if ((m_end + 3) >= m_appendLimit)
        grow(3);
    new (m_end) T(value1);
    new (m_end + 1) T(value2);
    new (m_end + 2) T(value3);
    new (m_end + 3) T(value4);
    m_end += 4;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::append(const T *values, int count)
{
    if (count <= 0)
        return;
    if (!m_start || (m_end + count) > m_appendLimit)
        grow(count);
    copy(m_end, values, count);
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
    copyReplace(data() + index, values, count);
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
    T *start = data();
    copyReplace(start + index, start + index + count,
                (currentSize - (index + count)));
    resize(currentSize - count);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::resize(int size)
{
    if (size < 0)
        return;
    int currentSize = count();
    if (size < currentSize) {
        T *start = data();  // Force copy on write if necessary.
        if (QTypeInfo<T>::isComplex)
            free(start + size, currentSize - size);
        m_end = start + size;
    } else if (size > currentSize) {
        grow(size - currentSize);
        initialize(m_start + currentSize, size - currentSize);
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
Q_INLINE_TEMPLATE void QDataArray<T, PreallocSize>::squeeze()
{
    squeeze(size());
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::squeeze(int size)
{
    if (size <= 0) {
        clear();
        return;
    }

    // Bail out if the capacity is already smaller than the squeeze size.
    if (size >= capacity())
        return;

    // Delete unneeded elements.
    if (size < count())
        resize(size);

    // If the array is in the preallocated area or raw data,
    // then no point squeezing.
    if (!m_data)
        return;

    // Reallocate the array on the heap to the smaller size.
    reallocate(size);
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QDataArray<T, PreallocSize>::reverse()
{
    if (count() > 0) {
        iterator src = begin();
        iterator dst = end();
        --dst;
        while (src < dst)
            qSwap(*(dst--), *(src++));
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE QDataArray<T, PreallocSize> QDataArray<T, PreallocSize>::reversed() const
{
    QDataArray<T, PreallocSize> result;
    int count = size();
    if (count > 0) {
        result.extend(count);
        const_iterator src = constBegin();
        const_iterator end = constEnd();
        iterator dst = result.end();
        --dst;
        if (!QTypeInfo<T>::isComplex) {
            while (src != end)
                *(dst--) = *(src++);
        } else {
            while (src != end)
                new (dst--) T(*src++);
        }
    }
    return result;
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
    if (m_data) {
        if (m_data->ref != 1)
            detachForWrite();
    } else if (m_start != reinterpret_cast<T *>(m_prealloc)) {
        // Copy the const raw data.
        detachForWrite();
    }
    return m_start;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T *QDataArray<T, PreallocSize>::data() const
{
    return m_start;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T *QDataArray<T, PreallocSize>::constData() const
{
    return m_start;
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
            if (!QTypeInfo<T>::isStatic) {
                qMemCopy(dst, m_array->constData() + m_offset,
                         sizeof(T) * m_size);
            } else {
                const T *src = m_array->constData() + m_offset;
                int size = m_size;
                while (size-- > 0)
                    new (dst++) T(*src++);
            }
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
