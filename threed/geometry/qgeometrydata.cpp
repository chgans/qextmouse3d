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

#include "qgeometrydata.h"
#include "qlogicalvertex.h"
#include "qglindexarray.h"
#include <QtCore/qdebug.h>

/*!
    \class QGeometryData
    \brief The QGeometryData class encapsulates sets of geometry data.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::geometry

    The QGeometryData class encloses a number of data arrays
    that model most typical vertex data needs.  The class provides a
    store for all of the data types in the QGL::VertexAttribute enumeration.

    \table
        \header
            \o QGL::VertexAttribute
            \o QGeometryData functions
        \row
            \o QGL::Position
            \o appendVertex(), vertexRef(), vertices()
        \row
            \o QGL::Normal
            \o appendNormal(), normalRef(), normals()
        \row
            \o QGL::Color
            \o appendColor(), colorRef(), colors()
        \row
            \o QGL::TextureCoord0 - QGL::TextureCoord7
            \o appendTexCoord(), texCoordRef(), texCoords()
        \row
            \o QGL::CustomVertex0 - QGL::CustomVertex7
            \o appendAttribute(), vector3DAttributeRef(), attributes()
     \endtable

     Additionally the class provides the following features:
     \list
        \o appendVertex() for adding a QLogicalVertex()
        \o vertexAt() for return the data at an index as a QLogicalVertex()
        \o hasField() to find if a particular data type is present
        \o normalizeNormals() to reduce all normal vectors to unit length
        \o boundingBox() to find the bounds of the geometry
        \o setCommonNormal() / commonNormal() for a normal that applies to all vertices
     \endlist

     It is up to the user of a QGeometryData instance to ensure that the
     data has an equal number of items in each field.  For example, if five
     vertices are added and only two normals are added, the logical vertex at
     position 3 will be corrupt, since it does not have a normal.

     While data is being accumulated the counts of different fields will vary,
     since it may be convenient to add several vertices, then several normals,
     colors or attributes at a time.  However when a logical vertex is
     constructed or when the data is sent to the GPU, counts of all fields
     must be equal.

     QGeometryData uses implicit sharing with lazy creation of internal
     data so that code like:
     \code
     QGeometryData myData;
     if (processed)
        myData = processedData();
     \endcode
     is very inexpensive, since the first declaration and initialization
     does not cause internal data to be created (only to be overwritten by the
     assignment operation).

     \sa QGLPrimitive
*/

class QGeometryDataPrivate
{
public:
    QGeometryDataPrivate();
    ~QGeometryDataPrivate();

    QBasicAtomicInt ref;

    QVector3DArray vertices;
    QVector3DArray normals;
    QArray<QColor4B> colors;
    QList<QCustomDataArray> attributes;
    QList<QVector2DArray> textures;
    QGLIndexArray indices;

    static const int ATTR_CNT = 32;
    QVector3D commonNormal;
    quint32 fields;
    qint8 key[ATTR_CNT];
    quint8 size[ATTR_CNT];
    int count;
};

QGeometryDataPrivate::QGeometryDataPrivate()
    : fields(0)
    , count(0)
{
    ref = 0;
    qMemSet(key, -1, ATTR_CNT);
    qMemSet(size, 0, ATTR_CNT);
}

QGeometryDataPrivate::~QGeometryDataPrivate()
{
}

/*!
    \fn quint32 QGL::fieldMask(QGL::VertexAttribute attribute)
    \relates QGeometryData
    Returns an unsigned integer mask from the \a attribute.

    \sa QGeometryData::fields()
*/

/*!
    Construct an empty QGeometryData
*/
QGeometryData::QGeometryData()
    : d(0)
{
}

/*!
    Construct QGeometryData as a copy of \a other
*/
QGeometryData::QGeometryData(const QGeometryData &other)
    : d(other.d)
{
    if (d)
        d->ref.ref();
}

/*!
    Construct an empty QGeometryData with the \a fields enabled.
*/
QGeometryData::QGeometryData(quint32 fields)
    : d(new QGeometryDataPrivate)
{
    d->ref.ref();
    const quint32 mask = 0x01;
    for (int field = 0; fields; ++field, fields >>= 1)
    {
        if (!(mask & fields)) continue;
        QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
        enableField(attr);
    }
}

/*!
    Destroys this QGeometryData recovering any resources.
*/
QGeometryData::~QGeometryData()
{
    if (d && !d->ref.deref())
        delete d;
}

