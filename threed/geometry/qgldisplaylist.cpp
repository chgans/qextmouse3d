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

#include "qgldisplaylist.h"
#include "qglsection.h"
#include "qglmaterialcollection.h"
#include "qglpainter.h"
#include "qglgeometry_p.h"

#include <QtCore/qobject.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/private/qobject_p.h>
#include <QtCore/qmap.h>
#include <QtCore/qvarlengtharray.h>
#include <QtGui/qvector2d.h>

#include <QtCore/qdebug.h>

#include <limits>

/*!
    \class QGLDisplayList
    \brief The QGLDisplayList class accumulates geometry for efficient display.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::geometry

    Use a QGLDisplayList to build up vertex, index, texture and other data
    when an application starts up, then it can be efficiently and flexibly
    displayed during frames of rendering.

    Traditional OpenGL programming uses display lists to build geometry once
    during an initialization phase, and then efficiently display that geometry
    each frame of rendering.

    While the underlying implementation of QGLDisplayList does not use the same
    OpenGL calls (since those calls are not implemented on all platforms) it
    provides the same convenience and improved performance.

    The geometry data in a QGLDisplayList is divided into QGLSection
    instances which reference sections of the geometry in order to
    display them with different attributes.  For example a
    QGLDisplayList representing a 3D can of soup might have a smooth
    QGLSection for the rounded outside cylinder and a faceted QGLSection
    instance for the flat top and bottom.

    \snippet displaylist/displaylist.cpp 0

    Alternatively for unusual requirements, QGLDisplayList may be sub-classed
    and the range of append methods used to add geometry vertex by vertex.

    Accessor functions are provided for getting copies of the data in
    the display list:
    \list
        \o vertexArray()
        \o normalArray()
        \o texCoordArray()
        \o colorArray()
        \o indexArray()
        \o materials()
    \endlist

    These functions all return QVector values (except for the
    materials() function which returns a pointer). QVector instances are
    implicitly shared, thus the copies are inexpensive unless a
    non-const function is called, triggering a copy-on-write.

    \section1 Sub-classing QGLDisplayList

    Generally for triangulated geometry, use appendTriangle() or appendQuad().
    If the geometry cannot be built this way, sub-class QGLDisplayList and
    (for example) create a build() function which calls the append functions.
    The build function can be called during initialization.

    When subclassing and using the append functions it will be seen that
    QGLDisplayList supports the following two main types of scenes:
    \list
        \o lit scenes; with lighting and materials, and
        \o flat color scenes; wth per-vertex colors
    \endlist

    For lit scenes use appendSmooth() or appendFaceted() to supply lighting
    normals along with each vertex.  For flat color scenes use appendColor()
    to supply color values for each vertex.  Flat scenes can also use append()
    to create uncolored vertices.

    Within the lit scenes QGLDisplayList allows creation of smooth surfaces,
    via the appendSmooth() function, to model for example a light globe; and
    faceted surfaces via the appendFaceted() function, to model for example a
    gemstone or dice.

    Smooth surfaces are continuous over vertices, where faceted surfaces have
    hard edges at the vertices.  In reality both surfaces are made of faces,
    but in the smooth case lighting normals are calculated to make for a smooth
    appearance.

    Once the geometry has been accumulated in the display list, first call
    the finalize() method to complete the geometry and optimize it for
    display; then call the loadArrays() method to register the data for
    OpenGL to render it.  Generally finalize() only needs to be called once
    in the application lifetime.  loadArrays() may need to be called more
    often if multiple display lists are being used.

    The finalize() and loadArrays() functions are called by the default
    draw() function of QGLDisplayList.

    \sa QGLSection
*/

class QVector3DMapped
{
public:
    typedef QVector<QVector3DMapped> VList;

    QVector3DMapped(int v = -1, QGLDisplayList::VectorArray *sto = 0);
    QVector3DMapped(const QVector3D* vec);
    ~QVector3DMapped();
    inline bool isNull() const;
    operator QVector3D () const;
    inline bool operator<(const QVector3DMapped &rhs) const;
    inline bool operator==(const QVector3D &rhs) const;
    inline bool QVector3DMapped::findNormal(const QVector3D &norm,
                                            const VList &list) const;
    inline void appendNormal(int, QGLDisplayList::VectorArray *, QVector3DMapped::VList &);

