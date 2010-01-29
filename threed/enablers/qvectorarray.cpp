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

#include "qvectorarray.h"

QT_BEGIN_NAMESPACE

/*!
    \class QVector2DArray
    \brief The QVector2DArray class is a convenience for wrapping a QArray of QVector2D values.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers

    QVector2DArray is used to build an array of 2D vector values
    based on floating-point x and y arguments:

    \code
    QVector2DArray array;
    array.append(1.0f, 2.0f);
    array.append(-1.0f, 2.0f);
    array.append(1.0f, -2.0f);
    \endcode

    This is more convenient and readable than the equivalent with
    QArray:

    \code
    QArray<QVector2D> array;
    array.append(QVector2D(1.0f, 2.0f));
    array.append(QVector2D(-1.0f, 2.0f));
    array.append(QVector2D(1.0f, -2.0f));
    \endcode

    QVector2DArray also has convenience functions for transforming
    the contents of the array with translate(), translated(),
    transform(), and transformed().

    \sa QArray, QVector3DArray, QVector4DArray
*/

/*!
    \fn QVector2DArray::QVector2DArray()

    Constructs an empty array of QVector2D values.
*/

/*!
    \fn QVector2DArray::QVector2DArray(int size, const QVector2D& value)

    Constructs an array of QVector2D values with an initial \a size.
    All elements in the array are initialized to \a value.
*/

/*!
    \fn QVector2DArray::QVector2DArray(const QArray<QVector2D>& other)

    Constructs a copy of \a other.
*/

/*!
    \fn void QVector2DArray::append(qreal x, qreal y)
    \overload

    Appends (\a x, \a y) to this array of QVector2D values.
*/

/*!
    \fn void QVector2DArray::append(const QPointF& point)
    \overload

    Appends \a point to this array of QVector2D values.
*/

/*!
    \fn void QVector2DArray::append(const QPoint& point);
    \overload

    Appends \a point to this array of QVector2D values.
*/

/*!
    Translates the elements in this array of QVector2D values
    by the components of \a value.

    \sa translated()
*/
void QVector2DArray::translate(const QVector2D& value)
{
    if (isDetached()) {
        // Modify the array in-place.
        int size = count();
        QVector2D *dst = data();
        for (int index = 0; index < size; ++index)
            *dst++ += value;
    } else {
        // Create a new array, translate the values, and assign.
        QArray<QVector2D> result;
        int size = count();
        const QVector2D *src = constData();
        QVector2D *dst = result.extend(size);
        for (int index = 0; index < size; ++index)
            *dst++ = *src++ + value;
        *this = result;
    }
}

/*!
    \fn void QVector2DArray::translate(qreal x, qreal y)
    \overload

    Translates the elements in this array of QVector2D values
    by (\a x, \a y).

    \sa translated()
*/

/*!
    \fn void QVector2DArray::translate(const QPointF& point)
    \overload

    Translates the elements in this array of QVector2D values
    by the components of \a point.

    \sa translated()
*/

/*!
    \fn void QVector2DArray::translate(const QPoint& point)
    \overload

    Translates the elements in this array of QVector2D values
    by the components of \a point.

    \sa translated()
*/

/*!
    Returns a copy of this array of QVector2D values, translated
    by the components of \a value.

    \sa translate()
*/
QArray<QVector2D> QVector2DArray::translated(const QVector2D& value) const
{
    QArray<QVector2D> result;
    int size = count();
    QVector2D *dst = result.extend(size);
    const QVector2D *src = constData();
    for (int index; index < size; ++index)
        *dst++ = *src++ + value;
    return result;
}

/*!
    \fn QArray<QVector2D> QVector2DArray::translated(qreal x, qreal y) const
    \overload

    Returns a copy of this array of QVector2D values, translated
    by (\a x, \a y).

    \sa translate()
*/