/*!
    Assigns this QGeometryData to be a copy of \a other.
*/
QGeometryData &QGeometryData::operator=(const QGeometryData &other)
{
    if (d != other.d)
    {
        if (d && !d->ref.deref())
            delete d;
        d = other.d;
        if (d)
            d->ref.ref();
    }
    return *this;
}

/*!
    Appends the geometry in \a data to this.  If this is empty, then all
    fields of \a data are appended; otherwise (when this has existing fields)
    only those fields that exist in both are appended.
*/
void QGeometryData::appendGeometry(const QGeometryData &data)
{
    if (data.d && data.count())
    {
        detach();
        int cnt = data.d->count;
        const quint32 mask = 0x01;
        // only append fields that are in both, unless we have NO fields, then
        // append everything that is in data
        quint32 fields = d->fields ? d->fields & data.d->fields : data.d->fields;
        for (int field = 0; fields; ++field, fields >>= 1)
        {
            if (mask & fields)
            {
                QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
                enableField(attr);  // might not be enabled if we had NO fields
                if (attr < QGL::TextureCoord0)
                {
                    if (attr == QGL::Position)
                        d->vertices.append(data.d->vertices);
                    else if (attr == QGL::Normal)
                        d->normals.append(data.d->normals);
                    else  // colors
                        d->colors.append(data.d->colors);
                }
                else if (attr < QGL::CustomVertex0)
                {
                    d->textures[d->key[attr]].append(data.texCoords(attr));
                }
                else
                {
                    d->attributes[d->key[attr]].append(data.attributes(attr));
                }
            }
        }
        d->count += cnt;
    }
}

/*!
    Appends all the data fields in QLogicalVertex \a v to this
    QGeometryData object.
*/
int QGeometryData::appendVertex(const QLogicalVertex &v)
{
    detach();
    quint32 fields = v.fields();
    const quint32 mask = 0x01;
    for (int field = 0; fields; ++field, fields >>= 1)
    {
        if (mask & fields)
        {
            QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
            if (attr < QGL::TextureCoord0)
            {
                if (attr == QGL::Position)
                    appendVertex(v.vertex());
                else if (attr == QGL::Normal)
                    appendNormal(v.normal());
                else
                    appendColor(v.color());
            }
            else if (attr < QGL::CustomVertex0)
            {
                appendTexCoord(v.texCoord(attr), attr);
            }
            else
            {
                appendAttribute(v.attribute(attr), attr);
            }
        }
    }
    return d->count - 1;
}

/*!
    Returns a QLogicalVertex that references the \a{i}'th logical vertex
    of this geometry.
*/
QLogicalVertex QGeometryData::vertexAt(int i) const
{
    return QLogicalVertex(*this, i);
}

/*!
    Returns a QGLVertexArray that contains all the data of this geometry.

    NOTE: this is a temporary facility - when QGeometryData can function as
        QGLVertexArray, uploading to the GPU and setting on QPainter then
        this function can be removed.  The bounds checking asserts should be
        moved at that point into what ever code does the upload/setting.
*/
QGLVertexArray QGeometryData::toVertexArray() const
{
    QGLVertexArray array;
    if (d)
    {
        const quint32 mask = 0x01;
        quint32 fields = d->fields;
        for (int field = 0; fields; ++field, fields >>= 1)
        {
            if (mask & fields)
            {
                QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
                array.addField(attr, d->size[field]);
#ifndef QT_NO_DEBUG
                if (attr < QGL::TextureCoord0)
                {
                    if (attr == QGL::Position)
                        Q_ASSERT(d->vertices.count() == d->count);
                    else if (attr == QGL::Normal)
                        Q_ASSERT(d->normals.count() == d->count);
                    else
                        Q_ASSERT(d->colors.count() == d->count);
                }
                else if (attr < QGL::CustomVertex0)
                {
                    Q_ASSERT(d->textures.at(d->key[field]).count() == d->count);
                }
                else
                {
                    Q_ASSERT(d->attributes.at(d->key[field]).count() == d->count);
                }
#endif
            }
        }
        for (int index = 0; index < d->count; ++index)
        {
            fields = d->fields;
            for (int field = 0; fields; ++field, fields >>= 1)
            {
                if (mask & fields)
                {
                    QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
                    if (attr < QGL::TextureCoord0)
                    {
                        if (attr == QGL::Position)
                            array.append(d->vertices.at(index));
                        else if (attr == QGL::Normal)
                            array.append(d->normals.at(index));
                        else
                            array.append(d->colors.at(index));
                    }
                    else if (attr < QGL::CustomVertex0)
                    {
                        array.append(d->textures.at(d->key[field]).at(index));
                    }
                    else
                    {
                        // TODO
                        //array.append(d->attributes.at(field).at(index));
                    }
                }
            }
        }
    }
    return array;
}

