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

#include "qarray.h"
#include <limits.h>

QT_BEGIN_NAMESPACE

/*!
    \class QArray
    \brief The QArray class is a template class that provides a dynamic array of simple types.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers

    QArray is similar to QVector except that it has less overhead
    when constructing large arrays by appending individual elements
    one by one.  It is intended for building arrays of points
    and vertex attributes in high-performance graphics applications.

    QArray instances have a preallocated data area for quickly
    building small arrays on the stack without malloc overhead.
    Once the array grows beyond the preallocated size, it is copied
    to the heap.  The size of the preallocated area, which defaults to 8,
    can be specified with the second template parameter:

    \code
    QArray<QVector3D, 32> array;
    \endcode

    QArray uses implicit sharing and copy-on-write semantics to support
    passing large arrays around an application with little overhead.

    \sa QArrayRef
*/

/*!
    \fn QArray::QArray()

    Constructs an empty array.

    \sa reserve()
*/

/*!
    \fn QArray::QArray(int size, const T &value)

    Constructs an array of \a size elements, all initialized
    to \a value.
*/

/*!
    \fn QArray::QArray(const QArray<T, PreallocSize> &other)

    Constructs a copy of \a other.

    \sa operator=()
*/

/*!
    \fn QArray::QArray(const QUnsharedArray<T, PreallocSize> &other)

    Constructs a copy of the unshared array \a other.

    This constructor makes a deep copy of the contents of \a other.
*/

/*!
    \fn QArray::~QArray()

    Destroys the array.
*/

/*!
    \fn QArray<T, PreallocSize> &QArray::operator=(const QArray<T, PreallocSize> &other)

    Assigns \a other to this array and returns a reference
    to this array.
*/

/*!
    \fn QArray<T, PreallocSize> &QArray::operator=(const QUnsharedArray<T, PreallocSize> &other)

    Assigns the contents \a other to this array and returns a reference
    to this array.

    This assignment operator makes a deep copy of the contents of \a other.
*/

/*!
    \fn int QArray::size() const

    Returns the number of elements in this array.

    \sa resize(), capacity(), isEmpty()
*/

/*!
    \fn int QArray::count() const

    Same as size(), provided for convenience.
*/

/*!
    \fn int QArray::capacity() const

    Returns the number of elements that can be stored in this
    array before reallocation.

    \sa reserve(), size()
*/

/*!
    \fn bool QArray::isEmpty() const

    Returns true if this array is empty; false otherwise.

    \sa size(), clear()
*/

/*!
    \fn bool QArray::isDetached() const

    Returns true if this array has been detached from all other
    shared copies of the data; false otherwise.

    This function can be used to determine if functions that
    write to this array such as append(), replace(),
    and data(), will need to make a copy.
*/

/*!
    \fn void QArray::clear()

    Clears all elements from this array and sets the size to zero.

    This function will deallocate any memory that is used on the heap
    to store the array's elements.  To reuse the same memory
    as before, call resize() with an argument of zero.

    \sa resize(), isEmpty()
*/

/*!
    \fn const T &QArray::at(int index) const

    Returns the item at position \a index in the array.

    \a index must be a valid index position in the array (i.e., 0 <= \a
    index < size()).

    \sa operator[](), constData()
*/

/*!
    \fn T &QArray::operator[](int index)

    Returns the item at position \a index as a modifiable reference.

    \a index must be a valid index position in the vector (i.e., 0 <= \a index
    < size()).

    Note that using non-const operators can cause QArray
    to do a deep copy.

    \sa at()
*/

/*!
    \fn const T &QArray::operator[](int index) const

    \overload

    Same as at(\a index).
*/

/*!
    \fn T *QArray::extend(int size)

    Extends this array by \a size elements and returns a pointer
    to the storage, which is not initialized.  The pointer is only
    valid until the array is reallocated or destroyed.

    The append() or resize() functions are recommended if T is a
    complex type, with extend() only used for simple types.
    Because the storage is not initialized, the caller should use
    the in-place new operator to set elements if T is a complex type:

    \code
    QArray<QRegExp> array;
    QRegExp *space = array.extend(1);
    new (space) QRegExp(QLatin1String("exp"));
    \endcode

    \sa append(), resize()
*/

/*!
    \fn void QArray::append(const T &value)

    Appends \a value to this array.
*/

/*!
    \fn void QArray::append(const T &value1, const T &value2)

    \overload

    Appends \a value1 and \a value2 to this array.
*/

/*!
    \fn void QArray::append(const T &value1, const T &value2, const T &value3)

    \overload

    Appends \a value1, \a value2, and \a value3 to this array.
*/