/*!
    \fn QArray<QVector2D> QVector2DArray::translated(const QPointF& point) const
    \overload

    Returns a copy of this array of QVector2D values, translated
    by the components of \a point.

    \sa translate()
*/

/*!
    \fn QArray<QVector2D> QVector2DArray::translated(const QPoint& point) const
    \overload

    Returns a copy of this array of QVector2D values, translated
    by the components of \a point.

    \sa translate()
*/

/*!
    \overload

    Transforms the elements in this array of QVector2D values
    by \a matrix.

    \sa transformed()
*/
void QVector2DArray::transform(const QTransform& matrix)
{
    if (isDetached()) {
        // Modify the array in-place.
        int size = count();
        QVector2D *dst = data();
        for (int index = 0; index < size; ++index) {
            *dst = QVector2D(matrix.map(dst->toPointF()));
            ++dst;
        }
    } else {
        // Create a new array, transform the values, and assign.
        QArray<QVector2D> result;
        int size = count();
        const QVector2D *src = constData();
        QVector2D *dst = result.extend(size);
        for (int index = 0; index < size; ++index)
            *dst++ = QVector2D(matrix.map((src++)->toPointF()));
        *this = result;
    }
}

/*!
    Transforms the elements in this array of QVector2D values
    by \a matrix.

    \sa transformed()
*/
void QVector2DArray::transform(const QMatrix4x4& matrix)
{
    if (isDetached()) {
        // Modify the array in-place.
        int size = count();
        QVector2D *dst = data();
        for (int index = 0; index < size; ++index) {
            *dst = (matrix * QVector3D(*dst, 0.0f)).toVector2D();
            ++dst;
        }
    } else {
        // Create a new array, transform the values, and assign.
        QArray<QVector2D> result;
        int size = count();
        const QVector2D *src = constData();
        QVector2D *dst = result.extend(size);
        for (int index = 0; index < size; ++index)
            *dst++ = (matrix * QVector3D(*src++, 0.0f)).toVector2D();
        *this = result;
    }
}

/*!
    \overload

    Returns a copy of this array of QVector2D values,
    transformed by \a matrix.

    \sa transformed()
*/
QArray<QVector2D> QVector2DArray::transformed(const QTransform& matrix) const
{
    QArray<QVector2D> result;
    int size = count();
    const QVector2D *src = constData();
    QVector2D *dst = result.extend(size);
    for (int index = 0; index < size; ++index)
        *dst++ = QVector2D(matrix.map((src++)->toPointF()));
    return result;
}

/*!
    Returns a copy of this array of QVector2D values,
    transformed by \a matrix.

    \sa transform()
*/
QArray<QVector2D> QVector2DArray::transformed(const QMatrix4x4& matrix) const
{
    QArray<QVector2D> result;
    int size = count();
    const QVector2D *src = constData();
    QVector2D *dst = result.extend(size);
    for (int index = 0; index < size; ++index)
        *dst++ = (matrix * QVector3D(*src++, 0.0f)).toVector2D();
    return result;
}

/*!
    \class QVector3DArray
    \brief The QVector3DArray class is a convenience for wrapping a QArray of QVector3D values.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers

    QVector3DArray is used to build an array of 3D vector values
    based on floating-point x, y, and z arguments:

    \code
    QVector3DArray array;
    array.append(1.0f, 2.0f, 3.0f);
    array.append(-1.0f, 2.0f, 3.0f);
    array.append(1.0f, -2.0f, 3.0f);
    \endcode

    This is more convenient and readable than the equivalent with
    QArray:

    \code
    QArray<QVector3D> array;
    array.append(QVector3D(1.0f, 2.0f, 3.0f));
    array.append(QVector3D(-1.0f, 2.0f, 3.0f));
    array.append(QVector3D(1.0f, -2.0f, 3.0f));
    \endcode

    QVector3DArray also has convenience functions for transforming
    the contents of the array with translate(), translated(),
    transform(), and transformed().

    \sa QArray, QVector2DArray, QVector4DArray
*/

