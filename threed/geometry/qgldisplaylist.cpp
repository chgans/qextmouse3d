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
#include "qgldisplaylist_p.h"

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
    Construct a new QGLDisplayList with \a materials and \a parent.  If the
    \a materials argument is null, then a new collection is created internally.
*/
QGLDisplayList::QGLDisplayList(QObject *parent, QGLMaterialCollection *materials)
    : QGLGeometry(*new QGLDisplayListPrivate, parent)
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
    qDebug() << "Adding triangle:" << a << b << c << " - normal:"
            << n << "tex:" << textureModel.topLeft() << "-" << textureModel.bottomRight();
    m_currentSection->append(a, norm, textureModel.bottomLeft());
    m_currentSection->append(b, norm, inverted
                             ? textureModel.topRight() : textureModel.bottomRight());
    m_currentSection->append(c, norm, inverted
                             ? textureModel.topLeft() : textureModel.topRight());
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
                                         const QGeometryData::VectorArray &edges,
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
        d->fillArrays(QGLDisplayListPrivate::FillColors |
                      QGLDisplayListPrivate::FillNormals |
                      QGLDisplayListPrivate::FillTextures);
        setBoundingBox(bb);
        m_vertices.squeeze();
        m_normals.squeeze();
        m_texCoords.squeeze();
        m_colors.squeeze();
        m_indices.squeeze();
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
    m_sections.append(sec);
}

/*!
    \fn QList<QGLSection*> &QGLDisplayList::sections()
    Returns a list of the sectiones of the geometry in this display list
    as a QList of QGLSection instances.
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
    \internal
    Mark the data as dirty and in need of loading/finalizing.
*/
void QGLDisplayList::setDirty(bool dirty)
{
    Q_D(QGLDisplayList);
    d->loadNeeded = dirty;
    d->finalizeNeeded = dirty;
}