/*!
    \fn void QArray::append(const T &value1, const T &value2, const T &value3, const T &value4)

    \overload

    Appends \a value1, \a value2, \a value3, and \a value4 to this array.
*/

/*!
    \fn void QArray::append(const T *values, int count)

    Appends the \a count elements of \a values to this array.
*/

/*!
    \fn void QArray::append(const QArray<T, PreallocSize> &other)

    Appends the elements of \a other to this array.
*/

/*!
    \fn void QArray::replace(int index, const T *values, int count)

    Replaces the \a count elements of this array with the
    contents of \a values, starting at \a index.

    If (\a index + \a count) is larger than the current size of this
    array, the array will be extended to that size.

    \sa append()
*/

/*!
    \fn void QArray::remove(int index)

    \overload

    Removes the element at position \a index in this array.
*/

/*!
    \fn void QArray::remove(int index, int count)

    Removes the \a count elements starting at position \a index
    in this array.  If \a index or \a count is out of range,
    the set of removed elements will be truncated to those that
    are in range.
*/

/*!
    \fn void QArray::resize(int size)

    Sets the size of the array to \a size.  If \a size is greater
    than the current size, elements are added to the end; the new elements
    are initialized with all-zeroes.  If \a size is less than the current
    size, elements are removed from the end.

    \sa size(), reserve(), squeeze()
*/

/*!
    \fn void QArray::reserve(int size)

    Increases the capacity of this array to reserve space for
    at least \a size elements.  If the capacity is already larger
    than \a size, this function does nothing; in particular, it does
    not remove elements from the array like resize() does.

    This function can be useful when you know how roughly many elements
    will be appended ahead of time.  Reserving the space once can avoid
    unnecessary realloc operations later.

    \sa capacity(), resize(), squeeze()
*/

/*!
    \fn void QArray::squeeze()

    Releases any memory not required to store the array's elements
    by reducing its capacity() to size().

    \sa reserve(), capacity()
*/

/*!
    \fn void QArray::squeeze(int size)

    Reduces the capacity() of this array to \a size, removing
    elements from the end if necessary.  Does nothing if the capacity()
    is already less than \a size.

    This function is intended for reclaiming memory in an
    array that is being used over and over with different contents.
    As elements are added to an array, it will be constantly
    expanded in size.  This function can realloc the array
    to a smaller size to reclaim unused memory.

    \sa reserve(), capacity()
*/

/*!
    \fn void QArray::reverse()

    Reverses the order of this array in place.
*/

/*!
    \fn QArray<T, PreallocSize> QArray::reversed() const

    Returns a copy of this array with elements in the reverse order.
*/

/*!
    \fn QArrayRef<T, PreallocSize> QArray::mid(int index, int length) const

    Returns a QArrayRef that refers to the \a length elements of
    this array, starting at \a index.  If \a length is less
    than zero, then all elements extending from \a index to the
    end of the array will be included in the returned reference.

    \sa left(), right()
*/

/*!
    \fn QArrayRef<T, PreallocSize> QArray::left(int length) const;

    Returns a QArrayRef that refers to the first \a length
    elements of this array.  If \a length is less than zero,
    or greater than size(), then all elements in this array will
    be included in the returned reference.

    \sa mid(), right()
*/

/*!
    \fn QArrayRef<T, PreallocSize> QArray::right(int length) const;

    Returns a QArrayRef that refers to the last \a length
    elements of this array.  If \a length is less than zero,
    or greater than size(), then all elements in this array
    will be included in the returned reference.

    \sa mid(), left()
*/

/*!
    \fn T *QArray::data()

    Returns a pointer to the data stored in the array.  The pointer
    can be used to access and modify the items in the array.

    The pointer remains valid as long as the array isn't
    reallocated.

    This function is mostly useful to pass an array to a function
    that accepts a plain C++ array.  It may make a deep copy of the
    array's elements if the array is implicitly shared.

    \sa constData(), operator[]()
*/

/*!
    \fn const T *QArray::data() const

    \overload
*/

/*!
    \fn const T *QArray::constData() const

    Returns a const pointer to the data stored in the array.
    The pointer can be used to access the items in the array.
    The pointer remains valid as long as the array isn't
    reallocated.

    This function is mostly useful to pass an array to a function
    that accepts a plain C++ array.

    \sa data(), operator[]()
*/

