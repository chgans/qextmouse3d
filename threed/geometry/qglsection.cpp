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

#include <QtGui/qvector3d.h>

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
    QGLDisplayList - the start and count of this range are available via
    the start() and count() methods.

    A QGLDisplayList instance has the \l{QGLDisplayList::newSection()}{newSection()}
    function which creates a new QGLSection to reference its data.  Use this
    to construct new QGLSection instances, or alternatively construct
    a new QGLSection() and pass a non-null QGLDisplayList pointer.

    The draw() method draws just the vertices in the range of the section.
    Call the QGLDisplayList::draw() convenience function to draw all the
    QGLSection objects that make up the list.

    All faces in a QGLSection are triangles, although if drawn with the
    addQuad method, the two adjoining triangles making up the quad are made
    without a hard edge between them.  The triangle data can be accessed
    by indexing the QGLDisplayList via the start() and count() methods of
    the QGLSection class:
    \code
    const QVector<QVector3D> &v = displayList->vertexArray();
    qreal area = 0.0;
    int start = section->start();
    int end = start + section->count();
    for (int i = start; i < end; i += 3)
        area += calcTriangleArea(v[i], v[i+1], v[i+2]);
    \endcode
*/

/*!
    \enum QGLSection::GLDataFlag
    This enumeration specifies which types of OpenGL data this section is
    utilizing for its geometry.
    \value Vertex Only vertex data is utilized
    \value Normals Normal vector data is utilized
    \value Textures Texture coordinate data is utilized
    \value Color per vertex data is utilized
*/

class QGLSectionPrivate
{
public:
    typedef QMap<QVector3DMapped, int> VecMap;

    VecMap map;
    QVector3DMapped::VList smoothedNormals;
    QGeometryData *data;
    bool uploaded;
};

/*!
    \fn QGLSection::QGLSection(QGLDisplayList *d,  QGL::Smoothing s)
    Construct a new QGLSection on the display list \a d, and with smoothing \a s.
    By default the smoothing is QGL::Smooth.

    The \a d must be non-null, and in debug mode, unless QT_NO_DEBUG is
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
    : m_start(0)
    , m_count(0)
    , m_smoothing(s)
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


void QGLSection::draw(QGLPainter *painter) const
{
    m_displayList->finalize();
    m_displayList->draw(painter, m_start, m_count);
}

/*!
    Adds the logical vertex \a lv to this display list.

    The vertex will be drawn as part of a smooth continuous surface, with
    no distinct edge.  To acheive this, duplicates of vertex \a lv are
    coalesced into one vertex (within this section).  This one vertex
    has its normal set to the average of supplied normals.

    Call this function to add the vertices of a smooth face to the display
    list, or use:

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
    Q_ASSERT(lv.hasType(QLogicalVertex::Vertex));
    Q_ASSERT(lv.hasType(QLogicalVertex::Normal));
    QGLSectionPrivate::VecMap::iterator it = d->map.find(&lv.vertex());
    if (it == d->map.end())
    {
        int v = d->data->appendVertex(lv);
        qDebug() << lv << "not found, adding at" << v << "here";
        d->map.insert(&d->data->vertices[v], v);
    }
    else
    {
        int v = it.value();
        qDebug() << lv << "found at" << v;
        int ix = updateTexCoord(v, lv.texCoord());
        if (ix != -1)
            v = ix;
        if (ix != -1)
            qDebug() << "after updating tex coord" << lv.texCoord();
        QVector3DMapped &vm = const_cast<QVector3DMapped&>(it.key());
        if (!vm.findNormal(lv.normal(), d->smoothedNormals))
        {
            for ( ; it != d->map.end() && it.key() == lv.vertex(); ++it)
                (*d->data->normals)[it.value()] += lv.normal();
            vm.appendNormal(v, d->data, d->smoothedNormals);
        }
    }
    m_count += 1;
    m_displayList->setDirty(true);
}

/*!
    Add the \a vertex to this display list.

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
    Q_ASSERT(lv.hasType(QLogicalVertex::Vertex));
    Q_ASSERT(lv.hasType(QLogicalVertex::Normal));
    QGLDisplayListPrivate::VecMap::const_iterator it = d->map.constFind(&lv.vertex());
    for ( ; it != d->map.constEnd() && it.key() == lv.vertex(); ++it)
        if (qFuzzyCompare(d->data->normals[it.value()], lv.normal()))
            break;
    if (it != d->map.constEnd() && it.key() == lv.vertex()) // found
    {
        int v = it.value();
        int ix = updateTexCoord(v, lv.texCoord());
        if (ix != -1)
            v = ix;
    }
    else
    {
        int v = d->data->appendVertex(lv);
        d->map.insertMulti(&d->data->vertices[v], v);
    }
    m_count += 1;
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
    if (t != QGLTextureSpecifier::InvalidTexCoord  && t != d->data->texCoords[index])
    {
        if (d->data->texCoords[index] == QGLTextureSpecifier::InvalidTexCoord)
        {
            v = index;
            d->data->texCoords[index] = t;
        }
        else
        {
            QLogicalVertex vx(d->data->vertexAt(index));
            v = d->data->append(vx);
            d->map.insert(QVector3DMapped(index, d->data), v);
        }
        d->setDirty(true);
    }
    return v;
}

QGL::VectorArray QGLSection::vertices() const
{
    return d->data->vertices;
}

QGL::VectorArray QGLSection::normals() const
{
    return *d->data->normals;
}

QGL::IndexArray QGLSection::indices() const
{
    return d->data->indices;
}

QGL::TexCoordArray QGLSection::texCoords() const
{
    return *d->data->texCoords;
}

QGL::ColorArray QGLSection::colors() const
{
    return *d->data->colors;
}

/*!
    Returns the position of the first vertex matching \a lv in this section.
    If no such matching vertex is found, returns -1.
*/
int QGLSection::indexOf(const QLogicalVertex &lv) const
{
    QGLSectionPrivate::VecMap::const_iterator it = d->map.constFind(lv.vertex());
    while (it != d->map.constEnd())
    {
        if (it.key() != lv.vertex())
            return -1;
        if (lv == d->data->vertexAt(it.value()))
            return it.value();
    }
    return -1;
}