/*!
    Normalize all the normal vectors in this geometry to unit length.
*/
void QGeometryData::normalizeNormals()
{
    if (d)  // nothng to do if its null
    {
        detach();
        if (hasField(QGL::Normal))
        {
            for (int i = 0; i < d->normals.count(); ++i)
                d->normals[i].normalize();
        }
    }
}

/*!
    Calculate and return a bounding box for the vertex data in this geometry.
*/
QBox3D QGeometryData::boundingBox() const
{
    QBox3D box;
    if (d)
    {
        for (int i = 0; i < d->count; ++i)
            box.expand(d->vertices.at(i));
    }
    return box;
}

/*!
    Sets \a normal to be a common normal.  This normal vector is used for
    all vertices in the geometry.
*/
void QGeometryData::setCommonNormal(const QVector3D &normal)
{
    if (!d && normal.isNull())
        return
    detach();
    d->commonNormal = normal;
}

/*!
    Returns the common normal set on this geometry, or a null vector if
    none was set.  This vector is not affected by normalizeNormals().
    Also fields() and enableFields() only affect per-vertex normal values
    stored in the geometry data - these values are unrelated to commonNormal().

    A QLogicalVertex extracted from this geometry will return the commonNormal()
    value, if it is non-null.  In this way, the common normal overrides any
    geometry data set.
*/
QVector3D QGeometryData::commonNormal() const
{
    if (d)
        return d->commonNormal;
    return QVector3D();
}

/*!
    Returns a QGeometryData instance containing alternating vertices from
    this geometry and \a other.  The resulting geometry contains N vertices
    where \c{N == qMin(count(), other.count())}, and has only the fields
    that are in both geometries.
*/
QGeometryData QGeometryData::zippedWith(const QGeometryData &other) const
{
    QGeometryData res;
    if (d && other.d)
    {
        int cnt = qMax(d->count, other.d->count);
        const quint32 mask = 0x01;
        quint32 fields = d->fields & other.d->fields;
        for (int field = 0; fields; ++field, fields >>= 1)
        {
            if (mask & fields)
            {
                QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
                if (attr < QGL::TextureCoord0)
                {
                    if (attr == QGL::Position)
                    {
                        QArray<QVector3D> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->vertices.at(i));
                            tmp.append(other.d->vertices.at(i));
                        }
                        res.d->vertices = tmp;
                    }
                    else if (attr == QGL::Normal)
                    {
                        QArray<QVector3D> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->normals.at(i));
                            tmp.append(other.d->normals.at(i));
                        }
                        res.d->normals = tmp;
                    }
                    else  // colors
                    {
                        QArray<QColor4B> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->colors.at(i));
                            tmp.append(other.d->colors.at(i));
                        }
                        res.d->colors = tmp;
                    }
                }
                else if (attr < QGL::CustomVertex0)
                {
                    QArray<QVector2D> tmp;
                    const QArray<QVector2D> txa = d->textures.at(d->key[attr]);
                    const QArray<QVector2D> txb = other.d->textures.at(other.d->key[attr]);
                    for (int i = 0; i < cnt; ++i)
                    {
                        tmp.append(txa.at(i));
                        tmp.append(txb.at(i));
                    }
                    res.d->textures[d->key[attr]] = tmp;
                }
                else
                {
                    QCustomDataArray tmp;
                    const QCustomDataArray ata = d->attributes.at(d->key[attr]);
                    const QCustomDataArray atb = other.d->attributes.at(other.d->key[attr]);
                    for (int i = 0; i < cnt; ++i)
                    {
                        tmp.append(ata.at(i));
                        tmp.append(atb.at(i));
                    }
                    res.d->attributes[d->key[attr]] = tmp;
                }
            }
        }
        res.d->count = cnt;
    }
    return res;
}