    QString toString() const {
        const QVector3D &v = vecPtr ? *vecPtr : store->at(vecIx);
        QString vs = QString("QVector3D( %1, %2, %3 )").arg(v.x()).arg(v.y()).arg(v.z());
        return vs + "\n" + QString("[QVector3DMapped - vecIx: %1 - vecPtr: %2 - normPtr: %3 - store %4]\n").arg(vecIx).arg((int)vecPtr).arg(normPtr).arg((int)store);
    }

//private:
    int vecIx;
    const QVector3D *vecPtr;
    int normPtr;
    QGLDisplayList::VectorArray *store;
};

inline bool QVector3DMapped::isNull() const
{
    return (vecPtr == NULL) && ((vecIx < 0) || (store == NULL));
}

QVector3DMapped::QVector3DMapped(int v, QGLDisplayList::VectorArray *sto)
    : vecIx(v)
    , vecPtr(0)
    , normPtr(-1)
    , store(sto)
{
}

QVector3DMapped::QVector3DMapped(const QVector3D* vec)
    : vecIx(-1)
    , vecPtr(vec)
    , normPtr(-1)
    , store(0)
{
}

QVector3DMapped::~QVector3DMapped()
{
}

QVector3DMapped::operator QVector3D () const
{
    Q_ASSERT(!isNull());
    return vecPtr ? *vecPtr : store->at(vecIx);
}

inline bool QVector3DMapped::operator<(const QVector3DMapped &rhs) const
{
    Q_ASSERT(!isNull());
    Q_ASSERT(!rhs.isNull());
    const QVector3D &a = vecPtr ? *vecPtr : store->at(vecIx);
    const QVector3D &b = rhs.vecPtr ? *rhs.vecPtr : rhs.store->at(rhs.vecIx);
    if (qFuzzyCompare(a.x(), b.x()))
    {
        if (qFuzzyCompare(a.y(), b.y()))
        {
            if (qFuzzyCompare(a.z(), b.z()))
            {
                return false;
            }
            else
            {
                return a.z() < b.z();
            }
        }
        else
        {
            return a.y() < b.y();
        }
    }
    else
    {
        return a.x() < b.x();
    }
}

inline bool QVector3DMapped::operator==(const QVector3D &rhs) const
{
    Q_ASSERT(!isNull());
    Q_ASSERT(!rhs.isNull());
    const QVector3D &a = vecPtr ? *vecPtr : store->at(vecIx);
    return qFuzzyCompare(a, rhs);
}

inline bool QVector3DMapped::findNormal(const QVector3D &norm,
                                       const QVector3DMapped::VList &list) const
{
    const QVector3DMapped *nx = this;
    for ( ; nx->normPtr != -1; nx = &(list[nx->normPtr]))
        if (qFuzzyCompare(norm, list[nx->normPtr]))
            return true;
    return false;
}

inline void QVector3DMapped::appendNormal(int vec, QGLDisplayList::VectorArray *normals, QVector3DMapped::VList &list)
{
    QVector3DMapped &nx = *this;
    for ( ; nx.normPtr != -1; nx = list[nx.normPtr]) {}
    nx.normPtr = list.count();
    list.append(QVector3DMapped(vec, normals));
}

class QGLDisplayListPrivate : public QGLGeometryPrivate
{
    Q_DECLARE_PUBLIC(QGLDisplayList)
public:
    QGLDisplayListPrivate(int version = QObjectPrivateVersion);
    ~QGLDisplayListPrivate();
    bool finalizeNeeded;
    bool loadNeeded;
    const QGLContext *context;

    typedef QMap<QVector3DMapped, int> VecMap;
    typedef QMap<QGLSection *, VecMap> DedupMap;
    DedupMap dedupMap;
    VecMap &getMap(QGLSection *s = 0);
    const VecMap &getMap(QGLSection *s = 0) const;
    QVector3DMapped::VList smoothedNormals;
};

QGLDisplayListPrivate::QGLDisplayListPrivate(int version)
    : QGLGeometryPrivate(version)
    , finalizeNeeded(true)
    , loadNeeded(true)
    , context(0)
{
}

QGLDisplayListPrivate::~QGLDisplayListPrivate()
{
}

/*!
    \internal
    Returns a reference to the dedup map for the given section \a s, or if
    \a s is null (the default), the current section.
*/
QGLDisplayListPrivate::VecMap &QGLDisplayListPrivate::getMap(QGLSection *s)
{
    Q_Q(QGLDisplayList);
    Q_ASSERT(s || q->m_currentSection);
    DedupMap::iterator section = s ? dedupMap.find(s) : dedupMap.find(q->m_currentSection);
    Q_ASSERT(section != dedupMap.end());
    return section.value();
}

