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

    \sa QDataArrayRef
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
    \fn T *QDataArray::extend(int size)

    Extends this data array by \a size elements and returns a pointer
    to the storage, which is not initialized.  The pointer is only
    valid until the array is reallocated or destroyed.

    \sa resize()
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
    \fn QDataArrayRef<T, PreallocSize> QDataArray::mid(int index, int length) const

    Returns a QDataArrayRef that refers to the \a length elements of
    this data array, starting at \a index.  If \a length is less
    than zero, then all elements extending from \a index to the
    end of the data array will be included in the returned reference.

    \sa left(), right()
*/

/*!
    \fn QDataArrayRef<T, PreallocSize> QDataArray::left(int length) const;

    Returns a QDataArrayRef that refers to the first \a length
    elements of this data array.  If \a length is less than zero,
    or greater than size(), then all elements in this data array will
    be included in the returned reference.

    \sa mid(), right()
*/

/*!
    \fn QDataArrayRef<T, PreallocSize> QDataArray::right(int length) const;

    Returns a QDataArrayRef that refers to the last \a length
    elements of this data array.  If \a length is less than zero,
    or greater than size(), then all elements in this data array
    will be included in the returned reference.

    \sa mid(), left()
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
    \fn QDataArray<T, PreallocSize> QDataArray::fromRawData(const T *data, int size)

    Returns a data array consisting of the \a size elements from \a data.

    This function takes a reference to \a data, but does not copy
    the elements until the array is modified.  The memory at \a data
    must remain valid until the returned data array is destroyed
    or modified.

    Use append() instead of fromRawData() to force a copy to be made
    of the elements at \a data when the data array is created:

    \code
    // Makes a copy of the data immediately.
    QDataArray<float> array;
    array.append(data, size);

    // Does not make a copy of the data until the array is modified.
    QDataArray<float> array;
    array = QDataArray<float>::fromRawData(data, size);
    \endcode

    \sa append()
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

/*!
    \typedef QDataArray::iterator

    The QDataArray::iterator typedef provides an STL-style non-const
    iterator for QDataArray.  The iterator is simply a typedef
    for "T *" (pointer to T).

    \sa QDataArray::begin(), QDataArray::const_iterator
*/

/*!
    \typedef QDataArray::const_iterator

    The QDataArray::iterator typedef provides an STL-style const
    iterator for QDataArray.  The iterator is simply a typedef
    for "const T *" (pointer to const T).

    \sa QDataArray::constBegin(), QDataArray::iterator
*/

/*!
    \typedef QDataArray::Iterator

    Qt-style synonym for QDataArray::iterator.
*/

/*!
    \typedef QDataArray::ConstIterator

    Qt-style synonym for QDataArray::const_iterator.
*/

/*!
    \fn QDataArray::iterator QDataArray::begin()

    Returns an STL-style iterator pointing to the first item
    in the data array.

    \sa end(), constBegin(), QDataArray::iterator
*/

/*!
    \fn QDataArray::const_iterator QDataArray::begin() const
    \overload
*/

/*!
    \fn QDataArray::const_iterator QDataArray::constBegin() const

    Returns a const STL-style iterator pointing to the first item
    in the data array.

    \sa constEnd(), begin(), QDataArray::const_iterator
*/

/*!
    \fn QDataArray::iterator QDataArray::end()

    Returns an STL-style iterator pointing to the imaginary item
    after the last item in the data array.

    \sa begin(), constEnd(), QDataArray::iterator
*/

/*!
    \fn QDataArray::const_iterator QDataArray::end() const
    \overload
*/

/*!
    \fn QDataArray::const_iterator QDataArray::constEnd() const

    Returns a const STL-style iterator pointing to the imaginary item
    after the last item in the data array.

    \sa constBegin(), end(), QDataArray::const_iterator
*/

/*!
    \class QDataArrayRef
    \brief The QDataArrayRef class is a template class for referring to a subset of a QDataArray.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers

    \sa QDataArray
*/

/*!
    \fn QDataArrayRef::QDataArrayRef()

    Constructs a null data array reference.

    \sa isNull()
*/

/*!
    \fn QDataArrayRef::QDataArrayRef(QDataArray<T, PreallocSize> *array)

    Constructs a referenct to all of the elements of \a array.

    If \a array is destroyed before this reference, then this
    reference will be pointing at invalid memory.  If the size
    of \a array is decreased by QDataArray::resize(), then
    elements beyond that point that are within the range
    of this reference will become undefined.
*/

/*!
    \fn QDataArrayRef::QDataArrayRef(QDataArray<T, PreallocSize> *array, int offset, int size)

    Constructs a reference to the \a size elements starting at
    \a offset in \a array.

    If \a size is less than zero or \a offset + \a size is
    greater than the size of \a array, then \a size will be
    adjusted to extend no further than the end of \a array.

    The \a offset must be between 0 and the size of \a array.

    If \a array is destroyed before this reference, then this
    reference will be pointing at invalid memory.  If the size
    of \a array is decreased by QDataArray::resize(), then
    elements beyond that point that are within the range
    of this reference will become undefined.
*/

/*!
    \fn bool QDataArrayRef::isNull() const

    Returns true if dataArray() is null; false otherwise.

    \sa dataArray()
*/

/*!
    \fn int QDataArrayRef::size() const

    Returns the number of elements in this reference.

    If dataArray() has been resized since the reference was
    constructed, it is possible for size() to refer to beyond
    the end of dataArray().  The extra elements of such a
    reference are undefined.

    \sa isEmpty(), offset()
*/

