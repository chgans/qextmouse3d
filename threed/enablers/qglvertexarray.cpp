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

#include "qglvertexarray.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLVertexArray
    \brief The QGLVertexArray class manages arrays of vertex attributes.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::enablers

    3D graphical applications need to deal with large numbers of vertices
    that specify the attributes of an object in 3D space.  The QGLVertexArray
    class makes it easier to construct and manage such vertex arrays.

    Vertices can have multiple types of information associated with them:
    positions, normals, colors, texture co-ordinates, etc.  The
    QGLVertexArray class supports this through the use of fields that
    define the layout of the data in the array.

    Each field has a vertex attribute type, a size in components
    (1, 2, 3, or 4), and an offset with each vertex definition.
    The sum of all field sizes is the vertex array's stride().

    The following example creates a vertex array with two fields:
    a 3D position and a 2D texture co-ordinate:

    \code
    QGLVertexArray array;
    array.addField(QGL::Position, 3);
    array.addField(QGL::TextureCoord0, 2);
    \endcode

    Once the layout of the vertex array has been specified, field values
    are specified using append():

    \code
    array.append(2.0f, 0.0f, 0.0f);
    array.append(1.0f, 0.0f);
    array.append(2.0f, 3.0f, 0.0f);
    array.append(1.0f, 1.0f);
    array.append(0.0f, 3.0f, 0.0f);
    array.append(0.0f, 1.0f);
    \endcode
*/

/*!
    \enum QGL::VertexAttribute
    \since 4.6
    This enum defines the type of vertex attribute to set on an effect with QGLPainter::setVertexArray()

    \value Position The primary position of the vertex.
    \value Normal The normal at each vertex.
    \value Color The color at each vertex.
    \value TextureCoord0 The texture co-ordinate at each vertex for texture unit 0.
    \value TextureCoord1 The texture co-ordinate at each vertex for texture unit 1.
    \value TextureCoord2 The texture co-ordinate at each vertex for texture unit 2.
    \value TextureCoord3 The texture co-ordinate at each vertex for texture unit 3.
    \value TextureCoord4 The texture co-ordinate at each vertex for texture unit 4.
    \value TextureCoord5 The texture co-ordinate at each vertex for texture unit 5.
    \value TextureCoord6 The texture co-ordinate at each vertex for texture unit 6.
    \value TextureCoord7 The texture co-ordinate at each vertex for texture unit 7.
    \value CustomVertex0 First custom vertex attribute.
    \value CustomVertex1 Second custom vertex attribute.
    \value CustomVertex2 Third custom vertex attribute.
    \value CustomVertex3 Fourth custom vertex attribute.
    \value CustomVertex4 Fifth custom vertex attribute.
    \value CustomVertex5 Sixth custom vertex attribute.
    \value CustomVertex6 Seventh custom vertex attribute.
    \value CustomVertex7 Eighth custom vertex attribute.
*/

/*!
    \fn QGLVertexArray::QGLVertexArray()

    Constructs an empty vertex array.  This is usually followed
    by calls to addField() to define the field layout.  For example:

    \code
    QGLVertexArray array;
    array.addField(QGL::Position, 3);
    array.addField(QGL::Normal, 3);
    array.addField(QGL::Color, 4);
    array.addField(QGL::TextureCoord0, 2);
    array.addField(QGL::TextureCoord1, 2);
    \endcode

    \sa addField()
*/

/*!
    \fn QGLVertexArray::QGLVertexArray(QGL::VertexAttribute attr, int size)

    Constructs an empty vertex array with a single field defined
    by \a attr and \a size.  For example:

    \code
    QGLVertexArray array(QGL::Position, 3);
    array.append(2.0f, 0.0f, 0.0f);
    array.append(2.0f, 3.0f, 0.0f);
    array.append(0.0f, 3.0f, 0.0f);
    \endcode

    The \a size parameter must be between 1 and 4, inclusive.
*/