/*!
    \fn QArray<T, PreallocSize> QArray::fromRawData(const T *data, int size)

    Returns an array consisting of the \a size elements from \a data.

    This function takes a reference to \a data, but does not copy
    the elements until the array is modified.  The memory at \a data
    must remain valid until the returned array is destroyed
    or modified.

    Use append() instead of fromRawData() to force a copy to be made
    of the elements at \a data when the array is created:

    \code
    // Makes a copy of the data immediately.
    QArray<float> array;
    array.append(data, size);

    // Does not make a copy of the data until the array is modified.
    QArray<float> array;
    array = QArray<float>::fromRawData(data, size);
    \endcode

    \sa append()
*/

/*!
    \fn bool QArray::operator==(const QArray<T, PreallocSize> &other) const

    Returns true if \a other is equal to this array; otherwise
    returns false.

    Two arrays are considered equal if they contain the same values
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
    \fn bool QArray::operator!=(const QArray<T, PreallocSize> &other) const

    Returns true if \a other is not equal to this array; otherwise
    returns false.

    Two arrays are considered equal if they contain the same values
    in the same order.

    This function requires the value type to have an implementation
    of \c operator==().

    \sa operator==()
*/

/*!
    \fn QArray<T, PreallocSize> &QArray::operator+=(const T &value)

    \overload

    Appends \a value to this array and returns a reference to
    this array.

    \sa operator<<(), append()
*/

/*!
    \fn QArray<T, PreallocSize> &QArray::operator+=(const QArray<T, PreallocSize> &other)

    Appends the elements of the \a other array to this array
    and returns a reference to this array.

    \sa operator<<(), append()
*/

/*!
    \fn QArray<T, PreallocSize> &QArray::operator<<(const T &value)

    \overload

    Appends \a value to this array and returns a reference to
    this array.

    \sa operator+=(), append()
*/

/*!
    \fn QArray<T, PreallocSize> &QArray::operator<<(const QArray<T, PreallocSize> &other)

    Appends the elements of the \a other array to this array
    and returns a reference to this array.

    \sa operator+=(), append()
*/

/*!
    \typedef QArray::iterator

    The QArray::iterator typedef provides an STL-style non-const
    iterator for QArray.  The iterator is simply a typedef
    for "T *" (pointer to T).

    \sa QArray::begin(), QArray::const_iterator
*/

/*!
    \typedef QArray::const_iterator

    The QArray::iterator typedef provides an STL-style const
    iterator for QArray.  The iterator is simply a typedef
    for "const T *" (pointer to const T).

    \sa QArray::constBegin(), QArray::iterator
*/

/*!
    \typedef QArray::Iterator

    Qt-style synonym for QArray::iterator.
*/

/*!
    \typedef QArray::ConstIterator

    Qt-style synonym for QArray::const_iterator.
*/

/*!
    \typedef QArray::const_pointer

    Typedef for const T *. Provided for STL compatibility.
*/

/*!
    \typedef QArray::const_reference

    Typedef for T &. Provided for STL compatibility.
*/

/*!
    \typedef QArray::difference_type

    Typedef for ptrdiff_t. Provided for STL compatibility.
*/

/*!
    \typedef QArray::pointer

    Typedef for T *. Provided for STL compatibility.
*/

/*!
    \typedef QArray::reference

    Typedef for T &. Provided for STL compatibility.
*/

/*!
    \typedef QArray::size_type

    Typedef for int. Provided for STL compatibility.
*/

/*!
    \typedef QArray::value_type

    Typedef for T. Provided for STL compatibility.
*/

/*!
    \fn QArray::iterator QArray::begin()

    Returns an STL-style iterator pointing to the first item
    in the array.

    \sa end(), constBegin(), QArray::iterator
*/

/*!
    \fn QArray::const_iterator QArray::begin() const
    \overload
*/

/*!
    \fn QArray::const_iterator QArray::constBegin() const

    Returns a const STL-style iterator pointing to the first item
    in the array.

    \sa constEnd(), begin(), QArray::const_iterator
*/

/*!
    \fn QArray::iterator QArray::end()

    Returns an STL-style iterator pointing to the imaginary item
    after the last item in the array.

    \sa begin(), constEnd(), QArray::iterator
*/

/*!
    \fn QArray::const_iterator QArray::end() const
    \overload
*/

/*!
    \fn QArray::const_iterator QArray::constEnd() const

    Returns a const STL-style iterator pointing to the imaginary item
    after the last item in the array.

    \sa constBegin(), end(), QArray::const_iterator
*/

/*!
    \class QUnsharedArray
    \brief The QUnsharedArray class is a template class for an unreference counted array.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers

    QUnsharedArray differs from QArray in that it does not support
    implicit sharing.  This results in better performance for random access
    to the array's contents, at the cost of a reduction in performance for
    array copying.

    \sa QArray
*/

/*!
    \fn QUnsharedArray::QUnsharedArray()

    Constructs and empty unshared array.
*/