/*!
    Sets this QGeometryData to contain alternating vertices from
    this geometry and \a other.  The resulting geometry contains \c{N * 2} vertices
    where \c{N == qMin(count(), other.count())}, and has only the fields
    that are in both geometries.
*/
void QGeometryData::zipWith(const QGeometryData &other)
{
    if (d && other.d)
    {
        detach();
        int cnt = qMin(d->count, other.d->count);
        const quint32 mask = 0x01;
        quint32 fields = d->fields & other.d->fields;
        for (int field = 0; fields; ++field, fields >>= 1)
        {
            if (mask & fields)
            {
                QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
                if (attr < QGL::TextureCoord0)
                {
                    if (attr == QGL::Position)
                    {
                        QArray<QVector3D> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->vertices.at(i));
                            tmp.append(other.d->vertices.at(i));
                        }
                        d->vertices = tmp;
                    }
                    else if (attr == QGL::Normal)
                    {
                        QArray<QVector3D> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->normals.at(i));
                            tmp.append(other.d->normals.at(i));
                        }
                        d->normals = tmp;
                    }
                    else  // colors
                    {
                        QArray<QColor4B> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->colors.at(i));
                            tmp.append(other.d->colors.at(i));
                        }
                        d->colors = tmp;
                    }
                }
                else if (attr < QGL::CustomVertex0)
                {
                    QArray<QVector2D> tmp;
                    const QArray<QVector2D> txa = d->textures.at(d->key[attr]);
                    const QArray<QVector2D> txb = other.d->textures.at(other.d->key[attr]);
                    for (int i = 0; i < cnt; ++i)
                    {
                        tmp.append(txa.at(i));
                        tmp.append(txb.at(i));
                    }
                    d->textures[d->key[attr]] = tmp;
                }
                else
                {
                    QCustomDataArray tmp;
                    const QCustomDataArray ata = d->attributes.at(d->key[attr]);
                    const QCustomDataArray atb = other.d->attributes.at(other.d->key[attr]);
                    for (int i = 0; i < cnt; ++i)
                    {
                        tmp.append(ata.at(i));
                        tmp.append(atb.at(i));
                    }
                    d->attributes[d->key[attr]] = tmp;
                }
            }
        }
        d->count = cnt * 2;
    }
}

/*!
    Clear all data structures.  The actual fields are retained, but they
    have no contents.
    \code
    QGeometryData data;
    data.appendVertex(a);
    data.appendTexCoord(t);

    // prints "1"
    qDebug() << data.count();

    // x == a
    QVector3D x = data.vertex(0);

    quint32 flds = QGL::fieldMask(QGL::Position)
                    | QGL::fieldMask(QGL::TextureCoord0);
    qDebug() << (flds == data.fields());  // prints "true"

    data.clear();
    qDebug() << data.count();             // prints "0"
    QVector3D x = data.vertex(0);         // asserts - no data in vertices
    qDebug() << (flds == data.fields());  // still prints "true"
    \endcode
*/
void QGeometryData::clear()
{
    if (d)
    {
        detach();
        const quint32 mask = 0x01;
        quint32 fields = d->fields;
        for (int field = 0; fields; ++field, fields >>= 1)
        {
            if (mask & fields)
            {
                QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
                if (attr < QGL::TextureCoord0)
                {
                    if (attr == QGL::Position)
                        d->vertices.clear();
                    else if (attr == QGL::Normal)
                        d->normals.clear();
                    else
                        d->colors.clear();
                }
                else if (attr < QGL::CustomVertex0)
                {
                    d->textures[d->key[field]].clear();
                }
                else
                {
                    d->attributes[d->key[field]].clear();
                }
            }
        }
        d->count = 0;
    }
}

/*!
    Clears the data from \a field.
    \overload
*/
void QGeometryData::clear(QGL::VertexAttribute field)
{
    if (d && (QGL::fieldMask(field) & d->fields))
    {
        detach();
        QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
        if (attr < QGL::TextureCoord0)
        {
            if (attr == QGL::Position)
                d->vertices.clear();
            else if (attr == QGL::Normal)
                d->normals.clear();
            else
                d->colors.clear();
        }
        else if (attr < QGL::CustomVertex0)
        {
            d->textures[d->key[field]].clear();
        }
        else
        {
            d->attributes[d->key[field]].clear();
        }
        d->key[field] = -1;
    }
}

/*!
    Appends \a index to the vertex index array.

    \sa appendIndices(), indices()
*/
void QGeometryData::appendIndex(int index)
{
    detach();
    d->indices.append(index);
}

/*!
    Appends \a index1, \a index2, and \a index3 to the vertex
    index array.

    \sa appendIndex(), indices()
*/
void QGeometryData::appendIndices(int index1, int index2, int index3)
{
    detach();
    d->indices.append(index1, index2, index3);
}

/*!
    Returns the index array that was created by appendIndex().

    \sa appendIndex(), appendIndices()
*/
QGLIndexArray QGeometryData::indices() const
{
    if (d)
        return d->indices;
    else
        return QGLIndexArray();
}