/*!
    \fn QGLVertexArray::QGLVertexArray(QGL::VertexAttribute attr1, int size1, QGL::VertexAttribute attr2, int size2)

    Constructs an empty vertex array with a two fields defined
    by \a attr1, \a size1, \a attr2, and \a size2.  For example:

    \code
    QGLVertexArray array(QGL::Position, 3, QGL::TextureCoord0, 2);
    array.append(2.0f, 0.0f, 0.0f);
    array.append(1.0f, 0.0f);
    array.append(2.0f, 3.0f, 0.0f);
    array.append(1.0f, 1.0f);
    array.append(0.0f, 3.0f, 0.0f);
    array.append(0.0f, 1.0f);
    \endcode

    The \a size1 and \a size2 parameters must be between 1 and 4, inclusive.
*/

/*!
    \fn QGLVertexArray::QGLVertexArray(QGL::VertexAttribute attr1, int size1, QGL::VertexAttribute attr2, int size2, QGL::VertexAttribute attr3, int size3)

    Constructs an empty vertex array with a three fields defined
    by \a attr1, \a size1, \a attr2, \a size2, \a attr3, and \a size3.
    For example:

    \code
    QGLVertexArray array(QGL::Position, 3, QGL::TextureCoord0, 2,
                         QGL::Color, 4);
    array.append(2.0f, 0.0f, 0.0f);
    array.append(1.0f, 0.0f);
    array.append(0.5f, 0.0f, 0.0f, 1.0f);
    array.append(2.0f, 3.0f, 0.0f);
    array.append(1.0f, 1.0f);
    array.append(0.0f, 0.5f, 0.0f, 1.0f);
    array.append(0.0f, 3.0f, 0.0f);
    array.append(0.0f, 1.0f);
    array.append(0.0f, 0.0f, 0.5f, 1.0f);
    \endcode

    The \a size1, \a size2, and \a size3 parameters must be between
    1 and 4, inclusive.
*/

/*!
    \fn QGLVertexArray::QGLVertexArray(QGL::VertexAttribute attr1, int size1, QGL::VertexAttribute attr2, int size2, QGL::VertexAttribute attr3, int size3, QGL::VertexAttribute attr4, int size4)

    Constructs an empty vertex array with a four fields defined
    by \a attr1, \a size1, \a attr2, \a size2, \a attr3, \a size3,
    \a attr4, and \a size4.  For example:

    \code
    QGLVertexArray array(QGL::Position, 3, QGL::Normal, 3,
                         QGL::TextureCoord0, 2, QGL::Color, 4);
    array.append(2.0f, 0.0f, 0.0f);
    array.append(0.0f, 0.0f, 1.0f);
    array.append(1.0f, 0.0f);
    array.append(0.5f, 0.0f, 0.0f, 1.0f);
    array.append(2.0f, 3.0f, 0.0f);
    array.append(0.0f, 0.0f, 1.0f);
    array.append(1.0f, 1.0f);
    array.append(0.0f, 0.5f, 0.0f, 1.0f);
    array.append(0.0f, 3.0f, 0.0f);
    array.append(0.0f, 0.0f, 1.0f);
    array.append(0.0f, 1.0f);
    array.append(0.0f, 0.0f, 0.5f, 1.0f);
    \endcode

    The \a size1, \a size2, \a size3, and \a size4 parameters must be
    between 1 and 4, inclusive.
*/

/*!
    \fn QGLVertexArray::QGLVertexArray(const QGLVertexArray& other)

    Constructs a copy of \a other.

    This operation takes constant time, because the data contents are
    implicitly shared.  This makes returning a QGLVertexArray from a
    function very fast.  If a shared instance is modified, it will be
    copied (copy-on-write), and that takes linear time.

    \sa operator=()
*/

