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

#include "qglsection_p.h"
#include "qgldisplaylist_p.h"
#include "qgeometrydata_p.h"

#include <QtGui/qvector3d.h>
#include <QtCore/qdebug.h>

/*!
    \internal
    \class QGLSection
    \brief The QGLSection class clusters like geometry in a QGLDisplayList.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::geometry

    QGLSection instances partition a QGLDisplayList into related sections,
    while the display list is being initialized with geometry data.

    Once the display list is initialized, and geometry building is complete
    the QGLSection instances are destroyed and the data is uploaded to the
    graphics hardware.

    The QGLSection class is a work horse for the QGLDisplayList, and it
    takes care of automatically managing vertex data.  As such
    for usual use cases, its functionality will not need to be referenced
    directly.  For low-level access to geometry, QGLSection provides a
    range of accessors to reference geometry data during scene building.

    Within a section, incoming geometry data will be coalesced and
    indexes created to reference the fewest possible copies of the vertex
    data.  For example, in smooth geometry all copies of a vertex are
    coalesced into one, and referenced by indices - except in the case
    where texture data forms a \i seam and a copy must be created to carry
    the two texture coordinates of the seam.

    \image texture-seam.png

    This is handled automatically by QGLSection, to pack data into the
    smallest space possible thus improving cache coherence and performance.

    All the vertices in a QGLSection are treated with the same
    \l{QGL::Smoothing}{smoothing}, and have the same
    \l{QLogicalVertex::Type}{data types}.

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

    Generally for adding geometry, use append().  This function simply
    calls virtual protected functions appendSmooth() (for smoothed vertices)
    and appendFaceted() (for faceted vertices).  See QGLDisplayList for a
    discussion of smoothing.

    Call a sections finalize() method to calculate normals.  Prior to
    calling finalize() relying on normal values results in undefined behavior.
    \code
    // WRONG
    displayList->extrude(verts);
    QGL::VectorArray normals = displayList->currentSection()->normals();
    doSomething(normals[0]);  // normals undefined at this point
    \endcode

    \code
    // RIGHT
    displayList->extrude(verts);
    displayList->currentSection()->finalize();
    // OR...
    displayList->newSection();  // implicitly finalizes the previous section
    // OR...
    displayList->finalize();  // also finalizes that section
    QGL::VectorArray normals = displayList->currentSection()->normals();
    doSomething(normals[0]);  // normals now defined
    \endcode

    Note that after initialization of the display list, the QGLSection
    instances are destroyed.  To access the geometry data during the run-time
    of the application, if needed, call the geometry() method on the
    display list and its child nodes.
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
        quint64 n;
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
    typedef QHash<QVector3D, int> VecHash;
    typedef QHash<int, QVector3D> NormHash;

    QGLSectionPrivate()
        : data(new QGeometryData)
        , hash(new VecHash)
        , norms(new NormHash)
        , finalized(false)
    {
    }
    ~QGLSectionPrivate()
    {
        delete data;
        delete hash;
        delete norms;
    }
    bool normalAccumulated(int index, const QVector3D &norm) const
    {
        Q_ASSERT(norms);  // dont call this after finalizing
        NormHash::const_iterator nit = norms->constFind(index);
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
    VecHash *hash;
    NormHash *norms;
    bool finalized;
};

/*!
    \internal
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
*/
QGLSection::QGLSection(QGLDisplayList *d,  QGL::Smoothing s)
    : m_smoothing(s)
    , m_displayList(d)
    , d(new QGLSectionPrivate)
{
    Q_ASSERT(m_displayList);
    m_displayList->addSection(this);
}

/*!
    \internal
    Destroy this QGLSection, recovering any resources.
*/
QGLSection::~QGLSection()
{
    delete d;
}

/*!
    \internal
    Completes the calculations of normal values for this section.  This
    function also performs local memory optimizations on the section.
*/
void QGLSection::finalize()
{
    if (d->finalized)
        return;
    d->data->squeeze();
    d->data->normalizeNormals();
    d->finalized = true;
}

/*!
    \internal
    Returns true if this section has been finalized, false otherwise.

    \sa finalize()
*/
bool QGLSection::isFinalized() const
{
    return d->finalized;
}

/*!
    \internal
    Returns a bounding box for the vertices in this section.
*/
QBox3D QGLSection::boundingBox() const
{
    return d->data->boundingBox();
}