/*!
    Append the point \a v0 to this geometry data as a position field.
*/
void QGeometryData::appendVertex(const QVector3D &v0)
{
    detach();
    enableField(QGL::Position);
    d->vertices.append(v0);
    d->count = qMax(d->count, d->vertices.count());
}

/*!
    Append the points \a v0 and \a v1 to this geometry data as position fields.
*/
void QGeometryData::appendVertex(const QVector3D &v0, const QVector3D &v1)
{
    detach();
    enableField(QGL::Position);
    d->vertices.append(v0, v1);
    d->count = qMax(d->count, d->vertices.count());
}

/*!
    Append the points \a v0, \a v1 and \a v2 to this geometry data as position fields.
*/
void QGeometryData::appendVertex(const QVector3D &v0, const QVector3D &v1, const QVector3D &v2)
{
    detach();
    enableField(QGL::Position);
    d->vertices.append(v0, v1, v2);
    d->count = qMax(d->count, d->vertices.count());
}

/*!
    Append the points \a v0, \a v1, \a v2 and \a v3 to this geometry data as position fields.
*/
void QGeometryData::appendVertex(const QVector3D &v0, const QVector3D &v1, const QVector3D &v2, const QVector3D &v3)
{
    detach();
    enableField(QGL::Position);
    d->vertices.append(v0, v1, v2, v3);
    d->count = qMax(d->count, d->vertices.count());
}