/*!
    \fn QGLVertexArray::~QGLVertexArray()

    Destroys this vertex array.
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator=(const QGLVertexArray& other)

    Assigns \a other to this vertex array and returns a reference
    to this vertex array.  This operation takes constant time.
*/

/*!
    \fn void QGLVertexArray::addField(QGL::VertexAttribute attr, int size)

    Adds a field to this vertex array with the details \a attr, and \a size.
    The offset of the field is determined from stride().  The \a size must
    be between 1 and 4, inclusive.

    If the vertex array contains existing data, this will define the layout
    of that existing data but not change the existing data.  Use the
    interleaved() function to add fields and expand the data
    to include space for them.

    \sa stride(), fieldCount(), interleaved()
*/

/*!
    \fn int QGLVertexArray::fieldCount() const

    Returns the number of fields in this vertex array.

    \sa addField(), stride()
*/

/*!
    \fn QGLVertexDescription QGLVertexArray::fields() const

    Returns the field description list.

    \sa setFields()
*/

/*!
    \fn void QGLVertexArray::setFields(const QGLVertexDescription& value)

    Sets the field description list to \a value.

    \sa fields()
*/

/*!
    \fn QGLAttributeValue QGLVertexArray::attributeValue(int field) const

    Returns the attribute value for the index \a field; or a null
    QGLAttributeValue if \a field is out of range.
*/

/*!
    \fn QGLAttributeValue QGLVertexArray::attributeValue(QGL::VertexAttribute attr) const

    Returns the attribute value for the field associated with \a attr;
    or a null QGLAttributeValue if \a attr is not a field of this array.
*/

/*!
    \fn int QGLVertexArray::fieldIndex(QGL::VertexAttribute attr) const

    Returns the index of the field associated with \a attr in this array;
    or -1 if \a attr is not a field of this array.
*/

/*!
    \fn int QGLVertexArray::stride() const

    Returns the stride of this vertex array, which is the sum
    of the sizes of all fields.

    \sa addField()
*/

/*!
    \fn int QGLVertexArray::vertexCount() const

    Returns the number of vertices in this vertex array, which is
    the same as componentCount() / stride().  If stride() is zero,
    because no fields have been defined yet, then this function
    will return componentCount().

    \sa componentCount(), stride()
*/

/*!
    \fn int QGLVertexArray::componentCount() const

    Returns the number of components in this vertex array.

    \sa vertexCount()
*/

/*!
    \fn bool QGLVertexArray::isEmpty() const

    Returns true if this vertex array is empty; false otherwise.
*/

/*!
    Resizes this vertex array to \a size vertices, using stride()
    to determine the number of components per vertex.  Additional
    components are filled with zeroes.

    \sa reserve()
*/
void QGLVertexArray::resize(int size)
{
    m_data.resize(size * stride());
}

/*!
    Reserves space in this vertex array for \a size vertices.
    If the vertex array already contains space for \a size vertices
    or more, this function does nothing.  Note: the \a size refers
    to vertices, not components.  Use stride() to determine the
    number of components per vertex.

    The following example creates a vertex array containing 3D
    position and 2D texture co-ordinate values, and then reserves
    enough space for 100 vertices:

    \code
    QGLVertexArray array(QGL::Position, 3, QGL::TextureCoord0, 2);
    array.reserve(100);
    \endcode

    \sa componentCount(), resize()
*/
void QGLVertexArray::reserve(int size)
{
    m_data.reserve(size * stride());
}

/*!
    \fn void QGLVertexArray::append(qreal x)

    Appends a 1D vertex \a x to this vertex array.
*/

/*!
    \fn void QGLVertexArray::append(qreal x, qreal y)

    Appends a 2D vertex (\a x, \a y) to this vertex array.
*/

/*!
    \fn void QGLVertexArray::append(qreal x, qreal y, qreal z)

    Appends a 3D vertex (\a x, \a y, \a z) to this vertex array.
*/