/*!
    \internal
    \overload
*/
const QGLDisplayListPrivate::VecMap &QGLDisplayListPrivate::getMap(QGLSection *s) const
{
    Q_Q(const QGLDisplayList);
    Q_ASSERT(s || q->m_currentSection);
    DedupMap::const_iterator section = s ? dedupMap.constFind(s) : dedupMap.constFind(q->m_currentSection);
    Q_ASSERT(section != dedupMap.constEnd());
    return section.value();
}

/*!
    Construct a new QGLDisplayList with \a materials and \a parent.  If the
    \a materials argument is null, then a new collection is created internally.
*/
QGLDisplayList::QGLDisplayList(QObject *parent, QGLMaterialCollection *materials)
    : QGLGeometry(*new QGLDisplayListPrivate, parent)
    , m_hasColors(false)
    , m_hasNormals(false)
    , m_hasTexCoords(false)
    , m_materials(materials)
{
    if (!m_materials)
        m_materials = new QGLMaterialCollection(this);
    setPalette(m_materials);
    setDrawingMode(QGL::Triangles);
}

/*!
    Destroys this QGLDisplayList recovering any resources.
*/
QGLDisplayList::~QGLDisplayList()
{
}

/*!
    Draws the contents of the display list onto this painter.
    This is a convenience method which simply calls draw() on all
    the sections of this list.
*/
void QGLDisplayList::draw(QGLPainter *painter)
{
    loadArrays(painter);
    for (int i = 0; i < m_sections.count(); ++i)
        m_sections[i]->draw(painter);
}

/*!
    Reserves space in this display list for \a n vertices.  The sole purpose
    of this function is to fine tune the display lists memory management - fewer
    reallocations in underlying storage will need to be done if the \a n is close
    to the number of vertices actually stored.  If the approximate number of
    vertices to be stored in advance is known then calling this function can
    improve perfomance during building of geometry.
*/
void QGLDisplayList::reserve(int n)
{
    if (m_vertices.capacity() < n)
        m_vertices.reserve(n);
    if (m_indices.capacity() < (n * 3))
        m_indices.reserve(n * 3);
    if (m_hasColors && m_colors.capacity() < n)
        m_colors.reserve(n);
    if (m_hasTexCoords && m_texCoords.capacity() < n)
        m_texCoords.reserve(n);
    if (m_hasNormals && m_normals.capacity() < n)
        m_normals.reserve(n);
}

/*!
    Add to the current section a triangular face with lighting normal \a n; and
    defined by vertices \a a, \a b, and \a c.

    If \a textureModel is non-null then the following texture coordinates will
    be applied to the vertices:
    \list
        \o \a a - bottom-left
        \o \a b - bottom-right
        \o \a c - top-right
    \endllist
    which is the case for a triangle in an upright orientation with respect to
    texture-space, with its apex \a c at the high values of the texture and its
    base \a a - \a c at the low values of the texture.

    \bold{Note:} The \a inverted parameter is intended mostly for internal use.

    If \a inverted is true, the following texture coordinates
    will be applied:
    \list
        \o \a a - bottom-left
        \o \a b - bottom-right
        \o \a c - top-right
    \endllist
    The inverted case is a triangle with the vertex at \a on the low values
    of the texture and the \a b - \a c edge along the high values of the
    texture.
*/
void QGLDisplayList::addTriangle(const QVector3D &a, const QVector3D &b,
                      const QVector3D &c, const QVector3D &n,
                      const QGLTextureSpecifier &textureModel, bool inverted)
{
    QVector3D norm = n;
    if (norm.isNull())
        norm = QVector3D::normal(a, b, c);
    if (!textureModel.isNull())
        m_hasTexCoords = true;
    qDebug() << "Adding triangle:" << a << b << c << " - normal:" << n << "tex:" << textureModel.topLeft() << "-" << textureModel.bottomRight();
    if (m_currentSection->smoothing() == QGL::Smooth)
    {
        appendSmooth(a, norm, textureModel.bottomLeft());
        appendSmooth(b, norm, inverted ? textureModel.topRight() : textureModel.bottomRight());
        appendSmooth(c, norm, inverted ? textureModel.topLeft() : textureModel.topRight());
    }
    else
    {
        appendFaceted(a, norm, textureModel.bottomLeft());
        appendFaceted(b, norm, inverted ? textureModel.topRight() : textureModel.bottomRight());
        appendFaceted(c, norm, inverted ? textureModel.topLeft() : textureModel.topRight());
    }
}