/*!
    \internal
    \fn void QGLSection::append(const QLogicalVertex &vertex)
    Adds the logical \a vertex to this section.

    If the \a vertex has no lighting normal component, then the append will
    be done by calling appendFlat().

    Otherwise, if the \a vertex does have a lighting normal; then the
    vertex processing depends on the smoothing property of this section.
    If this section has smoothing QGL::Smooth, then the append will be done
    by calling appendSmooth(); or if this section has smoothing QGL::Faceted,
    then the append will be done by calling appendFaceted().

    \sa appendSmooth(), appendFaceted(), appendFlat()
*/

/*!
    \internal
    Adds the logical vertex \a lv to this section of a display list.

    The vertex will be drawn as part of a smooth continuous surface, with
    no distinct edge.  To acheive this, duplicates of vertex \a lv are
    coalesced into one vertex (within this section).  This one vertex
    has its normal set to the average of supplied normals.

    Vertices are not coalesced if \a lv has a different texture coordinate
    than its duplicate.  See updateTexCoord() for details.

    Call this function to add the vertices of a smooth face to the section
    of a display list, or use:

    \code
    myDisplayList->newSection(QGLDisplayList::Smooth);
    myDisplayList->addTriangle(a, b, c);
    \endcode

    In smooth surfaces, the vertex and its normal is only sent to the
    graphics hardware once (not once per face), thus smooth geometry may
    consume fewer resources.

    \sa appendFaceted(), updateTexCoord(), QGLDisplayList::newSection()
*/
void QGLSection::appendSmooth(const QLogicalVertex &lv)
{
    Q_ASSERT(lv.hasType(QLogicalVertex::Vertex));
    Q_ASSERT(lv.hasType(QLogicalVertex::Normal));
    QGLSectionPrivate::VecHash::const_iterator it = d->hash->constFind(lv.vertex());
    if (it == d->hash->constEnd())
    {
        int v = d->data->appendVertex(lv);
        d->hash->insert(lv.vertex(), v);
        d->accumulateNormal(v, lv.normal());
    }
    else
    {
        if (updateTexCoord(*it, lv.texCoord()) == -1)
        {
            d->data->appendIndex(*it);
            do
            {
                if (!d->normalAccumulated(*it, lv.normal()))
                {
                    QVector3D *va = d->data->normalData();
                    va[*it] += lv.normal();
                    d->accumulateNormal(*it, lv.normal());
                }
                ++it;

            } while (it != d->hash->constEnd() && it.key() == lv.vertex());
        }
    }
    d->finalized = false;
    m_displayList->setDirty(true);
}

/*!
    \internal
    Add the logical vertex \a lv to this section of a display list.

    The vertex will be drawn as a distinct edge, instead of just part of a
    continuous smooth surface.  To acheive this the vertex value of \a lv
    is duplicated for each unique normal in the current section.  However
    if a vertex is already present with a given normal it is coalesced
    and simply referenced by index.

    Vertices are not coalesced if \a lv has a different texture coordinate
    than its duplicate.  See updateTexCoord() for details.

    Call this method to add the vertices of a faceted face to the list, or
    use:

    \code
    myDisplayList->newSection(QGLDisplayList::Faceted);
    myDisplayList->addTriangle(a, b, c);
    \endcode

    In faceted surfaces, the vertex is sent to the graphics hardware once for
    each normal it has, and thus may consume more resources.

    \sa appendSmooth(), updateTexCoord(), QGLDisplayList::newSection()
*/
void QGLSection::appendFaceted(const QLogicalVertex &lv)
{
    Q_ASSERT(lv.hasType(QLogicalVertex::Vertex));
    Q_ASSERT(lv.hasType(QLogicalVertex::Normal));
    QGLSectionPrivate::VecHash::const_iterator it = d->hash->constFind(lv.vertex());
    const QVector3D *vn = d->data->normalConstData();
    const QVector2D *vt = 0;
    if (d->data->hasType(QLogicalVertex::Texture))
        vt = d->data->texCoordConstData();
    for ( ; it != d->hash->constEnd() && it.key() == lv.vertex(); ++it)
    {
        if (vn && qFuzzyCompare(vn[*it], lv.normal()))
        {
            if (vt && lv.hasType(QLogicalVertex::Texture))
            {
                if (qFuzzyCompare(vt[*it], lv.texCoord()))
                    break;
            }
            else
            {
                break;
            }
        }
    }
    if (it != d->hash->constEnd() && it.key() == lv.vertex()) // found
    {
        if (updateTexCoord(*it, lv.texCoord()) == -1)
        {
            d->data->appendIndex(*it);
        }
    }
    else
    {
        int v = d->data->appendVertex(lv);
        d->hash->insertMulti(lv.vertex(), v);
    }
    d->finalized = false;
    m_displayList->setDirty(true);
}

