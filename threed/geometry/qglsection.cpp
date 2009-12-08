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

#include "qglsection.h"
#include "qgldisplaylist_p.h"
#include "qgeometrydata_p.h"

#include <QtGui/qvector3d.h>
#include <QtCore/qdebug.h>

/*!
    \class QGLSection
    \brief The QGLSection class clusters like geometry in a QGLDisplayList.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::geometry

    QGLSection instances partition a QGLDisplayList into related sections.
    All the vertices in a QGLSection are treated with the same
    \l{QGL::Smoothing}{smoothing}, and have the same
    \l{QGLSection::GLDataFlag}{data types}.

    Each QGLSection references a contiguous range of vertices in a
    QGLDisplayList.

    A QGLDisplayList instance has the \l{QGLDisplayList::newSection()}{newSection()}
    function which creates a new QGLSection to reference its data.  Use this
    to construct new QGLSection instances, or alternatively construct
    a new QGLSection() and pass a non-null QGLDisplayList pointer.

    Accessor functions are provided for getting copies of the data in
    the section:
    \list
        \o vertexArray()
        \o normalArray()
        \o texCoordArray()
        \o colorArray()
        \o indexArray()
    \endlist

    These functions all return QVector values. QVector instances are
    implicitly shared, thus the copies are inexpensive unless a
    non-const function is called on them, triggering a copy-on-write.

    Generally for triangulated geometry, use append().  This function simply
    calls virtual protected functions appendSmooth() (for smoothed vertices)
    and appendFaceted() (for faceted vertices).  See QGLDisplayList for a
    discussion of smoothing.
*/

uint qHash(float data)
{
    union U {
        quint32 n;
        float f;
    };
    U u;
    u.f = data;
    return u.f;
}

uint qHash(double data)
{
    union U {
        quint32 n;
        double f;
    };
    U u;
    u.f = data;
    return u.f;
}

uint qHash(const QVector3D &v)
{
    return qHash(v.x()) ^ qHash(v.y()) ^ qHash(v.z());
}

class QGLSectionPrivate
{
public:
    typedef QHash<QVector3D, int> VecMap;
    typedef QHash<int, QVector3D> NormMap;

    QGLSectionPrivate()
        : data(new QGeometryData)
        , map(new VecMap)
        , norms(new NormMap)
        , finalized(false)
    {
    }
    ~QGLSectionPrivate()
    {
        delete data;
        delete map;
        delete norms;
    }
    void finalize()
    {
        delete map;
        map = 0;
        delete norms;
        norms = 0;
        finalized = true;
    }
    bool normalAccumulated(int index, const QVector3D &norm) const
    {
        Q_ASSERT(norms);  // dont call this after finalizing
        QGLSectionPrivate::NormMap::const_iterator nit = norms->constFind(index);
        while (nit != norms->constEnd() && nit.key() == index)
            if (*nit++ == norm)
                return true;
        return false;
    }
    void accumulateNormal(int index, const QVector3D &norm)
    {
        Q_ASSERT(norms);  // dont call this after finalizing
        norms->insertMulti(index, norm);
    }


    QGeometryData *data;
    VecMap *map;
    NormMap *norms;
    bool finalized;
};

/*!
    \fn QGLSection::QGLSection(QGLDisplayList *d,  QGL::Smoothing s)
    Construct a new QGLSection on the display list \a d, and with smoothing \a s.
    By default the smoothing is QGL::Smooth.

    See QGLDisplayList for a discussion of smoothing.

    The pointer \a d must be non-null, and in debug mode, unless QT_NO_DEBUG is
    defined, this function will assert if \a d is null.

    The following lines of code have identical effect:
    \code
    QGLSection *s = myDisplayList->newSection(QGL::Faceted);
    QGLSection *s2 = new QGLSection(myDisplayList, QGL::Faceted);
    \endcode

    The start of the section is initialized to be the first unassigned
    index of the display list, in other words the current size of the
    list.
*/
QGLSection::QGLSection(QGLDisplayList *d,  QGL::Smoothing s)
    : m_smoothing(s)
    , m_displayList(d)
    , m_dataTypes(QLogicalVertex::Vertex)
    , d(new QGLSectionPrivate)
{
    Q_ASSERT(m_displayList);
    m_displayList->addSection(this);
}

