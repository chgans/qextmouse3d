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

#include "qdataarray.h"
#include <limits.h>

QT_BEGIN_NAMESPACE

/*!
    \class QDataArray
    \brief The QDataArray class is a template class that provides a dynamic array of simple types.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers

    QDataArray is similar to QVector except that it has less overhead
    when constructing large arrays of simple types by appending individual
    elements one by one.  It is intended for building arrays of points
    and vertex attributes in high-performance graphics applications.
    For other applications, QVector should be used instead.

    The first template parameter T is restricted to simple primitive
    and movable types such as int, float, QVector3D, etc.  In particular,
    constructors and destructors will not be called on elements that are
    stored in the array.  When an array is extended with resize(),
    new elements are filled with zeroes.

    QDataArray instances have a preallocated data area for quickly
    building small arrays on the stack without malloc overhead.
    Once the array grows beyond the preallocated size, it is copied
    to the heap.  The size of the preallocated area, which defaults to 8,
    can be specified with the second template parameter:

    \code
    QDataArray<QVector3D, 32> array;
    \endcode

    QDataArray uses implicit sharing and copy-on-write semantics to support
    passing large arrays around an application with little overhead.
*/

/*!
    \fn QDataArray::QDataArray()

    Constructs an empty data array.

    \sa reserve()
*/

/*!
    \fn QDataArray::QDataArray(int size, const T& value)

    Constructs a data array of \a size elements, all initialized
    to \a value.
*/

/*!
    \fn QDataArray::QDataArray(const QDataArray<T, PreallocSize>& other)

    Constructs a copy of \a other.

    \sa operator=()
*/

/*!
    \fn QDataArray::~QDataArray()

    Destroys the data array.
*/

/*!
    \fn QDataArray<T, PreallocSize>& QDataArray::operator=(const QDataArray<T, PreallocSize>& other)

    Assigns \a other to this data array and returns a reference
    to this data array.
*/

/*!
    \fn int QDataArray::size() const

    Returns the number of elements in this data array.

    \sa resize(), capacity(), isEmpty()
*/

/*!
    \fn int QDataArray::count() const

    Same as size(), provided for convenience.
*/

/*!
    \fn int QDataArray::capacity() const

    Returns the number of elements that can be stored in this
    data array before reallocation.

    \sa reserve(), size()
*/

/*!
    \fn bool QDataArray::isEmpty() const

    Returns true if this data array is empty; false otherwise.

    \sa size(), clear()
*/

/*!
    \fn void QDataArray::clear()

    Clears all elements from this data array and sets the size to zero.

    This function will deallocate any memory that is used on the heap
    to store the data array's elements.  To reuse the same memory
    as before, call resize() with an argument of zero.

    \sa resize(), isEmpty()
*/

/*!
    \fn const T& QDataArray::at(int index) const

    Returns the item at position \a index in the data array.

    \a index must be a valid index position in the data array (i.e., 0 <= \a
    index < size()).

    \sa operator[](), constData()
*/

/*!
    \fn T& QDataArray::operator[](int index)

    Returns the item at position \a index as a modifiable reference.

    \a index must be a valid index position in the vector (i.e., 0 <= \a index
    < size()).

    Note that using non-const operators can cause QDataArray
    to do a deep copy.

    \sa at()
*/

/*!
    \fn const T& QDataArray::operator[](int index) const

    \overload

    Same as at(\a index).
*/

/*!
    \fn void QDataArray::append(const T& value)

    Appends \a value to this data array.
*/

/*!
    \fn void QDataArray::append(const T& value1, const T& value2)

    \overload

    Appends \a value1 and \a value2 to this data array.
*/

/*!
    \fn void QDataArray::append(const T& value1, const T& value2, const T& value3)

    \overload

    Appends \a value1, \a value2, and \a value3 to this data array.
*/

/*!
    \fn void QDataArray::append(const T& value1, const T& value2, const T& value3, const T& value4)

    \overload

    Appends \a value1, \a value2, \a value3, and \a value4 to this data array.
*/

/*!
    \fn void QDataArray::append(const T *values, int count)

    Appends the \a count elements of \a values to this data array.
*/

/*!
    \fn void QDataArray::append(const QDataArray<T, PreallocSize>& other)

    Appends the elements of \a other to this data array.
*/

/*!
    \fn void QDataArray::replace(int index, const T *values, int count)

    Replaces the \a count elements of this data array with the
    contents of \a values, starting at \a index.

    If (\a index + \a count) is larger than the current size of this
    data array, the array will be extended to that size.

    \sa append()
*/

/*!
    \fn void QDataArray::remove(int index)

    \overload

    Removes the element at position \a index in this data array.
*/