/*!
    Append the float \a a0 to this geometry data, as an attribute \a field.
*/
void QGeometryData::appendAttribute(float a0, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->attributes[d->key[field]].append(a0);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the float \a a0 and \a a1 to this geometry data, as an attribute \a field.
*/
void QGeometryData::appendAttribute(float a0, float a1, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->attributes[d->key[field]].append(a0, a1);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the floats \a a0, \a a1 and \a a2 to this geometry data, as attribute \a field.
*/
void QGeometryData::appendAttribute(float a0, float a1, float a2, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->attributes[d->key[field]].append(a0, a1, a2);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the floats \a a0, \a a1, \a a2 and \a a3 to this geometry data, as attribute \a field.
*/
void QGeometryData::appendAttribute(float a0, float a1, float a2, float a3, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->attributes[d->key[field]].append(a0, a1, a2, a3);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the 2D point \a a to this geometry data, as an attribute \a field.
*/
void QGeometryData::appendAttribute(const QVector2D &a, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->attributes[d->key[field]].append(a);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the 3D point \a v to this geometry data, as an attribute \a field.
*/
void QGeometryData::appendAttribute(const QVector3D &v, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->attributes[d->key[field]].append(v);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the variant value \a a to this geometry data, as an attribute \a field.
*/
void QGeometryData::appendAttribute(const QVariant &a, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->attributes[d->key[field]].append(a);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the vector \a n0 to this geometry data, as a lighting normal.
*/
void QGeometryData::appendNormal(const QVector3D &n0)
{
    detach();
    enableField(QGL::Normal);
    d->normals.append(n0);
    d->count = qMax(d->count, d->normals.count());
}

/*!
    Append the vectors \a n0 and \a n1 to this geometry data, as lighting normals.
*/
void QGeometryData::appendNormal(const QVector3D &n0, const QVector3D &n1)
{
    detach();
    enableField(QGL::Normal);
    d->normals.append(n0, n1);
    d->count = qMax(d->count, d->normals.count());
}

/*!
    Append the vectors \a n0, \a n1 and \a n2 to this geometry data, as lighting normals.
*/
void QGeometryData::appendNormal(const QVector3D &n0, const QVector3D &n1, const QVector3D &n2)
{
    detach();
    enableField(QGL::Normal);
    d->normals.append(n0, n1, n2);
    d->count = qMax(d->count, d->normals.count());
}

/*!
    Append the vectors \a n0, \a n1, \a n2 and \a n3 to this geometry data, as lighting normals.
*/
void QGeometryData::appendNormal(const QVector3D &n0, const QVector3D &n1, const QVector3D &n2, const QVector3D &n3)
{
    detach();
    enableField(QGL::Normal);
    d->normals.append(n0, n1, n2, n3);
    d->count = qMax(d->count, d->normals.count());
}

/*!
    Append the point \a t0 to this geometry data, as an texture \a field.
*/
void QGeometryData::appendTexCoord(const QVector2D &t0, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->textures[d->key[field]].append(t0);
    d->count = qMax(d->count, d->textures[d->key[field]].count());
}

/*!
    Append the points \a t0 and \a t1 to this geometry data, as texture \a{field}s.
*/
void QGeometryData::appendTexCoord(const QVector2D &t0, const QVector2D &t1, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->textures[d->key[field]].append(t0, t1);
    d->count = qMax(d->count, d->textures[d->key[field]].count());
}

/*!
    Append the points \a t0, \a t1 and \a t2 to this geometry data, as texture \a{field}s.
*/
void QGeometryData::appendTexCoord(const QVector2D &t0, const QVector2D &t1, const QVector2D &t2, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->textures[d->key[field]].append(t0, t1, t2);
    d->count = qMax(d->count, d->textures[d->key[field]].count());
}

/*!
    Append the points \a t0, \a t1, \a t2 and \a t3 to this geometry data, as texture \a{field}s.
*/
void QGeometryData::appendTexCoord(const QVector2D &t0, const QVector2D &t1, const QVector2D &t2, const QVector2D &t3, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->textures[d->key[field]].append(t0, t1, t2, t3);
    d->count = qMax(d->count, d->textures[d->key[field]].count());
}

/*!
    Append the color \a c0 to this geometry data, as an color field.
*/
void QGeometryData::appendColor(const QColor4B &c0)
{
    detach();
    enableField(QGL::Color);
    d->colors.append(c0);
    d->count = qMax(d->count, d->colors.count());
}

/*!
    Append the color \a c0 and \a c1 to this geometry data, as color fields.
*/
void QGeometryData::appendColor(const QColor4B &c0, const QColor4B &c1)
{
    detach();
    enableField(QGL::Color);
    d->colors.append(c0, c1);
    d->count = qMax(d->count, d->colors.count());
}

/*!
    Append the color \a c0, \a c1 and \a c2 to this geometry data, as color fields.
*/
void QGeometryData::appendColor(const QColor4B &c0, const QColor4B &c1, const QColor4B &c2)
{
    detach();
    enableField(QGL::Color);
    d->colors.append(c0, c1, c2);
    d->count = qMax(d->count, d->colors.count());
}

/*!
    Append the color \a c0, \a c1, \a c2  and \a c3 to this geometry data, as color fields.
*/
void QGeometryData::appendColor(const QColor4B &c0, const QColor4B &c1, const QColor4B &c2, const QColor4B &c3)
{
    detach();
    enableField(QGL::Color);
    d->colors.append(c0, c1, c2, c3);
    d->count = qMax(d->count, d->colors.count());
}

/*!
    Append the points in \a ary to this geometry data as position fields.
*/
void QGeometryData::appendVertexArray(const QVector3DArray &ary)
{
    if (ary.count())
    {
        detach();
        enableField(QGL::Position);
        d->vertices.append(ary);
        d->count = qMax(d->count, d->vertices.count());
    }
}

/*!
    Append the points in \a ary to this geometry data, as an attribute \a field entries.
*/
void QGeometryData::appendAttributeArray(const QCustomDataArray &ary, QGL::VertexAttribute field)
{
    if (ary.count())
    {
        detach();
        enableField(field);
        d->attributes[d->key[field]].append(ary);
        d->count = qMax(d->count, d->attributes[d->key[field]].count());
    }
}

/*!
    Append the vectors in \a ary to this geometry data, as lighting normals.
*/
void QGeometryData::appendNormalArray(const QVector3DArray &ary)
{
    if (ary.count())
    {
        detach();
        enableField(QGL::Normal);
        d->normals.append(ary);
        d->count = qMax(d->count, d->normals.count());
    }
}

/*!
    Append the 2D points in \a ary to this geometry data, as texture \a field entries.
*/
void QGeometryData::appendTexCoordArray(const QVector2DArray &ary, QGL::VertexAttribute field)
{
    if (ary.count())
    {
        detach();
        enableField(field);
        d->textures[d->key[field]].append(ary);
        d->count = qMax(d->count, d->textures[d->key[field]].count());
    }
}

/*!
    Append the colors in \a ary to this geometry data, as color fields.
*/
void QGeometryData::appendColorArray(const QArray<QColor4B> &ary)
{
    if (ary.count())
    {
        detach();
        enableField(QGL::Position);
        d->colors.append(ary);
        d->count = qMax(d->count, d->colors.count());
    }
}

/*!
    Returns a modifiable reference to the vertex data at index \a i.
*/
QVector3D &QGeometryData::vertexRef(int i)
{
    detach();
    return d->vertices[i];
}

/*!
    Returns a copy of the vertex position data.
*/
QVector3DArray QGeometryData::vertices() const
{
    if (d)
        return d->vertices;
    return QArray<QVector3D>();
}

/*!
    Returns a copy of the vertex position data at index \a i.
*/
QVector3D QGeometryData::vertex(int i) const
{
    Q_ASSERT(hasField(QGL::Position));
    return d->vertices.at(i);
}

/*!
    Returns a modifiable reference to the normal data at index \a i.
*/
QVector3D &QGeometryData::normalRef(int i)
{
    detach();
    return d->normals[i];
}

/*!
    Returns a copy of the normal data at index \a i.
*/
QVector3D QGeometryData::normal(int i) const
{
    Q_ASSERT(hasField(QGL::Normal));
    return d->normals.at(i);
}

/*!
    Returns a copy of the lighting normal data.
*/
QVector3DArray QGeometryData::normals() const
{
    if (d)
        return d->normals;
    return QArray<QVector3D>();
}

/*!
    Returns a modifiable reference to the color data at index \a i.
*/
QColor4B &QGeometryData::colorRef(int i)
{
    detach();
    return d->colors[i];
}

/*!
    Returns a modifiable reference to the color data at index \a i.
*/
QColor4B QGeometryData::color(int i) const
{
    Q_ASSERT(hasField(QGL::Color));
    return d->colors.at(i);
}

/*!
    Returns a copy of the color data.
*/
QArray<QColor4B> QGeometryData::colors() const
{
    if (d)
        return d->colors;
    return QArray<QColor4B>();
}

/*!
    Returns a modifiable reference to the \a field texture coordinate data at index \a i.
*/
QVector2D &QGeometryData::texCoordRef(int i, QGL::VertexAttribute field)
{
    detach();
    return d->textures[d->key[field]][i];
}

/*!
    Returns a copy of the \a field texture coordinate data.
*/
QVector2DArray QGeometryData::texCoords(QGL::VertexAttribute field) const
{
    return hasField(field) ? d->textures.at(d->key[field]) : QVector2DArray();
}

/*!
    Returns a copy of the texture coordinate data at index \a i for \a field.
*/
QVector2D QGeometryData::texCoord(int i, QGL::VertexAttribute field) const
{
    Q_ASSERT(hasField(field));
    return d->textures.at(d->key[field]).at(i);
}

/*!
    Returns a modifiable reference to the float \a field attribute data at index \a i.
*/
float &QGeometryData::floatAttributeRef(int i, QGL::VertexAttribute field)
{
    detach();
    QCustomDataArray &ary = d->attributes[d->key[field]];
    Q_ASSERT(ary.elementType() == QCustomDataArray::Float);
    return ary.m_array[i];
}

/*!
    Returns a modifiable reference to the 2D vector \a field attribute data at index \a i.
*/
QVector2D &QGeometryData::vector2DAttributeRef(int i, QGL::VertexAttribute field)
{
    detach();
    QCustomDataArray &ary = d->attributes[d->key[field]];
    Q_ASSERT(ary.elementType() == QCustomDataArray::Vector2D);
    float *data = ary.m_array.data();
    QVector2D *v = reinterpret_cast<QVector2D*>(data + i*2);
    return *v;
}

/*!
    Returns a modifiable reference to the 3D vector \a field attribute data at index \a i.
*/
QVector3D &QGeometryData::vector3DAttributeRef(int i, QGL::VertexAttribute field)
{
    detach();
    QCustomDataArray &ary = d->attributes[d->key[field]];
    Q_ASSERT(ary.elementType() == QCustomDataArray::Vector3D);
    float *data = ary.m_array.data();
    QVector3D *v = reinterpret_cast<QVector3D*>(data + i*2);
    return *v;
}

/*!
    Returns a copy of the \a field attribute data.
*/
QCustomDataArray QGeometryData::attributes(QGL::VertexAttribute field) const
{
    return hasField(field) ? d->attributes.at(d->key[field]) : QCustomDataArray();
}

/*!
    Returns a copy of the float \a field attribute data at index \a i.
*/
float QGeometryData::floatAttribute(int i, QGL::VertexAttribute field) const
{
    Q_ASSERT(hasField(field));
    return d->attributes.at(d->key[field]).floatAt(i);
}

/*!
    Returns a copy of the 2D vector \a field attribute data at index \a i.
*/
QVector2D QGeometryData::vector2DAttribute(int i, QGL::VertexAttribute field) const
{
    Q_ASSERT(hasField(field));
    return d->attributes.at(d->key[field]).vector2DAt(i);
}

/*!
    Returns a copy of the 3D vector \a field attribute data at index \a i.
*/
QVector3D QGeometryData::vector3DAttribute(int i, QGL::VertexAttribute field) const
{
    Q_ASSERT(hasField(field));
    return d->attributes.at(d->key[field]).vector3DAt(i);
}


/*!
    Returns true if this geometry has the field corresponding to \a attr.  Note
    that it is still possible for no data to have been added for that field.
*/
bool QGeometryData::hasField(QGL::VertexAttribute attr) const
{
    if (d)
        return d->key[attr] != -1;
    return false;
}

/*!
    Enables this geometry to contain data of type \a field.  Generally it is
    not necessary to call this function since it is called by all the append
    functions.
*/
void QGeometryData::enableField(QGL::VertexAttribute field)
{
    if (d && d->key[field] != -1)
        return;
    detach();
    Q_ASSERT(field < d->ATTR_CNT); // don't expand that enum too much
    d->fields |= (1 << field);
    switch (field)
    {
    case QGL::Position:
        d->key[QGL::Position] = 0;
        d->size[QGL::Position] = 3;
        break;
    case QGL::Normal:
        d->key[QGL::Normal] = 1;
        d->size[QGL::Normal] = 3;
        break;
    case QGL::Color:
        d->key[QGL::Color] = 2;
        d->size[QGL::Color] = 1;
        break;
    case QGL::TextureCoord0:
    case QGL::TextureCoord1:
    case QGL::TextureCoord2:
    case QGL::TextureCoord3:
    case QGL::TextureCoord4:
    case QGL::TextureCoord5:
    case QGL::TextureCoord6:
    case QGL::TextureCoord7:
        d->textures.append(QVector2DArray());
        d->key[field] = d->textures.count() - 1;
        d->size[field] = 2;
        break;
    case QGL::CustomVertex0:
    case QGL::CustomVertex1:
    case QGL::CustomVertex2:
    case QGL::CustomVertex3:
    case QGL::CustomVertex4:
    case QGL::CustomVertex5:
    case QGL::CustomVertex6:
    case QGL::CustomVertex7:
        d->key[field] = d->attributes.count() - 1;
        d->size[field] = d->attributes.at(d->key[field]).elementSize();
        break;
    }
}

/*!
    Return a bit-mask of the supported fields in this geometry.  The
    QGL::VertexAttribute enum can be recovered from this bit-mask by
    \code
    quint32 fields = fields();
    QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(fields);
    \endcode
*/
quint32 QGeometryData::fields() const
{
    if (d)
        return d->fields;
    return 0;
}

/*!
    Returns the count of logical vertices stored.  This is effectively
    the max() of QArray::count() over all of the enabled data types.
*/
int QGeometryData::count() const
{
    if (d)
        return d->count;
    return 0;
}

/*!
    Returns the count of data stored in \a field.  This will always be at
    most count(), but could be less.
*/
int QGeometryData::count(QGL::VertexAttribute field) const
{
    int result = 0;
    if (d && (QGL::fieldMask(field) & d->fields))
    {
        if (field < QGL::TextureCoord0)
        {
            if (field == QGL::Position)
                result = d->vertices.count();
            else if (field == QGL::Normal)
                result = d->normals.count();
            else
                result = d->colors.count();
        }
        else if (field < QGL::CustomVertex0)
        {
            result = d->textures[d->key[field]].count();
        }
        else
        {
            result = d->attributes[d->key[field]].count();
        }
    }
    return result;
}

/*!
    \internal
    You know what this is for.  No user serviceable parts below here.
*/
void QGeometryData::detach()
{
    if (!d) // lazy creation of data block
    {
        d = new QGeometryDataPrivate;
        d->ref.ref();
    }
    else
    {
        if (d->ref > 1)  // being shared, must detach
        {
            QGeometryDataPrivate *temp = new QGeometryDataPrivate;
            temp->vertices = d->vertices;
            temp->normals = d->normals;
            temp->colors = d->colors;
            temp->attributes = d->attributes;
            temp->indices = d->indices;
            temp->textures = d->textures;
            temp->fields = d->fields;
            qMemCopy(temp->key, d->key, d->ATTR_CNT);
            qMemCopy(temp->size, d->size, d->ATTR_CNT);
            temp->count = d->count;
            temp->ref.ref();
            d->ref.deref();
            d = temp;
        }
    }
}