/*!
    \internal
    Updates texture data at \a index to include value \a t.  This function
    is called by appendSmooth() and appendFaceted() when incoming vertex
    data is to be coalesced, but has a texture coordinate associated.

    If \a t is QGLTextureSpecifier::InvalidTexCoord this function does
    nothing and returns -1.

    The texture data at \a index is examined, and therefore this section
    must have texture data, ie \c{hasData(QLogicalVertex::Texture)} is true; and
    the \a index must be a valid vertex index.

    If the texture data at \a index is equal to \a t, then this function does
    nothing and returns -1.

    If no texture has been set at \a index then the effect is the same
    as setTexCoord().

    If the vertex at \a index already has texture coordinates set, then
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
    if (t != QLogicalVertex::InvalidTexCoord)
    {
        Q_ASSERT(d->data->hasType(QLogicalVertex::Texture));
        Q_ASSERT(index < d->data->vertexCount());
        QVector2D *vt = d->data->texCoordData();
        if (t != vt[index])
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
                d->hash->insert(vx.vertex(), v);
            }
            d->finalized = false;
            m_displayList->setDirty(true);
        }
    }
    return v;
}

/*!
    \internal
    Append the logical vertex \a lv to this section.

    The vertex will be treated as flat colored, and thus no management
    of lighting normals is done.
*/
void QGLSection::appendFlat(const QLogicalVertex &lv)
{
    d->data->appendVertex(lv);
    d->finalized = false;
    m_displayList->setDirty(true);
}

/*!
    \internal
    Return a copy of the vertex data for this section.  Since the data
    is implicitly shared this call is inexpensive, unless the copy is
    modified.
*/
QGL::VectorArray QGLSection::vertices() const
{
    return d->data->vertices();
}

/*!
    \internal
    Return a copy of the normal data for this section.  Since the data
    is implicitly shared this call is inexpensive, unless the copy is
    modified.
*/
QGL::VectorArray QGLSection::normals() const
{
    return d->data->normals();
}

/*!
    \internal
    Return a copy of the index data for this section.  Since the data
    is implicitly shared this call is inexpensive, unless the copy is
    modified.
*/
QGL::IndexArray QGLSection::indices() const
{
    return d->data->indices();
}

/*!
    \internal
    Return a copy of the texture coordinate data for this section.
    Since the data is implicitly shared this call is inexpensive, unless
    the copy is modified.
*/
QGL::TexCoordArray QGLSection::texCoords() const
{
    return d->data->texCoords();
}

/*!
    \internal
    Return a copy of the color data for this section.  Since the data
    is implicitly shared this call is inexpensive, unless the copy is
    modified.
*/
QGL::ColorArray QGLSection::colors() const
{
    return d->data->colors();
}

/*!
    \internal
    Return a copy of the data for this section as a vertex array.  The
    data is copied element-wise into a QGLVertexArray so this call is
    expensive.
*/
QGLVertexArray QGLSection::toVertexArray() const
{
    return d->data->toVertexArray();
}

/*!
    \internal
    Returns the vertex data for the \a index.  The \a index must be valid.
*/
QLogicalVertex QGLSection::vertexAt(int index) const
{
    return d->data->vertexAt(index);
}

/*!
    \internal
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
    \internal
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
    \internal
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
    \internal
    Sets the vertex color at \a position to have value \a c.

    The \a position must be a valid vertex with color data, in other words,
    one that has already been added by one of the append functions.

    \sa appendFlat()
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
    \internal
    \fn QGL::Smoothing QGLSection::smoothing() const
    Returns the smoothing mode for this section.

    \sa QGL::Smoothing
*/

/*!
    \internal
   \fn int QGLSection::count() const

   Returns the current count of vertices referenced for this section.  This is
   the same as \c{section->indices().count()} (but faster).
*/
int QGLSection::count() const
{
    return d->data->count();
}

/*!
    \internal
    Returns the types of data that this section contains.
*/
QLogicalVertex::Types QGLSection::dataTypes() const
{
    return d->data->types();
}

/*!
    \internal
    Returns true if this section has data of \a types.
*/
bool QGLSection::hasData(QLogicalVertex::Types types)
{
    return d->data->hasType(types);
}

/*!
    \internal
    Forces this section to have \a types of data.

    Normally the append functions will track what types of data are
    required by examining the data contained in vertex arguments passed
    to the append functions, and thus this method need not be called.
*/
void QGLSection::enableTypes(QLogicalVertex::Types types)
{
    d->data->enableTypes(types);
}

/*!
    \internal
    \fn QGLDisplayList *QGLSection::displayList() const

    Returns the display list associated with this section.
*/

#ifndef QT_NO_DEBUG_STREAM
/*!
    \internal
    Output a string representation of \a section to a debug stream \a dbg.
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