/*!
    Returns the vertex data for the \a index.  The \a index must be valid.
*/
const QLogicalVertex &QGLSection::vertexAt(int index) const
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
    d->data->setVertex(position, v);
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
        d->data->setNormal(position, n);
        m_displayList->setDirty(true);
    }
}

/*!
    Sets the vertex at \a position to have texture coordinate value \a t.

    If \a t is equal to QGLTextureSpecifier::InvalidTexCoord then this
    function does nothing.

    The \a position must be a valid vertex, in other words, one that has
    already been added by one of the append functions.

    \sa updateTexCoord()
*/
inline void QGLSection::setTexCoord(int position, const QVector2D &t)
{
    if (t != QGLTextureSpecifier::InvalidTexCoord)
    {
        d->data->setTexCoords(position, t);
        m_displayList->setDirty(true);
    }
}

/*!
    Sets the vertex color at \a position to have value \a c.

    The \a position must be a valid vertex, in other words, one that has
    already been added by one of the append functions.

    \sa appendColor()
*/
void QGLSection::setColor(int position, const QColor4b &c)
{
    d->data->setColor(position, c);
    m_displayList->setDirty(true);
}

/*!
    \fn QGL::Smoothing QGLSection::smoothing() const
    Returns the smoothing mode for this section.

    \sa QGL::Smoothing
*/

/*!
   \fn int QGLSection::start() const

   Returns the current start index for this section, as an index into
   the associated QGLDisplayList.

   \sa setStart(), displayList()
*/

/*!
    \fn void QGLSection::setStart(int s)

    Sets the current start index for this section, to \a s, an index into
    the associated QGLDisplayList

    \sa start(), displayList()
*/

/*!
   \fn int QGLSection::count() const

   Returns the current index count for this section, as a count of indexed
   vertices in the associated QGLDisplayList.

   While this section is not finalized this value may increase as new
   geometry is added to it.

   \sa displayList()
*/

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
            << ", start:" << section.start()
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