/*!
    Add to this section a quad face defined by vertices \a a, \a b, \a c and
    \a d.  The face is composed of two triangles having the same normal,
    calculated from the face \a a, \a b and \a c.
*/
void QGLDisplayList::addQuad(const QVector3D &a, const QVector3D &b,
                             const QVector3D &c, const QVector3D &d,
                             const QGLTextureSpecifier &textureModel)
{
    QVector3D norm = QVector3D::normal(a, b, c);
    addTriangle(a, b, c, norm, textureModel);
    addTriangle(a, c, d, norm, textureModel, true);
}

/*!
    Adds to this section a set of connected triangles defined by \a center
    and \a edges.

    N triangular faces are generated, where \c{N == edges.count() -1}, each
    formed by taking two vertices from the \a edges, eg edges[0] and edges[1]
    and then \a center.  Other triangles are formed similarly, and all have
    windings and normals as for \c{addTriangle(edges[0], edges[1], center)}.

    This function is very similar to the OpenGL mode GL_TRIANGLE_FAN.

    Textures are generated according to \a textureModel, if textureModel
    is non-null.  When textures are generated all the points on \a edges
    must lie in the same plane.
*/
void QGLDisplayList::addTriangleFan(const QVector3D &center,
                                    const QGLDisplayList::VectorArray &edges,
                                    const QGLTextureSpecifier &textureModel)
{
    Q_ASSERT(edges.count() > 1);
    if (!textureModel.isNull())
        qWarning("NOT IMPLEMENTED YET");
    QVector3D norm = QVector3D::normal(edges[0], edges[1], center);
    for (int i = 0; i < edges.count(); ++i)
        addTriangle(edges[i], edges[(i + 1) % edges.count()], center, norm, textureModel);
}

/*!
    Adds to this section a face defined by \a center and \a edges.  The face
    is composed of N triangular sub-faces, where N == edges.count(), each
    formed by taking two vertices from the \a edges, eg edges[0] and edges[1]
    and then \a center.

    One normal is calculated for the face, since a faces vertices lie in
    the same plane.  The \a center and \e edges must all lie in the same
    plane, and the center vertex must lie strictly within the region
    bounded by the \a edges or  or undefined behaviour will result.

    The face will have normals and windings all equal to the triangle formed
    by \c{addTriangle(edge[0], edge[1], center)}.  Note that since the normal
    calculation can be expensive this function may be faster than
    addTriangleFan() which must calculate a new normal for every sub-face.

    This function also differs from addTriangleFan(), in that it adds a closing
    sub-face \c{(edges[n], edges[0], center)} (ie between the last and first
    vertex listed in \a edges) to form a closed edge.

    Textures are generated according to \a textureModel, if textureModel
    is non-null.
*/
void QGLDisplayList::addTriangulatedFace(const QVector3D &center,
                                         const QGLDisplayList::VectorArray &edges,
                                         const QGLTextureSpecifier &textureModel)
{
    Q_ASSERT(edges.count() > 1);
    if (!textureModel.isNull())
        qWarning("NOT IMPLEMENTED YET");
    QVector3D norm = QVector3D::normal(edges[0], edges[1], center);
    for (int i = 0; i < edges.count(); ++i)
    {
        int n = (i + 1) % edges.count();
        addTriangle(edges[i], edges[n], center, norm, textureModel);
    }
}

