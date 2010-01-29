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

#include "qglvertexdescription.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLVertexDescription
    \brief The QGLVertexDescription class describes the layout of a vertex within a QGLVertexArray.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers

    Vertices can have multiple types of information associated with them:
    positions, normals, colors, texture co-ordinates, etc.
    QGLVertexDescription supports this through the use of fields that
    define the layout of the data in a corresponding QGLVertexArray.

    Each field has a fieldAttribute() type, a fieldSize() in components
    (1, 2, 3, or 4), and a fieldOffset().  The sum of all field sizes is
    the vertex array's stride().

    The following example creates a vertex description with two fields:
    a 3D position and a 2D texture co-ordinate:

    \code
    QGLVertexDescription desc;
    desc.addField(QGL::Position, 3);
    desc.addField(QGL::TextureCoord0, 2);
    \endcode

    Once the layout of the vertex has been specified, field values
    can be added to a QGLVertexArray using QGLVertexArray::append():

    \code
    QGLVertexArray array(desc);
    array.append(2.0f, 0.0f, 0.0f);
    array.append(1.0f, 0.0f);
    array.append(2.0f, 3.0f, 0.0f);
    array.append(1.0f, 1.0f);
    array.append(0.0f, 3.0f, 0.0f);
    array.append(0.0f, 1.0f);
    \endcode

    \sa QGLVertexArray
*/

/*!
    \fn QGLVertexDescription::QGLVertexDescription()

    Constructs an empty vertex description.  Use addField() to
    add fields to the description.

    \sa addField()
*/

/*!
    \fn QGLVertexDescription::QGLVertexDescription(const QGLVertexDescription& other)

    Constructs a copy of \a other.
*/

/*!
    \fn int QGLVertexDescription::fieldCount() const

    Returns the number of fields in this vertex description.

    \sa stride(), addField()
*/

/*!
    \fn int QGLVertexDescription::stride() const

    Returns the stride of this vertex description in GLfloat units.

    \sa fieldCount(), addField()
*/

/*!
    Adds a field to this vertex description with the details \a attr
    and \a size.  The offset of the field is determined from stride().
    The \a size must be between 1 and 4, inclusive.

    \sa stride(), fieldCount(), addFields(), indexOf()
*/
void QGLVertexDescription::addField(QGL::VertexAttribute attr, int size)
{
    Q_ASSERT(size >= 1 && size <= 4);
    Field field;
    field.attr = attr;
    field.size = size;
    field.offset = m_stride;
    field.reserved = 0;
    m_fields.append(field);
    m_stride += size;
}

/*!
    Adds all of the fields in \a other to this vertex description.

    \sa addField()
*/
void QGLVertexDescription::addFields(const QGLVertexDescription& other)
{
    for (int posn = 0; posn < other.m_fields.count(); ++posn) {
        Field field = other.m_fields[posn];
        field.offset += m_stride;
        m_fields.append(field);
    }
    m_stride += other.m_stride;
}

/*!
    Returns the index of the first field in this vertex description
    that is associated with \a attr; -1 if \a attr is not present.

    \sa addField()
*/
int QGLVertexDescription::indexOf(QGL::VertexAttribute attr) const
{
    int index, posn;
    for (index = 0, posn = 0; posn < m_fields.count(); ++index, ++posn) {
        if (m_fields[posn].attr == attr)
            return index;
    }
    return -1;
}

/*!
    \fn QGL::VertexAttribute QGLVertexDescription::fieldAttribute(int index) const

    Returns the attribute name associated with the field at \a index
    in this vertex description; or QGL::Position if \a index is
    out of range.

    \sa fieldSize(), fieldOffset()
*/

/*!
    \fn int QGLVertexDescription::fieldSize(int index) const

    Returns the number of components (between 1 and 4) in the field at
    \a index in this vertex description; or 0 if \a index is out of range.

    \sa fieldAttribute(), fieldOffset()
*/

/*!
    \fn int QGLVertexDescription::fieldOffset(int index) const

    Returns the offset of the field at \a index in this vertex description
    in GLfloat units; or 0 if \a index is out of range.

    \sa fieldAttribute(), fieldSize()
*/

/*!
    \fn bool QGLVertexDescription::operator==(const QGLVertexDescription& other) const

    Returns true if \a other is the same as this vertex description;
    false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool QGLVertexDescription::operator!=(const QGLVertexDescription& other) const

    Returns true if \a other is not the same as this vertex description;
    false otherwise.

    \sa operator==()
*/
#ifndef QT_NO_DEBUG_STREAM

static const char *vertAttrNames[] = {
    "Position", "Normal", "Color", "TextureCoord0",
    "TextureCoord1", "TextureCoord2", "TextureCoord3", "TextureCoord4",
    "TextureCoord5", "TextureCoord6", "TextureCoord7", "CustomVertex0",
    "CustomVertex1", "CustomVertex2", "CustomVertex3", "CustomVertex4",
    "CustomVertex5", "CustomVertex6", "CustomVertex7"
};

QDebug operator<<(QDebug dbg, const QGLVertexDescription &description)
{
    dbg.nospace() << "QGLVertexDescription(";
    int i = 0;
    for (i = 0; i < description.fieldCount() - 1; ++i)
        dbg << i << ":" << vertAttrNames[description.fieldAttribute(i)] << ", ";
    dbg << i << ":" << vertAttrNames[description.fieldAttribute(i)] << ")\n";
    return dbg.space();
}

#endif

QT_END_NAMESPACE