/*!
    \fn void QGLVertexArray::append(qreal x, qreal y, qreal z, qreal w)

    Appends a 4D vertex (\a x, \a y, \a z, \a w) to this vertex array.
*/

/*!
    \fn void QGLVertexArray::append(const QVector2D& value)

    Appends a 2D vertex \a value to this vertex array.
*/

/*!
    \fn void QGLVertexArray::append(const QVector3D& value)

    Appends a 3D vertex \a value to this vertex array.
*/

/*!
    \fn void QGLVertexArray::append(const QVector4D& value)

    Appends a 4D vertex \a value to this vertex array.
*/

/*!
    \fn void QGLVertexArray::append(const QPoint& value)

    Appends a 2D point \a value to this vertex array.
*/

/*!
    \fn void QGLVertexArray::append(const QPointF& value)

    Appends a 2D point \a value to this vertex array.
*/

/*!
    \fn void QGLVertexArray::append(const QColor4B& value)

    Appends a four byte color \a value to this vertex array.
    The field should have a size of 1.
*/

/*!
    Appends the components in \a other to this vertex array.  It is
    assumed that \a other has the same set of fields as this vertex array.
*/
void QGLVertexArray::append(const QGLVertexArray& other)
{
    detachBuffer();
    m_data.append(other.m_data);
}

/*!
    \fn qreal QGLVertexArray::floatAt(int vertex, int field) const

    Returns the specified \a field of \a vertex as a 1D value.
    The \a vertex should be between 0 and vertexCount() - 1,
    and the \a field must be between 0 and fieldCount() - 1.

    \sa setAt(), vector2DAt(), vector3DAt(), vector4DAt()
*/

/*!
    \fn QVector2D QGLVertexArray::vector2DAt(int vertex, int field) const

    Returns the specified \a field of \a vertex as a 2D value.
    The \a vertex should be between 0 and vertexCount() - 1,
    and the \a field must be between 0 and fieldCount() - 1.

    \sa setAt(), floatAt(), vector3DAt(), vector4DAt()
*/

/*!
    \fn QVector3D QGLVertexArray::vector3DAt(int vertex, int field) const

    Returns the specified \a field of \a vertex as a 3D value.
    The \a vertex should be between 0 and vertexCount() - 1,
    and the \a field must be between 0 and fieldCount() - 1.

    \sa setAt(), floatAt(), vector2DAt(), vector4DAt()
*/

/*!
    \fn QVector4D QGLVertexArray::vector4DAt(int vertex, int field) const

    Returns the specified \a field of \a vertex as a 4D value.
    The \a vertex should be between 0 and vertexCount() - 1,
    and the \a field must be between 0 and fieldCount() - 1.

    \sa setAt(), floatAt(), vector2DAt(), vector3DAt()
*/

/*!
    \fn QVector4D QGLVertexArray::color4bAt(int vertex, int field) const

    Returns the specified \a field of \a vertex as a four byte color value.
    The \a vertex should be between 0 and vertexCount() - 1,
    and the \a field must be between 0 and fieldCount() - 1.

    \sa setAt(), floatAt(), vector4DAt()
*/

/*!
    \fn void QGLVertexArray::setAt(int vertex, int field, qreal x)

    Sets the specified \a field of \a vertex to a 1D value \a x.
    The \a vertex should be between 0 and vertexCount() - 1,
    and the \a field must be between 0 and fieldCount() - 1.

    \sa floatAt()
*/

/*!
    \fn void QGLVertexArray::setAt(int vertex, int field, qreal x, qreal y)

    Sets the specified \a field of \a vertex to a 2D value (\a x, \a y).
    The \a vertex should be between 0 and vertexCount() - 1,
    and the \a field must be between 0 and fieldCount() - 1.

    \sa vector2DAt()
*/