/*!
    \fn int QDataArrayRef::count() const

    Same as size(), provided for convenience.
*/

/*!
    \fn bool QDataArrayRef::isEmpty() const

    Returns true if this reference has a size() of zero;
    false otherwise.

    \sa size()
*/

/*!
    \fn const T& QDataArrayRef::at(int index) const

    Returns the item at position \a index in the data array.

    \a index must be a valid index position in the reference
    (i.e., 0 <= \a index < size()).  If dataArray() has been
    resized since the reference was constructed, then
    \a index + offset() must be a valid index position in
    dataArray().

    \sa operator[](), constData()
*/

/*!
    \fn const T& QDataArrayRef::operator[](int index) const

    Returns the item at position \a index in the data array.

    \a index must be a valid index position in the reference
    (i.e., 0 <= \a index < size()).  If dataArray() has been
    resized since the reference was constructed, then
    \a index + offset() must be a valid index position in
    dataArray().

    \sa at(), constData()
*/

/*!
    \fn T& QDataArrayRef::operator[](int index)

    Returns the item at position \a index in the data array.

    \a index must be a valid index position in the reference
    (i.e., 0 <= \a index < size()).  If dataArray() has been
    resized since the reference was constructed, then
    \a index + offset() must be a valid index position in
    dataArray().

    Note that using non-const operators can cause dataArray()
    to do a deep copy.

    \sa at(), data()
*/

/*!
    \fn T *QDataArrayRef::data()

    Returns a pointer to the data stored at offset() in dataArray().
    The pointer can be used to access and modify the items in the
    data array.

    The pointer remains valid as long as the data array isn't
    reallocated.

    This function is mostly useful to pass a data array to a function
    that accepts a plain C++ array.  It may make a deep copy of the
    data array's elements if the array is implicitly shared.

    \sa constData(), operator[]()
*/

/*!
    \fn const T *QDataArrayRef::data() const

    \overload
*/

/*!
    \fn const T *QDataArrayRef::constData() const;

    Returns a const pointer to the data stored at offset()
    in dataArray().  The pointer can be used to access the
    items in the data array.  The pointer remains valid as
    long as the data array isn't reallocated.

    This function is mostly useful to pass a data array to a function
    that accepts a plain C++ array.

    \sa data(), operator[]()
*/

/*!
    \fn QDataArray<T, PreallocSize> *QDataArrayRef::dataArray() const

    Returns the QDataArray that this reference was constructed from.

    \sa offset(), size()
*/

/*!
    \fn int QDataArrayRef::offset() const

    Returns the offset into dataArray() where the data referred
    to by this reference is located.

    If dataArray() has been resized since the reference was
    constructed, it is possible for offset() to refer to beyond
    the end of dataArray().  The elements of such a reference
    are undefined.

    \sa dataArray(), size()
*/

/*!
    \fn QDataArray<T, PreallocSize> QDataArrayRef::toDataArray() const

    Returns the elements of this reference as a new QDataArray.
*/

/*!
    \fn bool QDataArrayRef::operator==(const QDataArrayRef<T, PreallocSize> &other) const

    Returns true if \a other has the same size and elements
    as this reference; false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool QDataArrayRef::operator!=(const QDataArrayRef<T, PreallocSize> &other) const

    Returns true if \a other does not have the same size or elements
    as this reference; false otherwise.

    \sa operator==()
*/

/*!
    \typedef QDataArrayRef::iterator

    The QDataArrayRef::iterator typedef provides an STL-style non-const
    iterator for QDataArrayRef.  The iterator is simply a typedef
    for "T *" (pointer to T).

    \sa QDataArrayRef::begin(), QDataArrayRef::const_iterator
*/

/*!
    \typedef QDataArrayRef::const_iterator

    The QDataArrayRef::iterator typedef provides an STL-style const
    iterator for QDataArrayRef.  The iterator is simply a typedef
    for "const T *" (pointer to const T).

    \sa QDataArrayRef::constBegin(), QDataArrayRef::iterator
*/

/*!
    \typedef QDataArrayRef::Iterator

    Qt-style synonym for QDataArrayRef::iterator.
*/

/*!
    \typedef QDataArrayRef::ConstIterator

    Qt-style synonym for QDataArrayRef::const_iterator.
*/

/*!
    \fn QDataArrayRef::iterator QDataArrayRef::begin()

    Returns an STL-style iterator pointing to the first item
    in the data array reference.

    \sa end(), constBegin(), QDataArrayRef::iterator
*/

/*!
    \fn QDataArrayRef::const_iterator QDataArrayRef::begin() const
    \overload
*/

/*!
    \fn QDataArrayRef::const_iterator QDataArrayRef::constBegin() const

    Returns a const STL-style iterator pointing to the first item
    in the data array reference.

    \sa constEnd(), begin(), QDataArrayRef::const_iterator
*/

/*!
    \fn QDataArrayRef::iterator QDataArrayRef::end()

    Returns an STL-style iterator pointing to the imaginary item
    after the last item in the data array reference.

    \sa begin(), constEnd(), QDataArrayRef::iterator
*/

/*!
    \fn QDataArrayRef::const_iterator QDataArrayRef::end() const
    \overload
*/

/*!
    \fn QDataArrayRef::const_iterator QDataArrayRef::constEnd() const

    Returns a const STL-style iterator pointing to the imaginary item
    after the last item in the data array reference.

    \sa constBegin(), end(), QDataArrayRef::const_iterator
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
