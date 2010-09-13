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

#include "qglattributevalue.h"
#include "qglfunctions.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLAttributeValue
    \brief The QGLAttributeValue class encapsulates information about an OpenGL attribute value.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::enablers

    OpenGL has many functions that take a pointer to vertex attribute
    values: \c{glVertexPointer()}, \c{glNormalPointer()},
    \c{glVertexAttribPointer()}, etc.  These functions typically
    take four arguments: tuple size (1, 2, 3, or 4), component type
    (e.g. GL_FLOAT), stride, and data pointer (\c{glNormalPointer()}
    does not use tuple size, assuming that it is 3).  When used with
    vertex buffers, the data pointer may be an offset into the vertex
    buffer instead.

    QGLAttributeValue encapsulates these four values so that they can
    be easily manipulated as a set during OpenGL painting operations.
    Constructors are provided for converting QArray and
    QCustomDataArray objects into an attribute value.

    Because the data() value is a raw pointer to arbitrary memory,
    care should be taken that the memory remains valid until the
    QGLAttributeValue is no longer required.

    \sa QGLAttributeDescription, QArray, QCustomDataArray
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue()

    Constructs a null attribute value with default parameters of
    tupleSize(), stride(), and offset() set to zero, type() set to
    GL_FLOAT, and data() set to null.

    \sa isNull()
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QArray<float>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 1, type() to GL_FLOAT, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QArray<QVector2D>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 2, type() to GL_FLOAT, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QArray<QVector3D>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 3, type() to GL_FLOAT, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QArray<QVector4D>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 4, type() to GL_FLOAT, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QArray<QColor4ub>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 4, type() to GL_UNSIGNED_BYTE, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    Constructs an attribute value that refers to the contents of \a array.
    The tupleSize() and type() of the attribute value will be set according
    to the QCustomDataArray::elementType() of \a array.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/
QGLAttributeValue::QGLAttributeValue(const QCustomDataArray& array)
    : m_data(array.data()), m_count(0)
{
    switch (array.elementType()) {
    case QCustomDataArray::Float:
        m_description.setTupleSize(1);
        m_description.setType(GL_FLOAT);
        break;
    case QCustomDataArray::Vector2D:
        m_description.setTupleSize(2);
        m_description.setType(GL_FLOAT);
        break;
    case QCustomDataArray::Vector3D:
        m_description.setTupleSize(3);
        m_description.setType(GL_FLOAT);
        break;
    case QCustomDataArray::Vector4D:
        m_description.setTupleSize(4);
        m_description.setType(GL_FLOAT);
        break;
    case QCustomDataArray::Color:
        m_description.setTupleSize(4);
        m_description.setType(GL_UNSIGNED_BYTE);
        break;
    default:
        // Just in case: set the object to null.
        m_description.setTupleSize(0);
        m_description.setType(GL_FLOAT);
        m_data = 0;
        break;
    }
}

/*!
    \fn QGLAttributeValue::QGLAttributeValue(int tupleSize, GLenum type, int stride, const void *data, int count)

    Constructs an attribute value with the fields \a tupleSize, \a type,
    \a stride, \a data, and \a count.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(int tupleSize, GLenum type, int stride, int offset, int count)

    Constructs an attribute value with the fields \a tupleSize, \a type,
    \a stride, \a offset, and \a count.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QGLAttributeDescription& description, const void *data, int count)

    Constructs an attribute value with the supplied \a description,
    \a data, and \a count.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QGLAttributeDescription& description, int offset, int count)

    Constructs an attribute value with the supplied \a description,
    \a offset, and \a count.
*/

/*!
    \fn bool QGLAttributeValue::isNull() const

    Returns true if tupleSize() is zero, which indicates an unset
    attribute value; false otherwise.

    Note: it is possible for data() to be null, but isNull() returns true.
    This can happen when data() is actually a zero offset() into a
    vertex buffer.

    \sa offset()
*/

/*!
    \fn const QGLAttributeDescription &QGLAttributeValue::description() const

    Returns a reference to the description of this attribute.

    \sa type()
*/

/*!
    \fn GLenum QGLAttributeValue::type() const

    Returns the component type for this attribute value.  The default
    value is GL_FLOAT.

    \sa sizeOfType(), description()
*/

/*!
    Returns the size in bytes of type().

    \sa type(), tupleSize()
*/
int QGLAttributeValue::sizeOfType() const
{
    switch (m_description.type()) {
    case GL_BYTE:           return int(sizeof(GLbyte));
    case GL_UNSIGNED_BYTE:  return int(sizeof(GLubyte));
    case GL_SHORT:          return int(sizeof(GLshort));
    case GL_UNSIGNED_SHORT: return int(sizeof(GLushort));
    case GL_INT:            return int(sizeof(GLint));
    case GL_UNSIGNED_INT:   return int(sizeof(GLuint));
    case GL_FLOAT:          return int(sizeof(GLfloat));
#if defined(GL_DOUBLE) && !defined(QT_OPENGL_ES)
    case GL_DOUBLE:         return int(sizeof(GLdouble));
#endif
    default:                return 0;
    }
}

/*!
    \fn int QGLAttributeValue::tupleSize() const

    Returns the tuple size of this attribute in components.  For example,
    a return value of 3 indicates a vector of 3-dimensional values.
    If tupleSize() is zero, then this attribute value is null.

    \sa isNull(), sizeOfType()
*/

/*!
    \fn int QGLAttributeValue::stride() const

    Returns the stride in bytes from one vertex element to the
    next for this attribute value.  The default value of 0 indicates
    that the elements are tightly packed within the data() array.
*/

/*!
    \fn int QGLAttributeValue::offset() const

    Returns the vertex buffer offset for this attribute value.

    This function is a convenience that returns data() cast
    to an integer offset value.

    \sa data()
*/

/*!
    \fn const void *QGLAttributeValue::data() const

    Returns the data pointer for the elements in this attribute value.

    \sa offset(), floatData()
*/

/*!
    \fn const float *QGLAttributeValue::floatData() const

    Returns the data pointer for the elements in this attribute value
    as a pointer to float.

    This is a convenience for the common case of passing data() to a
    function that expects a GLfloat * argument.

    \sa data()
*/

/*!
    \fn int QGLAttributeValue::count() const

    Returns the count of vertex elements in this attribute value;
    zero if the count is unknown.
*/

QT_END_NAMESPACE