/*!
    \fn void QGLVertexArray::setAt(int vertex, int field, qreal x, qreal y, qreal z)

    Sets the specified \a field of \a vertex to a 3D value
    (\a x, \a y, \a z).  The \a vertex should be between
    0 and vertexCount() - 1, and the \a field must be between
    0 and fieldCount() - 1.

    \sa vector3DAt()
*/

/*!
    \fn void QGLVertexArray::setAt(int vertex, int field, qreal x, qreal y, qreal z, qreal w)

    Sets the specified \a field of \a vertex to a 4D value
    (\a x, \a y, \a z, \a w).  The \a vertex should be between
    0 and vertexCount() - 1, and the \a field must be between
    0 and fieldCount() - 1.

    \sa vector4DAt()
*/

/*!
    \fn void QGLVertexArray::setAt(int vertex, int field, const QVector2D& value)

    Sets the specified \a field of \a vertex to a 2D \a value.
    The \a vertex should be between 0 and vertexCount() - 1,
    and the \a field must be between 0 and fieldCount() - 1.

    \sa vector2DAt()
*/

/*!
    \fn void QGLVertexArray::setAt(int vertex, int field, const QVector3D& value)

    Sets the specified \a field of \a vertex to a 3D \a value.
    The \a vertex should be between 0 and vertexCount() - 1,
    and the \a field must be between 0 and fieldCount() - 1.

    \sa vector3DAt()
*/

/*!
    \fn void QGLVertexArray::setAt(int vertex, int field, const QVector4D& value)

    Sets the specified \a field of \a vertex to a 4D \a value.
    The \a vertex should be between 0 and vertexCount() - 1,
    and the \a field must be between 0 and fieldCount() - 1.

    \sa vector4DAt()
*/

/*!
    \fn void QGLVertexArray::setAt(int vertex, int field, const QColor4B& value)

    Sets the specified \a field of \a vertex to a four byte color \a value.
    The \a vertex should be between 0 and vertexCount() - 1,
    and the \a field must be between 0 and fieldCount() - 1.

    \sa color4bAt()
*/

/*!
    Extracts and returns a new vertex array that consists of
    just the vertex attribute values for \a field.  Returns an
    empty vertex array if \a field is out of range.

    \sa interleaved()
*/
QGLVertexArray QGLVertexArray::extractField(int field) const
{
    // Bail out if the field index is invalid.
    if (field < 0 || field >= m_fields.fieldCount())
        return QGLVertexArray();

    // If there is only one field in this array, then return it as-is.
    if (m_fields.fieldCount() == 1)
        return *this;

    // Construct a new array with just the selected field.
    int size = m_fields.fieldSize(field);
    QGLVertexArray array(m_fields.fieldAttribute(field), size);
    int stride = m_fields.stride();
    int count = vertexCount();
    const float *src = m_data.constData() + m_fields.fieldOffset(field);
    float *dst = array.m_data.extend(count * size);
    for (int item = 0; item < count; ++item) {
        for (int component = 0; component < size; ++component)
            dst[component] = src[component];
        dst += size;
        src += stride;
    }
    return array;
}

/*!
    Extracts and returns a new vertex array that consists of
    just the vertex attribute values associated with the first
    field whose attribute is \a attr.  Returns an empty vertex
    array if \a attr is not present.

    \sa interleaved()
*/
QGLVertexArray QGLVertexArray::extractField(QGL::VertexAttribute attr) const
{
    return extractField(m_fields.indexOf(attr));
}

