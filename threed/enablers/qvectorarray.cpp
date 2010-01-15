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

#include "qvectorarray.h"

QT_BEGIN_NAMESPACE

/*!
    \class QVector2DArray
    \brief The QVector2DArray class is a convenience for wrapping a QDataArray of QVector2D values.
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
    QDataArray:

    \code
    QDataArray<QVector2D> array;
    array.append(QVector2D(1.0f, 2.0f));
    array.append(QVector2D(-1.0f, 2.0f));
    array.append(QVector2D(1.0f, -2.0f));
    \endcode

    In all other respects QVector2DArray is identical to
    QDataArray<QVector2D>.

    \sa QDataArray, QVector3DArray, QVector4DArray
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

/*
    \fn QVector2DArray::QVector2DArray(const QDataArray<QVector2D>& other)

    Constructs a copy of \a other.
*/

/*!
    \fn void QVector2DArray::append(const QVector2D& value)

    Appends \a value to this array of QVector2D values.
*/

/*!
    \fn void QVector2DArray::append(qreal x, qreal y)

    Appends (\a x, \a y) to this array of QVector2D values.
*/

/*!
    \class QVector3DArray
    \brief The QVector3DArray class is a convenience for wrapping a QDataArray of QVector3D values.
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
    QDataArray:

    \code
    QDataArray<QVector3D> array;
    array.append(QVector3D(1.0f, 2.0f, 3.0f));
    array.append(QVector3D(-1.0f, 2.0f, 3.0f));
    array.append(QVector3D(1.0f, -2.0f, 3.0f));
    \endcode

    In all other respects QVector3DArray is identical to
    QDataArray<QVector3D>.

    \sa QDataArray, QVector2DArray, QVector4DArray
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
    \fn QVector3DArray::QVector3DArray(const QDataArray<QVector3D>& other)

    Constructs a copy of \a other.
*/

/*!
    \fn void QVector3DArray::append(const QVector3D& value)

    Appends \a value to this array of QVector3D values.
*/

/*!
    \fn void QVector3DArray::append(qreal x, qreal y, qreal z)

    Appends (\a x, \a y, \a z) to this array of QVector3D values.
*/

/*!
    \class QVector4DArray
    \brief The QVector4DArray class is a convenience for wrapping a QDataArray of QVector4D values.
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
    QDataArray:

    \code
    QDataArray<QVector4D> array;
    array.append(QVector4D(1.0f, 2.0f, 3.0f, -4.0f));
    array.append(QVector4D(-1.0f, 2.0f, 3.0f, -4.0f));
    array.append(QVector4D(1.0f, -2.0f, 3.0f, -4.0f));
    \endcode

    In all other respects QVector4DArray is identical to
    QDataArray<QVector4D>.

    \sa QDataArray, QVector2DArray, QVector3DArray
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
    \fn QVector4DArray::QVector4DArray(const QDataArray<QVector4D>& other)

    Constructs a copy of \a other.
*/

/*!
    \fn void QVector4DArray::append(const QVector2D& value)

    Appends \a value to this array of QVector4D values.
*/

/*!
    \fn void QVector4DArray::append(qreal x, qreal y, qreal z, qreal w)

    Appends (\a x, \a y, \a z, \a w) to this array of QVector4D values.
*/

QT_END_NAMESPACE