/*!
    Add a series of quads based on the \a edges and \a offset given.

    The quads and their normals are generated in such a way that if \a line
    is the perimeter vertices of a polygon F, in counter-clockwise order, and
    \a offset is the inverse of the face normal of polygon F then the
    quads form the sides of a solid rectangular prism with the polygon F
    as its end-face.

    If \a offset is null (the default) then offset is set to be the
    negative (inverse) of the normalized cross-product of the first two
    segments of line: \c{QVector3D::normal(edges[0], edges[1], edges[2])}.
    If relying on this default behaviour ensure that the first 3 vertices
    do not lie on a straight line.

    If the line is less than 2 segments in length and \a offset is null,
    then the offset is set to \c{(0, 0, -1)}.

    \image extrude.png

    In the diagram above, the \i{Source line} is extruded to the \i{Extruded line}
    and one of the generated quads is shown in orange, with its face normal.

    Returns the extruded values of vertices in the line, but in reverse order.
    These values can be used to construct an obverse face, polygon F':
    \code
    QGLDisplayList::VectorArray front;
    front << a << b << c << d;
    QGLDisplayList::VectorArray obverse = displayList->extrude(front);
    // obverse now contains d', c', b' and a' in that order
    displayList->addQuad(obverse[0], obverse[1], obverse[2], obverse[3]);
    \endcode
    Note that the vertices in the returned array are reversed in sequence so
    that the obverse face has the correct winding to face outwards.

    Textures are generated according to \a textureModel, if textureModel
    is non-null.  When textures are generated all the points on \a line
    must lie in the same plane, which is the case anyway if they are the
    perimeter vertices of a polygon.
*/
QGLDisplayList::VectorArray QGLDisplayList::extrude(const QGLDisplayList::VectorArray &edges,
                                           const QVector3D &offset,
                                           const QGLTextureSpecifier &textureModel)
{
    // cannot extrude just a point, need at least 2 points
    Q_ASSERT(edges.count() > 1);
    QVector3D o = offset;
    QGLDisplayList::VectorArray result;
    if (offset.isNull())
    {
        if (edges.count() > 2)
            o = -QVector3D::normal(edges[0], edges[1], edges[2]);
        else
            o = QVector3D(0.0f, 0.0f, -1.0f);
    }
#ifndef QT_NO_DEBUG
    if (o.isNull())
        qWarning("Could not determine non-null offset for extrude");
#endif
    // calculate extruded version of line as result
    for (int i = 0; i < edges.count(); ++i)
        result.append(edges.at(i) + o);
    if (!textureModel.isNull())
    {
        m_hasTexCoords = true;
        // calculate texturecoords
        QGLTextureSpecifier tx(textureModel);
        qreal totalExtents = 0.0f;
        QVarLengthArray<qreal> extents(edges.count());
        for (int i = 0; i < edges.count(); ++i)
        {
            int n = (i + 1) % edges.count();
            extents[i] = (edges[n] - edges[i]).length();
            totalExtents += extents[i];
        }
        tx.startTileRight(totalExtents);
        for (int i = 0; i < edges.count(); ++i)
        {
            int n = (i + 1) % edges.count();
            addQuad(result[i], result[n], edges[n], edges[i],
                    tx.tileRight(extents[i]));
        }
    }
    else
    {
        for (int i = 0; i < edges.count(); ++i)
        {
            int n = (i + 1) % edges.count();
            addQuad(result[i], result[n], edges[n], edges[i]);
        }
    }
    QGLDisplayList::VectorArray temp;
    QGLDisplayList::VectorArray::const_iterator it = result.constEnd();
    for ( ; it != result.constBegin(); )
        temp.append(*--it);
    return temp;
}

/*!
    \internal
    Pads the arrays specified by \a which to allow for a new vertex which
    has been just added.
*/
void QGLDisplayList::fillArrays(QGLDisplayList::FillWhichSet which)
{
    if (m_hasNormals && (which & FillNormals))
    {
        if (m_normals.isEmpty())
            m_normals.fill(QVector3D(), m_vertices.size());
        else
            m_normals.append(QVector3D());
    }
    if (m_hasTexCoords && (which & FillTextures))
    {
        if (m_texCoords.isEmpty())
            m_texCoords.fill(QGLTextureSpecifier::InvalidTexCoord, m_vertices.size());
        else
            m_texCoords.append(QGLTextureSpecifier::InvalidTexCoord);
    }
    if (m_hasColors && (which & FillColors))
    {
        if (m_colors.isEmpty())
            m_colors.fill(QColor4b(), m_vertices.size());
        else
            m_colors.append(QColor4b());
    }
}

/*!
    Adds the vertex \a a to this display list, with the lighting normal \a n, and
    texture coordinate \a t.

    The vertex will be drawn as part of a smooth continuous surface, with
    no distinct edge.  To acheive this, duplicates of vertex \a a are
    coalesced into one vertex (within the current section).  This one vertex
    has a normal set to the average of supplied normals.

    Call this method to add the vertices of a smooth face to the display
    list, or preferably use:

    \code
    myDisplayList->openSection(QGLDisplayList::Smooth);
    myDisplayList->addTriangle(a, b, c);
    \endcode

    In smooth surfaces, the vertex and its normal is only sent to the
    graphics hardware once (not once per face), thus smooth geometry may
    consume fewer resources.

    \sa appendFaceted(), openSection()
*/
void QGLDisplayList::appendSmooth(const QVector3D &a, const QVector3D &n, const QVector2D &t)
{
    Q_D(QGLDisplayList);
    int v;
    if (!m_currentSection)
        newSection();
    QGLDisplayListPrivate::VecMap &dd = d->getMap();
    QGLDisplayListPrivate::VecMap::iterator it = dd.find(QVector3DMapped(&a));
    if (it == dd.end())
    {
        v = m_vertices.count();
        m_vertices.append(a);
        qDebug() << a << "not found, adding at" << v << "here";
        int nix = m_normals.count();
        m_normals.append(n);
        QVector3DMapped m(v, &m_vertices);
        m.appendNormal(nix, &m_normals, d->smoothedNormals);
        setTexCoord(v, t);
        dd.insert(m, v);
        fillArrays(FillColors);
    }
    else
    {
        v = it.value();
        qDebug() << a << "found at" << v;
        int ix = updateTexCoord(v, t);
        if (ix != -1)
            v = ix;
        if (ix != -1)
            qDebug() << "after updating tex coord" << t;
        QVector3DMapped &vm = const_cast<QVector3DMapped&>(it.key());
        if (!vm.findNormal(n, d->smoothedNormals))
        {
            for ( ; it != dd.end() && it.key() == a; ++it)
                m_normals[it.value()] += n;
            vm.appendNormal(v, &m_normals, d->smoothedNormals);
        }
    }
    m_indices.append(v);
    m_currentSection->m_count += 1;
    m_hasNormals = true;
    d->finalizeNeeded = true;
    d->loadNeeded = true;
}