/*!
    Interleaves the fields and component values in \a other into
    this vertex array and returns the new interleaved vertex array.
    This function is used to combine arrays of vertex attributes
    that have been built separately.

    The following example interleaves two vertex arrays containing
    separate position and color information:

    \code
    QGLVertexArray positions(QGL::Position, 3);
    QGLVertexArray colors(QGL::Color, 4);
    ...

    QGLVertexArray combined = positions.interleaved(colors);
    \endcode

    If this vertex array does not have the same vertexCount()
    as \a other, then the extra components in the result will be
    padded with zeroes.

    \sa addField(), vertexCount(), extractField()
*/
QGLVertexArray QGLVertexArray::interleaved(const QGLVertexArray& other) const
{
    QGLVertexArray result;
    result.m_fields = m_fields;
    result.m_fields.addFields(other.m_fields);

    // Determine the size of the new array.
    int thisCount = vertexCount();
    int otherCount = other.vertexCount();
    int thisStride = stride();
    int otherStride = other.stride();
    int maxCount = qMax(thisCount, otherCount);
    int size = maxCount * (thisStride + otherStride);

    // Reserve space for the interleaved version, but don't initialize it.
    float *data = result.m_data.extend(size);

    // Copy across the elements from the source arrays, padding with
    // zeroes if one of the source arrays is shorter than the other.
    const float *thisData = m_data.constData();
    const float *otherData = other.m_data.constData();
    int component;
    for (int index = 0; index < maxCount; ++index) {
        if (index < thisCount) {
            for (component = 0; component < thisStride; ++component)
                data[component] = thisData[component];
        } else {
            for (component = 0; component < thisStride; ++component)
                data[component] = 0.0f;
        }
        thisData += thisStride;
        data += thisStride;
        if (index < otherCount) {
            for (component = 0; component < otherStride; ++component)
                data[component] = otherData[component];
        } else {
            for (component = 0; component < otherStride; ++component)
                data[component] = 0.0f;
        }
        otherData += otherStride;
        data += otherStride;
    }
    return result;
}

/*!
    \fn float *QGLVertexArray::data()

    Returns the internal data pointer for this vertex array, for
    direct manipulation.  This will create a writable copy of the
    array if the underlying data is shared.  Returns null if the
    array is empty.

    \sa constData(), stride()
*/

/*!
    \fn const float *QGLVertexArray::data() const

    Returns the internal data pointer for this vertex array, for
    direct reading.  Returns null if the array is empty.

    \sa constData(), stride()
*/

/*!
    \fn const float *QGLVertexArray::constData() const

    Returns the internal data pointer for this vertex array, for
    direct reading.  Returns null if the array is empty.

    \sa data(), stride()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator+=(qreal value)

    Appends \a value to this vertex array.

    \sa append(), operator<<()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator+=(const QVector2D& value)

    Appends \a value to this vertex array.

    \sa append(), operator<<()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator+=(const QVector3D& value)

    Appends \a value to this vertex array.

    \sa append(), operator<<()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator+=(const QVector4D& value)

    Appends \a value to this vertex array.

    \sa append(), operator<<()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator+=(const QGLVertexArray& other)

    Appends the contents of \a other to this vertex array.

    \sa append(), operator<<()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator+=(const QPoint& value)

    Appends \a value to this vertex array.

    \sa append(), operator<<()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator+=(const QPointF& value)

    Appends \a value to this vertex array.

    \sa append(), operator<<()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator+=(const QColor4B& value)

    Appends \a value to this vertex array.

    \sa append(), operator<<()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator<<(qreal value)

    Appends \a value to this vertex array.

    \sa append(), operator+=()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator<<(const QVector2D& value)

    Appends \a value to this vertex array.

    \sa append(), operator+=()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator<<(const QVector3D& value)

    Appends \a value to this vertex array.

    \sa append(), operator+=()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator<<(const QVector4D& value)

    Appends \a value to this vertex array.

    \sa append(), operator+=()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator<<(const QGLVertexArray& other)

    Appends the contents of \a other to this vertex array.

    \sa append(), operator+=()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator<<(const QPoint& value)

    Appends \a value to this vertex array.

    \sa append(), operator+=()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator<<(const QPointF& value)

    Appends \a value to this vertex array.

    \sa append(), operator+=()
*/

/*!
    \fn QGLVertexArray& QGLVertexArray::operator<<(const QColor4B& value)

    Appends \a value to this vertex array.

    \sa append(), operator+=()
*/

