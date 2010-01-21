/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QtCore/qdebug.h>

/*!
    \class QGeometryData
    \brief The QGeometryData class encapsulates QDataArray instances.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::geometry

    The QGeometryData class encloses a number of QDataArray template instances
    that models most typical vertex data needs.  The class provides a
    QDataArray based store for all of the data types in the
    QGL::VertexAttribute enumeration.

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

     \sa QGLPrimitive, QDataArray
*/

class QGeometryDataPrivate
{
public:
    QGeometryDataPrivate();
    ~QGeometryDataPrivate();

    QBasicAtomicInt ref;

    QVector3DArray vertices;
    QVector3DArray normals;
    QDataArray<QColor4b> colors;
    QList<QCustomDataArray> attributes;
    QList<QVector2DArray> textures;

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
    qMemSet(key, -1, ATTR_CNT);
    qMemSet(size, 0, ATTR_CNT);
}

QGeometryDataPrivate::~QGeometryDataPrivate()
{
}

/*!
    Construct an empty QGeometryData
*/
QGeometryData::QGeometryData()
    : d(0)
{
}

/*!
    Construct QGeometryData as a copy of other
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
    Assigns this QGeometryData to be a copy of other.
*/
QGeometryData &QGeometryData::operator=(const QGeometryData &other)
{
    if (d != other.d)
    {
        if (d && !d->ref.deref())
            delete d;
        d = other.d;
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
    Returns a QLogicalVertex that references the \a i'th logical vertex
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
                    Q_ASSERT(d->textures.at(field).count() == d->count);
                }
                else
                {
                    Q_ASSERT(d->attributes.at(field).count() == d->count);
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
                        array.append(d->textures.at(field).at(index));
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
                        QDataArray<QVector3D> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->vertices.at(i));
                            tmp.append(other.d->vertices.at(i));
                        }
                        res.d->vertices = tmp;
                    }
                    else if (attr == QGL::Normal)
                    {
                        QDataArray<QVector3D> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->normals.at(i));
                            tmp.append(other.d->normals.at(i));
                        }
                        res.d->normals = tmp;
                    }
                    else  // colors
                    {
                        QDataArray<QColor4b> tmp;
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
                    QDataArray<QVector2D> tmp;
                    const QDataArray<QVector2D> txa = d->textures.at(d->key[attr]);
                    const QDataArray<QVector2D> txb = other.d->textures.at(other.d->key[attr]);
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
    this geometry and \a other.  The resulting geometry contains N vertices
    where \c{N == qMin(count(), other.count())}, and has only the fields
    that are in both geometries.
*/
void QGeometryData::zipWith(const QGeometryData &other)
{
    if (d && other.d)
    {
        detach();
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
                        QDataArray<QVector3D> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->vertices.at(i));
                            tmp.append(other.d->vertices.at(i));
                        }
                        d->vertices = tmp;
                    }
                    else if (attr == QGL::Normal)
                    {
                        QDataArray<QVector3D> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->normals.at(i));
                            tmp.append(other.d->normals.at(i));
                        }
                        d->normals = tmp;
                    }
                    else  // colors
                    {
                        QDataArray<QColor4b> tmp;
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
                    QDataArray<QVector2D> tmp;
                    const QDataArray<QVector2D> txa = d->textures.at(d->key[attr]);
                    const QDataArray<QVector2D> txb = other.d->textures.at(other.d->key[attr]);
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
        d->count = cnt;
    }
}

/*!
    Clear all data structures.
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

void QGeometryData::clear(QGL::VertexAttribute field)
{
    if (d && (field & d->fields))
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
    Append the point \a v to this geometry data as a position field.
*/
void QGeometryData::appendVertex(const QVector3D &v)
{
    detach();
    enableField(QGL::Position);
    d->vertices.append(v);
    d->count = qMax(d->count, d->vertices.count());
}

/*!
    Append the float \a a to this geometry data, as an attribute \a field.
*/
void QGeometryData::appendAttribute(float a, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->attributes[d->key[field]].append(a);
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
    Append the 3D point \a a to this geometry data, as an attribute \a field.
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
    Append the vector \a n to this geometry data, as lighting normal.
*/
void QGeometryData::appendNormal(const QVector3D &n)
{
    detach();
    enableField(QGL::Normal);
    d->normals.append(n);
    d->count = qMax(d->count, d->normals.count());
}

/*!
    Append the point \a t to this geometry data, as an texture \a field.
*/
void QGeometryData::appendTexCoord(const QVector2D &t, QGL::VertexAttribute field)
{
    detach();
    enableField(field);
    d->textures[d->key[field]].append(t);
    d->count = qMax(d->count, d->textures[d->key[field]].count());
}

/*!
    Append the color \a c to this geometry data, as an color field.
*/
void QGeometryData::appendColor(const QColor4b &c)
{
    detach();
    enableField(QGL::Color);
    d->colors.append(c);
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
    }
}

/*!
    Append the colors in \a ary to this geometry data, as color fields.
*/
void QGeometryData::appendColorArray(const QDataArray<QColor4b> &ary)
{
    if (ary.count())
    {
        detach();
        enableField(QGL::Position);
        d->colors.append(ary);
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
    return QDataArray<QVector3D>();
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
    Returns a copy of the lighting normal data.
*/
QVector3DArray QGeometryData::normals() const
{
    if (d)
        return d->normals;
    return QDataArray<QVector3D>();
}

/*!
    Returns a modifiable reference to the color data at index \a i.
*/
QColor4b &QGeometryData::colorRef(int i)
{
    detach();
    return d->colors[i];
}

/*!
    Returns a copy of the color data.
*/
QDataArray<QColor4b> QGeometryData::colors() const
{
    if (d)
        return d->colors;
    return QDataArray<QColor4b>();
}

/*!
    Returns a modifiable reference to the texture coordinate data at index \a i.
*/
QVector2D &QGeometryData::texCoordRef(int i, QGL::VertexAttribute field)
{
    detach();
    return d->textures[d->key[field]][i];
}

/*!
    Returns a copy of the texture coordinate data.
*/
QVector2DArray QGeometryData::texCoords(QGL::VertexAttribute field) const
{
    return hasField(field) ? d->textures.at(d->key[field]) : QVector2DArray();
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
    if (d->key[field] != -1)
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
        d->textures.append(QDataArray<QVector2D>());
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
        d->attributes.append(QDataArray<QVector3D>());
        d->key[field] = d->attributes.count() - 1;
        d->size[field] = 3;
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
    the max() of QDataArray::count() over all of the enabled data types.
*/
int QGeometryData::count() const
{
    if (d)
        return d->count;
    return 0;
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