/*!
    \fn void QDataArray::remove(int index, int count)

    Removes the \a count elements starting at position \a index
    in this data array.  If \a index or \a count is out of range,
    the set of removed elements will be truncated to those that
    are in range.
*/

/*!
    \fn void QDataArray::resize(int size)

    Sets the size of the data array to \a size.  If \a size is greater
    than the current size, elements are added to the end; the new elements
    are initialized with all-zeroes.  If \a size is less than the current
    size, elements are removed from the end.

    \sa size(), reserve(), shrink()
*/

/*!
    \fn void QDataArray::reserve(int size)

    Increases the capacity of this data array to reserve space for
    at least \a size elements.  If the capacity is already larger
    than \a size, this function does nothing; in particular, it does
    not remove elements from the array like resize() does.

    This function can be useful when you know how roughly many elements
    will be appended ahead of time.  Reserving the space once can avoid
    unnecessary realloc operations later.

    \sa capacity(), resize(), shrink()
*/

/*!
    \fn void QDataArray::shrink(int size)

    Shrinks the capacity of this data array to \a size, removing
    elements from the end if necessary.  Does nothing if the capacity
    is already less than \a size.

    This function is intended for reclaiming memory in a data
    array that is being used over and over with different contents.
    As elements are added to a data array, it will be constantly
    expanded in size.  This function will realloc the data array
    to a smaller size to reclaim unused memory.

    \sa reserve()
*/

/*!
    \fn T *QDataArray::data()

    Returns a pointer to the data stored in the data array.  The pointer
    can be used to access and modify the items in the data array.

    The pointer remains valid as long as the data array isn't
    reallocated.

    This function is mostly useful to pass a data array to a function
    that accepts a plain C++ array.  It may make a deep copy of the
    data array's elements if the array is implicitly shared.

    \sa constData(), operator[]()
*/

/*!
    \fn const T *QDataArray::data() const

    \overload
*/

/*!
    \fn const T *QDataArray::constData() const

    Returns a const pointer to the data stored in the data array.
    The pointer can be used to access the items in the data array.
    The pointer remains valid as long as the data array isn't
    reallocated.

    This function is mostly useful to pass a data array to a function
    that accepts a plain C++ array.

    \sa data(), operator[]()
*/

/*!
    \fn bool QDataArray::operator==(const QDataArray<T, PreallocSize> &other) const

    Returns true if \a other is equal to this data array; otherwise
    returns false.

    Two data arrays are considered equal if they contain the same values
    in the same order.
    This function requires the value type to have an implementation
    of \c operator==().

    This can be very expensive time-wise.  Consider using the constData()
    instead to quickly determine if two arrays are identical because
    they occupy the same memory location:

    \code
    if (array1.constData() == array2.constData())
        ...
    \endcode

    \sa operator!=(), constData()
*/

/*!
    \fn bool QDataArray::operator!=(const QDataArray<T, PreallocSize> &other) const

    Returns true if \a other is not equal to this data array; otherwise
    returns false.

    Two data arrays are considered equal if they contain the same values
    in the same order.

    This function requires the value type to have an implementation
    of \c operator==().

    \sa operator==()
*/

/*!
    \fn QDataArray<T, PreallocSize>& QDataArray::operator+=(const T& value)

    \overload

    Appends \a value to this data array and returns a reference to
    this data array.

    \sa operator<<(), append()
*/

/*!
    \fn QDataArray<T, PreallocSize>& QDataArray::operator+=(const QDataArray<T, PreallocSize>& other)

    Appends the elements of the \a other data array to this data array
    and returns a reference to this data array.

    \sa operator<<(), append()
*/

/*!
    \fn QDataArray<T, PreallocSize>& QDataArray::operator<<(const T& value)

    \overload

    Appends \a value to this data array and returns a reference to
    this data array.

    \sa operator+=(), append()
*/

/*!
    \fn QDataArray<T, PreallocSize>& QDataArray::operator<<(const QDataArray<T, PreallocSize>& other)

    Appends the elements of the \a other data array to this data array
    and returns a reference to this data array.

    \sa operator+=(), append()
*/

int qDataArrayAllocMore(int alloc, int extra)
{
    if (alloc == 0 && extra == 0)
        return 0;
    const int page = 1 << 12;
    int nalloc;
    alloc += extra;
    // don't do anything if the loop will overflow signed int.
    if (alloc >= INT_MAX/2)
        return INT_MAX;
    nalloc = (alloc < page) ? 64 : page;
    while (nalloc < alloc) {
        if (nalloc <= 0)
            return INT_MAX;
        nalloc *= 2;
    }
    return nalloc;
}

QT_END_NAMESPACE