/*!
    Sets the contents of this vertex array to the \a count component
    values from \a data.  The \a data pointer is copied by this
    function and must remain valid until the vertex array is
    modified or discarded.

    \code
    QGLVertexArray array(QGL::Position, 3, QGL::Normal, 3);
    array.setRawData(vertices, 300);
    \endcode
*/
void QGLVertexArray::setRawData(const float *data, int count)
{
    m_data = QArray<float>::fromRawData(data, count);
}

/*!
    Replaces the contents of this vertex array starting at \a vertex
    with the components from \a data.  It is assumed that \a data
    has the same set of fields as this vertex array.

    If there are more vertices in \a data than in this vertex array,
    then additional vertices will be appended to this vertex array.

    This function does nothing if there are no fields in this
    vertex array, \a vertex is out of range, or \a data is empty.

    \sa append()
*/
void QGLVertexArray::replace(int vertex, const QGLVertexArray& data)
{
    if (!m_fields.stride() || vertex < 0 || vertex > vertexCount()
            || data.isEmpty())
        return;
    detachBuffer();
    m_data.replace
        (vertex * m_fields.stride(), data.m_data.constData(),
         data.m_data.count());
}

void QGLVertexArray::typeCheckAppend(int size) const
{
    if (m_currentField < m_fields.fieldCount()) {
        if (size != m_fields.fieldSize(m_currentField) &&
                (m_currentField >= 32 ||
                 (m_warnings & (1 << m_currentField)) == 0)) {
            if (m_currentField < 32)
                m_warnings |= (1 << m_currentField);
            qWarning("QGLVertexArray: adding a %d-component value to a %d-component field", size, m_fields.fieldSize(m_currentField));
        }
        ++m_currentField;
        if (m_currentField >= m_fields.fieldCount())
            m_currentField = 0;
    }
}

void QGLVertexArray::typeCheckRead(int field, int size) const
{
    if (field >= 0 && field < m_fields.fieldCount()) {
        if (size != m_fields.fieldSize(field) &&
                (m_currentField >= 32 ||
                 (m_warnings & (1 << m_currentField)) == 0)) {
            if (m_currentField < 32)
                m_warnings |= (1 << m_currentField);
            qWarning("QGLVertexArray: reading a %d-component value from a %d-component field", size, m_fields.fieldSize(field));
        }
    }
}

void QGLVertexArray::typeCheckWrite(int field, int size) const
{
    if (field >= 0 && field < m_fields.fieldCount()) {
        if (size != m_fields.fieldSize(field) &&
                (m_currentField >= 32 ||
                 (m_warnings & (1 << m_currentField)) == 0)) {
            if (m_currentField < 32)
                m_warnings |= (1 << m_currentField);
            qWarning("QGLVertexArray: writing a %d-component value to a %d-component field", size, m_fields.fieldSize(field));
        }
    }
}

QGLVertexArray QGLVertexArray::toBufferForm() const
{
    // Create a simulated raw data version of the array where the
    // raw data pointer is set to zero.  This will cause later calls
    // of "array.constData() + field.offset()" to generate an offset
    // into a vertex buffer rather than a client-side pointer.
    QGLVertexArray result;
    result.m_data = QArray<float>::fromRawData(0, m_data.count());
    result.m_fields = m_fields;
    return result;
}

/*!
    \fn bool QGLVertexArray::isVertexBuffer() const

    Returns true if this vertex array corresponds to the contents of a
    vertex buffer.  The constData() and data() functions will return
    zero, which is the offset of the first field in the vertex buffer.
*/

/*!
  Returns true if this QGLVertexArray is equal to the \a other, and false otherwise.
  Equal here means that either the \a other and this are the same array,
  or they have the same elements, and the same field definitions.
  This method is potentially expensive: in the worst case it will iterate over all
  elements in both vertex arrays.
 */