/*!
    \fn QUnsharedArray::QUnsharedArray(int size, const T &value)

    Constructs an unshared array of \a size elements, all initialized
    to \a value.
*/

/*!
    \fn QUnsharedArray::QUnsharedArray(const QArray<T, PreallocSize> &other)

    Constructs a deep copy of \a other.
*/

/*!
    \fn QUnsharedArray<T, PreallocSize> &QUnsharedArray::operator=(const QArray<T, PreallocSize> &other)

    Assigns the contents \a other to this array and returns a reference
    to this array.

    This assignment operator makes a deep copy of the contents of \a other.
*/

/*!
    \fn T &QUnsharedArray::operator[](int index)

    Returns the item at position \a index as a modifiable reference.

    \a index must be a valid index position in the vector (i.e., 0 <= \a index
    < size()).

    \sa at()
*/

/*!
    \fn const T &QUnsharedArray::operator[](int index) const

    \overload

    Same as at(\a index).
*/

/*!
    \fn T *QUnsharedArray::data()

    Returns a pointer to the data stored at offset() in array().
    The pointer can be used to access and modify the items in the
    array.

    The pointer remains valid as long as the array isn't
    reallocated.

    This function is mostly useful to pass an array to a function
    that accepts a plain C++ array.

    \sa constData(), operator[]()
*/

/*!
    \fn const T *QUnsharedArray::data() const

    \overload
*/

/*!
    \fn QUnsharedArray<T, PreallocSize> QUnsharedArray::fromRawData(const T *data, int size)

    Returns a new unshared array that is initialized to the \a size
    elements at \a data.

    This function makes a deep copy of the elements at \a data,
    unlike QArray::fromRawData().
*/

/*!
    \class QArrayRef
    \brief The QArrayRef class is a template class for referring to a subset of a QArray.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers

    \sa QArray
*/

/*!
    \fn QArrayRef::QArrayRef()

    Constructs a null array reference.

    \sa isNull()
*/

/*!
    \fn QArrayRef::QArrayRef(QArray<T, PreallocSize> *array)

    Constructs a referenct to all of the elements of \a array.

    If \a array is destroyed before this reference, then this
    reference will be pointing at invalid memory.  If the size
    of \a array is decreased by QArray::resize(), then
    elements beyond that point that are within the range
    of this reference will become undefined.
*/

/*!
    \fn QArrayRef::QArrayRef(QArray<T, PreallocSize> *array, int offset, int size)

    Constructs a reference to the \a size elements starting at
    \a offset in \a array.

    If \a size is less than zero or \a offset + \a size is
    greater than the size of \a array, then \a size will be
    adjusted to extend no further than the end of \a array.

    The \a offset must be between 0 and the size of \a array.

    If \a array is destroyed before this reference, then this
    reference will be pointing at invalid memory.  If the size
    of \a array is decreased by QArray::resize(), then
    elements beyond that point that are within the range
    of this reference will become undefined.
*/

/*!
    \fn bool QArrayRef::isNull() const

    Returns true if array() is null; false otherwise.

    \sa array()
*/

/*!
    \fn int QArrayRef::size() const

    Returns the number of elements in this reference.

    If array() has been resized since the reference was
    constructed, it is possible for size() to refer to beyond
    the end of array().  The extra elements of such a
    reference are undefined.

    \sa isEmpty(), offset()
*/

/*!
    \fn int QArrayRef::count() const

    Same as size(), provided for convenience.
*/

/*!
    \fn bool QArrayRef::isEmpty() const

    Returns true if this reference has a size() of zero;
    false otherwise.

    \sa size()
*/

/*!
    \fn const T &QArrayRef::at(int index) const

    Returns the item at position \a index in the array.

    \a index must be a valid index position in the reference
    (i.e., 0 <= \a index < size()).  If array() has been
    resized since the reference was constructed, then
    \a index + offset() must be a valid index position in
    array().

    \sa operator[](), constData()
*/

/*!
    \fn const T &QArrayRef::operator[](int index) const

    Returns the item at position \a index in the array.

    \a index must be a valid index position in the reference
    (i.e., 0 <= \a index < size()).  If array() has been
    resized since the reference was constructed, then
    \a index + offset() must be a valid index position in
    array().

    \sa at(), constData()
*/

/*!
    \fn T &QArrayRef::operator[](int index)

    Returns the item at position \a index in the array.

    \a index must be a valid index position in the reference
    (i.e., 0 <= \a index < size()).  If array() has been
    resized since the reference was constructed, then
    \a index + offset() must be a valid index position in
    array().

    Note that using non-const operators can cause array()
    to do a deep copy.

    \sa at(), data()
*/

