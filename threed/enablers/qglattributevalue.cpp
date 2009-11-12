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

#include "qglattributevalue.h"

QT_BEGIN_NAMESPACE

/*!
    \enum QGL::ComponentType
    \since 4.6
    This enum defines the type of a vertex attribute component.

    \value Byte Signed 8-bit byte.
    \value UByte Unsigned 8-bit byte.
    \value Short Signed 16-bit integer.
    \value UShort Unsigned 16-bit integer.
    \value Int Signed 32-bit integer.
    \value UInt Unsigned 32-bit integer.
    \value Float Single-precision floating point value.
*/

/*!
    \class QGLAttributeValue
    \brief The QGLAttributeValue class encapsulates information about an OpenGL attribute value.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::enablers

    OpenGL has many functions that take a pointer to vertex attribute
    values: \c{glVertexPointer()}, \c{glNormalPointer()},
    \c{glVertexAttribPointer()}, etc.  These functions typically
    take four arguments: size, component type (e.g. GL_FLOAT), stride,
    and data pointer (\c{glNormalPointer()} does not use size, assuming
    that it is 3).  When used with vertex buffers, the data pointer
    may be an offset into the vertex buffer instead.

    QGLAttributeValue encapsulates these four values so that they can
    be easily manipulated as a set during OpenGL painting operations.
    The most common source of QGLAttributeValue instances is as a
    result of calling QGLVertexArray::attributeValue().

    Because the data() value is a raw pointer to arbitrary memory,
    care should be taken that the memory remains valid until the
    QGLAttributeValue is no longer required.

    \sa QGLVertexArray
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue()

    Constructs a null attribute value with default parameters of
    size(), stride(), and offset() set to zero, type() set to
    QGL::Float, and data() set to null.

    \sa isNull()
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(int size, QGL::ComponentType type, int stride, const void *data)

    Constructs an attribute value with the fields \a size, \a type,
    \a stride, and \a data.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(int size, QGL::ComponentType type, int stride, int offset)

    Constructs an attribute value with the fields \a size, \a type,
    \a stride, and \a offset.
*/

/*!
    \fn bool QGLAttributeValue::isNull() const

    Returns true if size() is zero, which indicates an unset
    attribute value; false otherwise.

    Note: it is possible for data() to be null, but isNull() returns true.
    This can happen when data() is actually a zero offset() into a
    vertex buffer.

    \sa offset()
*/

/*!
    \fn QGL::ComponentType QGLAttributeValue::type() const

    Returns the component type for this attribute value.  The default
    value is QGL::Float.

    \sa setType()
*/

/*!
    \fn void QGLAttributeValue::setType(QGL::ComponentType value)

    Sets the component type for this attribute value to \a value.

    \sa type()
*/

/*!
    \fn int QGLAttributeValue::size() const

    Returns the size of this attribute in components.  For example,
    a return value of 3 indicates a vector of 3-dimensional values.
    If size() is zero, then this attribute value is null.

    \sa setSize(), isNull()
*/

/*!
    \fn void QGLAttributeValue::setSize(int value)

    Sets the size of this attribute in components to \a value,
    which must be between 1 and 4.

    \sa size()
*/

/*!
    \fn int QGLAttributeValue::stride() const

    Returns the stride in bytes from one vertex element to the
    next for this attribute value.  The default value of 0 indicates
    that the elements are tightly packed within the data() array.

    \sa setStride()
*/

/*!
    \fn void QGLAttributeValue::setStride(int value)

    Sets the stride in bytes from one vertex element to the
    next to \a value.  If \a value is 0, then the elements are
    tightly packed within the data() array.

    \sa stride()
*/

/*!
    \fn size_t QGLAttributeValue::offset() const

    Returns the vertex buffer offset for this attribute value.

    This function is a convenience that returns data() cast
    to an integer offset value.

    \sa setOffset(), data()
*/

/*!
    \fn void QGLAttributeValue::setOffset(size_t value)

    Sets the vertex buffer offset for this attribute value to \a value.

    This function is a convenience that casts \a value to a pointer
    and then calls setData().

    \sa offset(), setData()
*/

/*!
    \fn const void *QGLAttributeValue::data() const

    Returns the data pointer for the elements in this attribute value.

    \sa setData(), offset(), floatData()
*/

/*!
    \fn void QGLAttributeValue::setData(const void *value)

    Sets the data pointer for the elements in this attribute value
    to \a value.

    Because the \a value is a raw pointer to arbitrary memory,
    care should be taken that the memory remains valid until the
    QGLAttributeValue is no longer required.

    \sa data(), setOffset()
*/

/*!
    \fn const float *QGLAttributeValue::floatData() const

    Returns the data pointer for the elements in this attribute value
    as a pointer to float.

    This is a convenience for the common case of passing data() to a
    function that expects a GLfloat * argument.

    \sa data()
*/

QT_END_NAMESPACE