/*!
    Add the vertex \a a to this display list, with the lighting normal \a n, and
    texture coordinate \a t.

    The vertex will be drawn as a distinct edge, instead of just part of a
    continuous smooth surface.  To acheive this a duplicate of \a a is
    added for each normal \a n (in the current section).

    Call this method to add the vertices of a faceted face to the display
    list, or preferably use:

    \code
    myDisplayList->openSection(QGLDisplayList::Faceted);
    myDisplayList->addTriangle(a, b, c);
    \endcode

    In faceted surfaces, the vertex is sent to the graphics hardware once for
    each normal it has, and thus may consume more resources.

    \sa appendSmooth()
*/
void QGLDisplayList::appendFaceted(const QVector3D &a, const QVector3D &n, const QVector2D &t)
{
    Q_D(QGLDisplayList);
    int v;
    if (!m_currentSection)
        newSection();
    QGLDisplayListPrivate::VecMap &dd = d->getMap();
    QGLDisplayListPrivate::VecMap::const_iterator it = dd.constFind(&a);
    for ( ; it != dd.constEnd() && it.key() == a; ++it)
        if (qFuzzyCompare(m_normals[it.value()], n))
            break;
    if (it != dd.constEnd() && it.key() == a) // found
    {
        v = it.value();
        int ix = updateTexCoord(v, t);
        if (ix != -1)
            v = ix;
    }
    else
    {
        v = m_vertices.count();
        m_vertices.append(a);
        m_normals.append(n);
        setTexCoord(v, t);
        fillArrays(FillColors);
        dd.insertMulti(&m_vertices[v], v);
    }
    m_indices.append(v);
    m_currentSection->m_count += 1;
    m_hasNormals = true;
    d->finalizeNeeded = true;
    d->loadNeeded = true;
}

/*!
    Add the vertex \a a to this display list, with the flat color \a c.
*/
void QGLDisplayList::appendColor(const QVector3D &a, const QColor4b &c, const QVector2D &t)
{
    Q_D(QGLDisplayList);
    int v;
    if (!m_currentSection)
        newSection();
    QGLDisplayListPrivate::VecMap &dd = d->getMap();
    QGLDisplayListPrivate::VecMap::const_iterator it = dd.constFind(&a);
    for ( ; it != dd.constEnd() && it.key() == a; ++it)
        if (m_colors[it.value()] == c)
            break;
    if (it != dd.constEnd() && it.key() == a) // found
    {
        v = it.value();
        int ix = updateTexCoord(v, t);
        if (ix != -1)
            v = ix;
    }
    else
    {
        v = m_vertices.count();
        m_vertices.append(a);
        m_colors.append(c);
        setTexCoord(v, t);
        fillArrays(FillNormals);
    }
    m_indices.append(v);
    m_currentSection->m_count += 1;
    m_hasColors = true;
    d->finalizeNeeded = true;
    d->loadNeeded = true;
}

/*!
    Add the vertex \a a to this display list.
*/
void QGLDisplayList::append(const QVector3D &a, const QVector2D &t)
{
    Q_D(QGLDisplayList);
    int v;
    if (!m_currentSection)
        newSection();
    QGLDisplayListPrivate::VecMap &dd = d->getMap();
    QGLDisplayListPrivate::VecMap::const_iterator it = dd.constFind(&a);
    if (it == dd.constEnd())
    {
        v = m_vertices.count();
        m_vertices.append(a);
        setTexCoord(v, t);
        fillArrays(FillColors | FillNormals);
        dd.insert(&m_vertices[v], v);
    }
    else
    {
        v = it.value();
        int ix = updateTexCoord(v, t);
        if (ix != -1)
            v = ix;
    }
    m_indices.append(v);
    m_currentSection->m_count += 1;
    m_hasNormals = true;
    d->finalizeNeeded = true;
    d->loadNeeded = true;
}