/*!
    \fn T *QArrayRef::data()

    Returns a pointer to the data stored at offset() in array().
    The pointer can be used to access and modify the items in the
    array.

    The pointer remains valid as long as the array isn't
    reallocated.

    This function is mostly useful to pass an array to a function
    that accepts a plain C++ array.  It may make a deep copy of the
    array's elements if the array is implicitly shared.

    \sa constData(), operator[]()
*/

/*!
    \fn const T *QArrayRef::data() const

    \overload
*/

/*!
    \fn const T *QArrayRef::constData() const;

    Returns a const pointer to the data stored at offset()
    in array().  The pointer can be used to access the
    items in the array.  The pointer remains valid as
    long as the array isn't reallocated.

    This function is mostly useful to pass an array to a function
    that accepts a plain C++ array.

    \sa data(), operator[]()
*/

/*!
    \fn QArray<T, PreallocSize> *QArrayRef::array() const

    Returns the QArray that this reference was constructed from.

    \sa offset(), size()
*/

/*!
    \fn int QArrayRef::offset() const

    Returns the offset into array() where the data referred
    to by this reference is located.

    If array() has been resized since the reference was
    constructed, it is possible for offset() to refer to beyond
    the end of array().  The elements of such a reference
    are undefined.

    \sa array(), size()
*/

/*!
    \fn QArray<T, PreallocSize> QArrayRef::toDataArray() const

    Returns the elements of this reference as a new QArray.
*/

/*!
    \fn bool QArrayRef::operator==(const QArrayRef<T, PreallocSize> &other) const

    Returns true if \a other has the same size and elements
    as this reference; false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool QArrayRef::operator!=(const QArrayRef<T, PreallocSize> &other) const

    Returns true if \a other does not have the same size or elements
    as this reference; false otherwise.

    \sa operator==()
*/

/*!
    \typedef QArrayRef::iterator

    The QArrayRef::iterator typedef provides an STL-style non-const
    iterator for QArrayRef.  The iterator is simply a typedef
    for "T *" (pointer to T).

    \sa QArrayRef::begin(), QArrayRef::const_iterator
*/

/*!
    \typedef QArrayRef::const_iterator

    The QArrayRef::iterator typedef provides an STL-style const
    iterator for QArrayRef.  The iterator is simply a typedef
    for "const T *" (pointer to const T).

    \sa QArrayRef::constBegin(), QArrayRef::iterator
*/

/*!
    \typedef QArrayRef::Iterator

    Qt-style synonym for QArrayRef::iterator.
*/

/*!
    \typedef QArrayRef::ConstIterator

    Qt-style synonym for QArrayRef::const_iterator.
*/

/*!
    \typedef QArrayRef::const_pointer

    Typedef for const T *. Provided for STL compatibility.
*/

/*!
    \typedef QArrayRef::const_reference

    Typedef for T &. Provided for STL compatibility.
*/

/*!
    \typedef QArrayRef::difference_type

    Typedef for ptrdiff_t. Provided for STL compatibility.
*/

/*!
    \typedef QArrayRef::pointer

    Typedef for T *. Provided for STL compatibility.
*/

/*!
    \typedef QArrayRef::reference

    Typedef for T &. Provided for STL compatibility.
*/

/*!
    \typedef QArrayRef::size_type

    Typedef for int. Provided for STL compatibility.
*/

/*!
    \typedef QArrayRef::value_type

    Typedef for T. Provided for STL compatibility.
*/

/*!
    \fn QArrayRef::iterator QArrayRef::begin()

    Returns an STL-style iterator pointing to the first item
    in the array reference.

    \sa end(), constBegin(), QArrayRef::iterator
*/

/*!
    \fn QArrayRef::const_iterator QArrayRef::begin() const
    \overload
*/

/*!
    \fn QArrayRef::const_iterator QArrayRef::constBegin() const

    Returns a const STL-style iterator pointing to the first item
    in the array reference.

    \sa constEnd(), begin(), QArrayRef::const_iterator
*/

/*!
    \fn QArrayRef::iterator QArrayRef::end()

    Returns an STL-style iterator pointing to the imaginary item
    after the last item in the array reference.

    \sa begin(), constEnd(), QArrayRef::iterator
*/

/*!
    \fn QArrayRef::const_iterator QArrayRef::end() const
    \overload
*/

/*!
    \fn QArrayRef::const_iterator QArrayRef::constEnd() const

    Returns a const STL-style iterator pointing to the imaginary item
    after the last item in the array reference.

    \sa constBegin(), end(), QArrayRef::const_iterator
*/

int qArrayAllocMore(int alloc, int extra)
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