/*!
    \fn QVector3DArray::QVector3DArray()

    Constructs an empty array of QVector3D values.
*/

/*!
    \fn QVector3DArray::QVector3DArray(int size, const QVector3D& value)

    Constructs an array of QVector3D values with an initial \a size.
    All elements in the array are initialized to \a value.
*/

/*!
    \fn QVector3DArray::QVector3DArray(const QArray<QVector3D>& other)

    Constructs a copy of \a other.
*/

/*!
    \fn void QVector3DArray::append(qreal x, qreal y, qreal z)

    Appends (\a x, \a y, \a z) to this array of QVector3D values.
*/

/*!
    Translates the elements in this array of QVector3D values
    by the components of \a value.

    \sa translated()
*/
void QVector3DArray::translate(const QVector3D& value)
{
    if (isDetached()) {
        // Modify the array in-place.
        int size = count();
        QVector3D *dst = data();
        for (int index = 0; index < size; ++index)
            *dst++ += value;
    } else {
        // Create a new array, translate the values, and assign.
        QArray<QVector3D> result;
        int size = count();
        const QVector3D *src = constData();
        QVector3D *dst = result.extend(size);
        for (int index = 0; index < size; ++index)
            *dst++ = *src++ + value;
        *this = result;
    }
}

/*!
    \fn void QVector3DArray::translate(qreal x, qreal y, qreal z)
    \overload

    Translates the elements in this array of QVector3D values
    by (\a x, \a y, \a z).

    \sa translated()
*/

/*!
    Returns a copy of this array of QVector3D values, translated
    by the components of \a value.

    \sa translate()
*/
QArray<QVector3D> QVector3DArray::translated(const QVector3D& value) const
{
    QArray<QVector3D> result;
    int size = count();
    const QVector3D *src = constData();
    QVector3D *dst = result.extend(size);
    for (int index = 0; index < size; ++index)
        *dst++ = *src++ + value;
    return result;
}

/*!
    \fn QArray<QVector3D> QVector3DArray::translated(qreal x, qreal y, qreal z) const
    \overload

    Returns a copy of this array of QVector3D values, translated
    by (\a x, \a y, \a z).

    \sa translate()
*/

/*!
    Transforms the elements in this array of QVector3D values
    by \a matrix.

    \sa transformed()
*/
void QVector3DArray::transform(const QMatrix4x4& matrix)
{
    if (isDetached()) {
        // Modify the array in-place.
        int size = count();
        QVector3D *dst = data();
        for (int index = 0; index < size; ++index) {
            *dst = matrix * *dst;
            ++dst;
        }
    } else {
        // Create a new array, transform the values, and assign.
        QArray<QVector3D> result;
        int size = count();
        const QVector3D *src = constData();
        QVector3D *dst = result.extend(size);
        for (int index = 0; index < size; ++index)
            *dst++ = matrix * *src++;
        *this = result;
    }
}

/*!
    Returns a copy of this array of QVector3D values, transformed
    by \a matrix.

    \sa transform()
*/
QArray<QVector3D> QVector3DArray::transformed(const QMatrix4x4& matrix) const
{
    QArray<QVector3D> result;
    int size = count();
    const QVector3D *src = constData();
    QVector3D *dst = result.extend(size);
    for (int index = 0; index < size; ++index)
        *dst++ = matrix * *src++;
    return result;
}

/*!
    \class QVector4DArray
    \brief The QVector4DArray class is a convenience for wrapping a QArray of QVector4D values.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers

    QVector4DArray is used to build an array of 4D vector values
    based on floating-point x, y, and z arguments:

    \code
    QVector4DArray array;
    array.append(1.0f, 2.0f, 3.0f, -4.0f);
    array.append(-1.0f, 2.0f, 3.0f, -4.0f);
    array.append(1.0f, -2.0f, 3.0f, -4.0f);
    \endcode

    This is more convenient and readable than the equivalent with
    QArray:

    \code
    QArray<QVector4D> array;
    array.append(QVector4D(1.0f, 2.0f, 3.0f, -4.0f));
    array.append(QVector4D(-1.0f, 2.0f, 3.0f, -4.0f));
    array.append(QVector4D(1.0f, -2.0f, 3.0f, -4.0f));
    \endcode

    QVector4DArray also has convenience functions for transforming
    the contents of the array with translate(), translated(),
    transform(), and transformed().

    \sa QArray, QVector2DArray, QVector3DArray
*/