/*!
    Destroy this QGLSection, recovering any resources.
*/
QGLSection::~QGLSection()
{
    delete d;
}

/*!
    Finalizes this section by squeezing the data to fit in the optimal memory
    and normalizing any normal vectors to unit length.
*/
void QGLSection::finalize()
{
    if (d->finalized)
        return;
    d->finalize();
    d->data->squeeze();
    d->data->normalizeNormals();
}

/*!
    Returns a bounding box for the vertices in this section.
*/
QBox3D QGLSection::boundingBox() const
{
    return d->data->boundingBox();
}

/*!
    Adds the logical vertex \a lv to this section of a display list.

    The vertex will be drawn as part of a smooth continuous surface, with
    no distinct edge.  To acheive this, duplicates of vertex \a lv are
    coalesced into one vertex (within this section).  This one vertex
    has its normal set to the average of supplied normals.

    Call this function to add the vertices of a smooth face to the section
    of a display list, or use:

    \code
    myDisplayList->openSection(QGLDisplayList::Smooth);
    myDisplayList->addTriangle(a, b, c);
    \endcode

    In smooth surfaces, the vertex and its normal is only sent to the
    graphics hardware once (not once per face), thus smooth geometry may
    consume fewer resources.

    \sa appendFaceted(), openSection()
*/
void QGLSection::appendSmooth(const QLogicalVertex &lv)
{
    Q_ASSERT(!d->finalized);
    Q_ASSERT(lv.hasType(QLogicalVertex::Vertex));
    Q_ASSERT(lv.hasType(QLogicalVertex::Normal));
    d->data->enableType(QLogicalVertex::Normal);
    QGLSectionPrivate::VecMap::const_iterator it = d->map->constFind(lv.vertex());
    if (it == d->map->constEnd())
    {
        int v = d->data->appendVertex(lv);
        //qDebug() << lv << "not found, adding at" << v << "here";
        d->map->insert(lv.vertex(), v);
    }
    else
    {
        //qDebug() << lv << "found at" << v;
        if (updateTexCoord(*it, lv.texCoord()) == -1)
        {
            // did not create a dup vert due to texture seam, add here
            d->data->appendIndex(*it);
            do
            {
                // accumulate normals to this copy of vert, and any seam dups
                if (!d->normalAccumulated(*it, lv.normal()))
                {
                    QVector3D *va = d->data->normalData();
                    va[*it] += lv.normal();
                    d->accumulateNormal(*it, lv.normal());
                }
                ++it;

            } while (it != d->map->constEnd() && it.key() == lv.vertex());
        }
    }
    m_displayList->setDirty(true);
}