bool QGLVertexArray::operator==(const QGLVertexArray &other) const
{
    if (this == &other)
        return true;
    if (m_fields != other.m_fields)
        return false;
    return m_data == other.m_data;
}

/*!
  \fn bool QGLVertexArray::operator!=(const QGLVertexArray &other) const
  Returns true if this QGLVertexArray is not equal to the \a other, and false otherwise.
  This method is simply the inverse of the equality operator.
  \sa operator==()
 */

/*!
    Binds this array to a vertex buffer in the GL server for the
    current GL context.

    If the array has not been uploaded to the GL server, or it has
    been modified since the last upload, the vertex buffer will be
    re-uploaded.  The first element in the array is uploaded as
    offset 0 in the vertex buffer.

    Returns true if the vertex buffer was bound, or false otherwise.

    \sa release(), upload()
*/
bool QGLVertexArray::bind() const
{
    if (!upload())
        return false;
    return m_buffer->bind();
}

/*!
    Releases the vertex buffer for this array from the current GL context.

    This function must be called with the same QGLContext current
    that was current when bind() was called.

    \sa bind()
*/
void QGLVertexArray::release() const
{
    if (m_buffer)
        m_buffer->release();
}

/*!
    Uploads this array to the GL server as a vertex buffer if it
    has not been uploaded before or it has been modified since the
    last upload.  Does nothing if the array is already uploaded.

    Returns true if the array was uploaded; false if uploading
    was not possible (usually because vertex buffers are not
    supported by the GL server).

    \sa bind(), isUploaded()
*/
bool QGLVertexArray::upload() const
{
    if (!m_buffer) {
        m_buffer = new QGLReferencedBuffer(QGLReferencedBuffer::VertexBuffer);
        if (!m_buffer->create()) {
            delete m_buffer;
            m_buffer = 0;
            return false;
        }
        if (m_buffer->bind()) {
            m_buffer->write(constData(), componentCount() * sizeof(float));
            m_buffer->release();
        } else {
            delete m_buffer;
            m_buffer = 0;
            return false;
        }
    }
    return true;
}

/*!
    \fn bool QGLVertexArray::isUploaded() const

    Returns true if this vertex array has been uploaded into a vertex
    buffer in the GL server by upload() or bind(); false otherwise.

    \sa upload(), bind()
*/

#ifndef QT_NO_DEBUG_STREAM

static const char *vertAttrNames[] = {
    "Pos", "Norm", "Col", "Tex0",
    "Tex1", "Tex2", "Tex3", "Tex4",
    "Tex5", "Tex6", "Tex7", "Cust0",
    "Cust1", "Cust2", "Cust3", "Cust4",
    "Cust5", "Cust6", "Cust7"
};

QDebug operator<<(QDebug dbg, const QGLVertexArray &vertices)
{
    dbg.nospace() << "QGLVertexArray(\n";
    QGLVertexDescription flds = vertices.fields();
    dbg << '\t' << flds;
    for (int i = 0; i < vertices.vertexCount(); ++i)
    {
        dbg << '\t' << i << ':' << '\n';
        for (int j = 0; j < flds.fieldCount(); ++j)
        {
            dbg << "\t\t" << vertAttrNames[flds.fieldAttribute(j)] << ':';
            if (flds.fieldAttribute(j) >= QGL::TextureCoord0 &&
                flds.fieldAttribute(j) <= QGL::TextureCoord7)
            {
                QVector2D v = vertices.vector2DAt(i, j);
                dbg << '\t' << '(' << v.x() << ", " << v.y() << ')' << '\n';
            }
            else
            {
                QVector3D v = vertices.vector3DAt(i, j);
                dbg << '\t' << '(' << v.x() << ", " << v.y() << ", " << v.z() << ')' << '\n';
            }
        }
    }
    dbg << ")\n";
    return dbg.space();
}

#endif

QT_END_NAMESPACE