/*!
    \fn QVector4DArray::QVector4DArray()

    Constructs an empty array of QVector4D values.
*/

/*!
    \fn QVector4DArray::QVector4DArray(int size, const QVector4D& value)

    Constructs an array of QVector4D values with an initial \a size.
    All elements in the array are initialized to \a value.
*/

/*!
    \fn QVector4DArray::QVector4DArray(const QArray<QVector4D>& other)

    Constructs a copy of \a other.
*/

/*!
    \fn void QVector4DArray::append(qreal x, qreal y, qreal z, qreal w)

    Appends (\a x, \a y, \a z, \a w) to this array of QVector4D values.
*/

/*!
    Translates the elements in this array of QVector4D values
    by the components of \a value.

    \sa translated()
*/
void QVector4DArray::translate(const QVector4D& value)
{
    if (isDetached()) {
        // Modify the array in-place.
        int size = count();
        QVector4D *dst = data();
        for (int index = 0; index < size; ++index)
            *dst++ += value;
    } else {
        // Create a new array, translate the values, and assign.
        QArray<QVector4D> result;
        int size = count();
        const QVector4D *src = constData();
        QVector4D *dst = result.extend(size);
        for (int index = 0; index < size; ++index)
            *dst++ = *src++ + value;
        *this = result;
    }
}

/*!
    \fn void QVector4DArray::translate(qreal x, qreal y, qreal z, qreal w);
    \overload

    Translates the elements in this array of QVector4D values
    by (\a x, \a y, \a z, \a w).

    \sa translated()
*/

/*!
    Returns a copy of this array of QVector4D values, translated
    by the components of \a value.

    \sa translate()
*/
QArray<QVector4D> QVector4DArray::translated(const QVector4D& value) const
{
    QArray<QVector4D> result;
    int size = count();
    const QVector4D *src = constData();
    QVector4D *dst = result.extend(size);
    for (int index = 0; index < size; ++index)
        *dst++ = *src++ + value;
    return result;
}

/*!
    \fn QArray<QVector4D> QVector4DArray::translated(qreal x, qreal y, qreal z, qreal w) const
    \overload

    Returns a copy of this array of QVector4D values, translated
    by (\a x, \a y, \a z, \a w).

    \sa translate()
*/

/*!
    Transforms the elements in this array of QVector4D values
    by \a matrix.

    \sa transformed()
*/
void QVector4DArray::transform(const QMatrix4x4& matrix)
{
    if (isDetached()) {
        // Modify the array in-place.
        int size = count();
        QVector4D *dst = data();
        for (int index = 0; index < size; ++index) {
            *dst = matrix * *dst;
            ++dst;
        }
    } else {
        // Create a new array, transform the values, and assign.
        QArray<QVector4D> result;
        int size = count();
        const QVector4D *src = constData();
        QVector4D *dst = result.extend(size);
        for (int index = 0; index < size; ++index)
            *dst++ = matrix * *src++;
        *this = result;
    }
}

/*!
    Returns a copy of this array of QVector3D values, transformed
    by \a matrix.

    \sa transform()
*/
QArray<QVector4D> QVector4DArray::transformed(const QMatrix4x4& matrix) const
{
    QArray<QVector4D> result;
    int size = count();
    const QVector4D *src = constData();
    QVector4D *dst = result.extend(size);
    for (int index = 0; index < size; ++index)
        *dst++ = matrix * *src++;
    return result;
}

QT_END_NAMESPACE