/*!
    Add the logical vertex \a lv to this section of a display list.

    The vertex will be drawn as a distinct edge, instead of just part of a
    continuous smooth surface.  To acheive this a duplicate of \a a is
    added for each normal \a n (in the current section).

    Call this method to add the vertices of a faceted face to the list, or
    use:

    \code
    myDisplayList->newSection(QGLDisplayList::Faceted);
    myDisplayList->addTriangle(a, b, c);
    \endcode

    In faceted surfaces, the vertex is sent to the graphics hardware once for
    each normal it has, and thus may consume more resources.

    \sa appendSmooth()
*/
void QGLSection::appendFaceted(const QLogicalVertex &lv)
{
    Q_ASSERT(!d->finalized);
    Q_ASSERT(lv.hasType(QLogicalVertex::Vertex));
    Q_ASSERT(lv.hasType(QLogicalVertex::Normal));
    d->data->enableType(QLogicalVertex::Normal);
    QGLSectionPrivate::VecMap::const_iterator it = d->map->constFind(lv.vertex());
    qDebug() << "#####" << this << "appendFaceted(" << lv << ")";
    if (it != d->map->constEnd())
    {
        qDebug() << "--- something matched at" << *it << "Key:" << it.key();
    }
    const QVector3D *vn = d->data->normalConstData();
    const QVector2D *vt = 0;
    if (d->data->hasType(QLogicalVertex::Texture))
        vt = d->data->texCoordConstData();
    for ( ; it != d->map->constEnd() && it.key() == lv.vertex(); ++it)
    {
        qDebug() << "comparing existing:" << vn[*it] << " - to incoming:" << lv.normal();
        if (qFuzzyCompare(vn[*it], lv.normal()))
        {
            if (vt && lv.hasType(QLogicalVertex::Texture))
            {
                if (qFuzzyCompare(vt[*it], lv.texCoord()))
                {
                    qDebug() << "   matched with normal & texture - found: break";
                    break;
                }
            }
            else
            {
                qDebug() << "    matched with normal only - found: break";
                break;
            }
        }
        else
        {
            qDebug() << "    didn't match";
        }
    }
    if (it != d->map->constEnd() && it.key() == lv.vertex()) // found
    {
        int v = *it;
        qDebug() << lv << "found at" << v;
        int ix = updateTexCoord(v, lv.texCoord());
        if (ix != -1)
        {
            v = ix;
            qDebug() << "after updating, added new for tex coord" << lv.texCoord();
        }
        else
        {
            d->data->appendIndex(v);
        }
    }
    else
    {
        int v = d->data->appendVertex(lv);
        const QVector3D *va = d->data->vertexConstData();
        qDebug() << va[v] << "not found, adding at" << v << "here";
        d->map->insertMulti(lv.vertex(), v);
    }
    m_displayList->setDirty(true);
}

/*!
    Updates texture data at \a position to include value \a t.

    If no texture has been set at \a position then the effect is the same
    as setTexCoord().

    If \a t is QGLTextureSpecifier::InvalidTexCoord this function does
    nothing and returns -1.

    If the vertex at \a position is equal to \a t, then this function does
    nothing and returns -1.

    If the vertex at \a position already has texture coordinates set, then
    a duplicate of the vertex is added, to carry the additional texture
    coordinates.

    Two or more texture coordinates for one logical vertex occurs where a texture
    is split, or wraps around an object and thus a line of vertices on
    the object forms a "seam".

    Returns the index of the vertex that received the texture coordinate
    \a t, which will be either \a index, in the simple case, or a new index
    in the case of a seam.

    \sa appendSmooth(), appendFaceted()
*/
int QGLSection::updateTexCoord(int index, const QVector2D &t)
{
    int v = -1;
    d->data->enableType(QLogicalVertex::Texture);
    QVector2D *vt = d->data->texCoordData();
    if (t != QLogicalVertex::InvalidTexCoord  && t != vt[index])
    {
        if (vt[index] == QLogicalVertex::InvalidTexCoord)
        {
            v = index;
            vt[index] = t;
        }
        else
        {
            QLogicalVertex vx = d->data->vertexAt(index);
            vx.setTexCoord(t);
            v = d->data->appendVertex(vx);
            d->map->insert(vx.vertex(), v);
        }
        m_displayList->setDirty(true);
    }
    return v;
}

void QGLSection::appendFlat(const QLogicalVertex &lv)
{
    d->data->appendVertex(lv);
}

QGL::VectorArray QGLSection::vertices() const
{
    return d->data->vertices();
}

QGL::VectorArray QGLSection::normals() const
{
    return d->data->normals();
}

QGL::IndexArray QGLSection::indices() const
{
    return d->data->indices();
}

QGL::TexCoordArray QGLSection::texCoords() const
{
    return d->data->texCoords();
}

QGL::ColorArray QGLSection::colors() const
{
    return d->data->colors();
}

QGLVertexArray QGLSection::toVertexArray() const
{
    return d->data->toVertexArray();
}

/*!
    Returns the vertex data for the \a index.  The \a index must be valid.
*/
QLogicalVertex QGLSection::vertexAt(int index) const
{
    return d->data->vertexAt(index);
}