/*!
    Returns the position of the first vertex matching \a in the given \a section,
    or if \a section is null (the defaut) in the current section.  If no such
    matching vertex is found, returns -1.
*/
int QGLDisplayList::indexOf(const QVector3D &a, QGLSection *section) const
{
    Q_D(const QGLDisplayList);
    int v;
    if (!m_currentSection && !section)
        return -1;
    const QGLDisplayListPrivate::VecMap &dd = d->getMap(section);
    QGLDisplayListPrivate::VecMap::const_iterator it = dd.constFind(&a);
    if (it == dd.constEnd())
        v = -1;
    else
        v = it.value();
    return v;
}


/*!
    Sets the vertex at \a position to have coordinates \a v.

    The \a position must be a valid vertex, in other words, one that has
    already been added by one of the append functions.

    \sa appendSmooth(), appendFaceted()
*/
void QGLDisplayList::setVertex(int position, const QVector3D &v)
{
    Q_D(QGLDisplayList);
    m_vertices[position] = v;
    d->finalizeNeeded = true;
    d->loadNeeded = true;
}

/*!
    Sets the vertex normal at \a position to have value \a n.

    If \a n is a null vector, in other words an invalid normals, then this
    function does nothing.

    The \a position must be a valid vertex, in other words, one that has
    already been added by one of the append functions.

    \sa appendSmooth(), appendFaceted()
*/
void QGLDisplayList::setNormal(int position, const QVector3D &n)
{
    Q_D(QGLDisplayList);
    if (!n.isNull())
    {
        if (m_normals.isEmpty())
            m_normals.fill(QVector3D(), m_vertices.size());
        m_normals[position] = n;
        d->finalizeNeeded = true;
        d->loadNeeded = true;
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
inline void QGLDisplayList::setTexCoord(int position, const QVector2D &t)
{
    Q_D(QGLDisplayList);
    if (t != QGLTextureSpecifier::InvalidTexCoord)
    {
        if (m_texCoords.isEmpty())
            m_texCoords.fill(QGLTextureSpecifier::InvalidTexCoord, m_vertices.size());
        m_texCoords[position] = t;
        m_hasTexCoords = true;
        d->finalizeNeeded = true;
        d->loadNeeded = true;
    }
}

/*!
    Sets the vertex color at \a position to have value \a c.

    The \a position must be a valid vertex, in other words, one that has
    already been added by one of the append functions.

    \sa appendColor()
*/
void QGLDisplayList::setColor(int position, const QColor4b &c)
{
    Q_D(QGLDisplayList);
    if (m_colors.isEmpty())
        m_colors.fill(QColor4b(), m_vertices.size());
    m_colors[position] = c;
    m_hasColors = true;
    d->finalizeNeeded = true;
    d->loadNeeded = true;
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
int QGLDisplayList::updateTexCoord(int index, const QVector2D &t)
{
    Q_D(QGLDisplayList);
    int v = -1;
    if (t != QGLTextureSpecifier::InvalidTexCoord  && t != m_texCoords[index])
    {
        if (m_texCoords[index] == QGLTextureSpecifier::InvalidTexCoord)
        {
            v = index;
            m_texCoords[index] = t;
        }
        else
        {
            v = m_vertices.count();
            m_vertices.append(m_vertices[index]);
            if (m_hasNormals)
                m_normals.append(m_normals[index]);
            if (m_hasColors)
                m_colors.append(m_colors[index]);
            m_texCoords.append(t);
            d->getMap().insert(&m_vertices[index], v);
        }
        m_hasTexCoords = true;
        d->finalizeNeeded = true;
        d->loadNeeded = true;
    }
    return v;
}

/*!
    Finish the building of this display list and optimize it for
    rendering.  This method must be called once after building the
    scene, or after modifying the geometry.

    This function may be expensive.
*/
void QGLDisplayList::finalize()
{
    Q_D(QGLDisplayList);
    if (d->finalizeNeeded)
    {
        for (int i = 0; i < m_normals.count(); ++i)
            m_normals[i].normalize();
        QBox3D bb;
        for (int i = 0; i < m_vertices.count(); ++i)
            bb.expand(m_vertices.at(i));
        setBoundingBox(bb);
        d->finalizeNeeded = false;
    }
}

/*!
    Registers the display lists geometry with the \a painter, and loads
    any vertex buffer objects ready for efficient display.

    If a new context has been switched in, or a different display list
    has been rendering to the current context, call this function to switch
    back to rendering this display list on the current context.

    If the geometry of this display list has already been regsitered with
    this painter's context, then this function does nothing.
*/
void QGLDisplayList::loadArrays(QGLPainter *painter)
{
    Q_D(QGLDisplayList);
    Q_ASSERT(painter);
    finalize();
    if (d->loadNeeded || painter->context() != d->context)
    {
        setVertexArray(toVertexArray());
        setIndexArray(QGLIndexArray::fromRawData(m_indices.constData(),
                                                 m_indices.count()));
        d->context = painter->context();
        d->loadNeeded = false;
    }
    upload();
}

/*!
    Creates a new QGLSection with smoothing mode set to \a smooth and makes
    it current on this QGLDisplayList.

    Returns the new current active QGLSection.

    \sa currentSection()
*/
QGLSection *QGLDisplayList::newSection(QGL::Smoothing s)
{
    addSection(new QGLSection(this, s));
    return m_currentSection;
}

void QGLDisplayList::addSection(QGLSection *sec)
{
    Q_D(QGLDisplayList);
    m_currentSection = sec;
    d->dedupMap.insert(sec, QGLDisplayListPrivate::VecMap());
    m_sections.append(sec);
}

/*!
    \fn QList<QGLSection*> &QGLDisplayList::sections()
    Returns a list of the sectiones of the geometry in this display list
    as a QList of QGLSection instances.
*/

/*!
    \fn QGLDisplayList::VectorArray QGLDisplayList::vertices() const
    Returns a list of all the vertices in this display list.
*/

/*!
    \fn QGLDisplayList::VectorArray QGLDisplayList::normals() const
    Returns a list of all the normals in this display list.
*/

/*!
    \fn QGLDisplayList::TexCoordArray QGLDisplayList::texCoords() const
    Returns a list of all the texture coordinates in this display list.
*/

/*!
    \fn QGLDisplayList::IndexArray QGLDisplayList::indices() const
    Returns a list of all the face indexes in this display list.
*/

/*!
    \fn QGLMaterialCollection *QGLDisplayList::materials() const
    Returns the QGLMaterialCollection object referenced by this display
    list.
*/

/*!
    \internal
    Temporary hack to use QGLVertexArray as an intermediary between
    display list and painter.
*/
QGLVertexArray QGLDisplayList::toVertexArray() const
{
    QGLVertexArray array;
    if (!m_hasNormals && !m_hasColors && !m_hasTexCoords)
    {
        array.setRawData(reinterpret_cast<const float *>(m_vertices.constData()),
                         m_vertices.count());
        return array;
    }
    array.addField(QGL::Position, 3);
    if (m_hasNormals)
        array.addField(QGL::Normal, 3);
    if (m_hasTexCoords)
        array.addField(QGL::TextureCoord0, 2);
    if (m_hasColors)
        array.addField(QGL::Color, 1);
    for (int i = 0; i < m_vertices.count(); ++i)
    {
        array.append(m_vertices.at(i));
        if (m_hasNormals)
            array.append(m_normals.at(i));
        if (m_hasTexCoords)
            array.append(m_texCoords.at(i));
        if (m_hasColors)
            array.append(m_colors.at(i));
    }
    return array;
}

/*!
    \fn QGLDisplayList::IndexArray QGLDisplayList::indexArray() const
    Returns the indices of this display list.  The result type is implictly
    shared so although it is a copy this method is inexpensive.  If the
    copy has a non-const method called on it then a copy is done (which may
    be expensive).
*/

/*!
    \fn bool QGLDisplayList::hasColors() const
    Returns true if this display list is utilizing flat coloring, and
    false otherwise.  If the appendColor() method has been called then
    this function will return true.
*/

/*!
    \fn bool QGLDisplayList::hasNormals() const
    Returns true if this display list is utilizing lighting normals, and
    false otherwise.  After calling one of the append methods which takes
    a normal, such as appendSmooth() or appendFaceted() this function
    will return true.
*/

/*!
    \fn bool QGLDisplayList::hasTexCoords() const
    Returns true if this display list is utilizing texture coordinates,
    and false otherwise.  After calling appendTexCoord(), this function
    will return true.
*/