/*!
    Sets the vertex at \a position to have coordinates \a v.

    The \a position must be a valid vertex, in other words, one that has
    already been added by one of the append functions.

    \sa appendSmooth(), appendFaceted()
*/
void QGLSection::setVertex(int position, const QVector3D &v)
{
    Q_ASSERT(position < d->data->vertexCount());
    QVector3D *va = d->data->vertexData();
    va[position] = v;
    m_displayList->setDirty(true);
}

/*!
    Sets the vertex normal at \a position to have value \a n.

    If \a n is a null vector, in other words an invalid normal, then this
    function does nothing.

    The \a position must be a valid vertex: in other words, one that has
    already been added by one of the append functions.

    \sa appendSmooth(), appendFaceted()
*/
void QGLSection::setNormal(int position, const QVector3D &n)
{
    if (!n.isNull())
    {
        Q_ASSERT(position < d->data->vertexCount());
        Q_ASSERT(d->data->hasType(QLogicalVertex::Normal));
        QVector3D *vn = d->data->normalData();
        vn[position] = n;
        m_displayList->setDirty(true);
    }
}

/*!
    Sets the vertex at \a position to have texture coordinate value \a t.

    If \a t is equal to QGLTextureSpecifier::InvalidTexCoord then this
    function does nothing.

    The \a position must be a valid vertex with texture data, in other words,
    one that has already been added by one of the append functions.

    \sa updateTexCoord()
*/
void QGLSection::setTexCoord(int position, const QVector2D &t)
{
    if (t != QLogicalVertex::InvalidTexCoord)
    {
        Q_ASSERT(position < d->data->vertexCount());
        Q_ASSERT(d->data->hasType(QLogicalVertex::Texture));
        QVector2D *vt = d->data->texCoordData();
        vt[position] = t;
        m_displayList->setDirty(true);
    }
}

/*!
    Sets the vertex color at \a position to have value \a c.

    The \a position must be a valid vertex with color data, in other words,
    one that has already been added by one of the append functions.

    \sa appendColor()
*/
void QGLSection::setColor(int position, const QColor4b &c)
{
    Q_ASSERT(position < d->data->vertexCount());
    Q_ASSERT(d->data->hasType(QLogicalVertex::Color));
    QColor4b *vc = d->data->colorData();
    vc[position] = c;
    m_displayList->setDirty(true);
}

/*!
    \fn QGL::Smoothing QGLSection::smoothing() const
    Returns the smoothing mode for this section.

    \sa QGL::Smoothing
*/

/*!
   \fn int QGLSection::count() const

   Returns the current count of vertices referenced for this section.  This is
   the same as \c{section->indices().count()} (but faster).
*/
int QGLSection::count() const
{
    return d->data->count();
}

/*!
    \fn QGLDisplayList *QGLSection::displayList() const

    Returns the display list associated with this section.
*/

/*!
    \fn void QGLSection::draw(QGLPainter *painter) const
    Draws this section on the \a painter.
*/

/*!
    \fn QString QGLSection::name() const

    Returns the QString name assigned to this section, or a null QString
    if no name has been assigned.  The name may be used to locate a
    section by name, or to capture model domain information.  It is also
    printed out by the QDebug stream operator.

    \sa setName()
*/

/*!
    \fn void QGLSection::setName(const QString &name)

    Sets the QString \a name to be assigned to this section.

    \sa name()
*/

#ifndef QT_NO_DEBUG_STREAM
/*!
    Output a string representation of the QGLSection to a debug stream.
    \relates QGLSection
*/
QDebug operator<<(QDebug dbg, const QGLSection &section)
{
    dbg.space()
            << "QGLSection("
            << ", count:" << section.count()
            << ", smoothing mode:" << (section.smoothing() == QGL::Smooth ?
                                       "QGL::Smooth" : "QGL::Faceted");
    QGL::IndexArray indices = section.indices();
    for (int i = 0; i < section.count(); ++i)
    {
        int ix = indices[i];
        dbg << section.vertexAt(ix);
    }
    dbg << ")\n";
    return dbg.space();
}
#endif
