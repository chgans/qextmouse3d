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
#include "qglsection_p.h"
#include "qgltexturemodel.h"

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

    QGLSceneNodes are used to manage application of local transformations,
    materials and effects, in the same way that glRotate() or glMaterial()
    might be used inside a display list.

    Since QGLDisplayList is itself a (sub-class of) QGLSceneNode materials
    and effects may be applied to the whole list, or to parts of it.

    \snippet displaylist/displaylist.cpp 0

    Use the functions newNode() or pushNode() and popNode() to manage
    QGLSceneNode generation inside a display list.

    To draw a display list, simply call its draw function as for any
    QGLSceneNode:

    \snippet displaylist/displaylist.cpp 1

    Call the \l{QGLGeometry::palette()}{palette()} function on the scene node's
    geometry to get the QGLMaterialCollection for the node, and record textures
    and materials into it.

    \snippet displaylist/displaylist.cpp 2

    These may then be applied as needed throughout the building of the
    geometry using the integer reference (see the next snippet).

    The geometry data in a QGLDisplayList is divided into QGLSection
    instances which manage the smoothing properties of the mesh. For
    example a QGLDisplayList representing a 3D can of soup might have a
    smooth QGLSection for the rounded outside cylinder and seperate
    QGLSection instances for the flat top and bottom.  This will cause
    the can to have lighting normals such that a sharp edge seperates
    the top, bottom and sides whilst retaining the smoothly rounded sides.

    \snippet displaylist/displaylist.cpp 3

    Smooth surfaces are continuous over vertices, where faceted surfaces have
    hard edges at the vertices.  In reality both surfaces are made of faces,
    but in the smooth case lighting normals are calculated to make for a smooth
    appearance.  In 3D applications this concept is often referred to as
    \l{http://www.google.com/search?smoothing+groups}{smoothing groups}.

    Once the geometry has been accumulated in the display list, call
    the finalize() method to complete the geometry and optimize it for
    display.  Finalize makes several passes through the data, optimizing it
    and preparing it for display, and thus it may be expensive for large
    geometry.  Generally finalize() only needs to be called once
    in the application lifetime.

    QGLDisplayList reimplements QGLSceneNode's draw function to simply call
    finalize() and then QGLSceneNode::draw(), but finalize() may be called
    explicitly to control when the overhead of the finalize() function is
    incurred.

    \sa QGLSection
*/

QGLDisplayListPrivate::QGLDisplayListPrivate(int version)
    : QGLSceneNodePrivate(QGLSceneObject::Main, version)
    , finalizeNeeded(true)
    , currentSection(0)
    , currentNode(0)
{
}

QGLDisplayListPrivate::~QGLDisplayListPrivate()
{
    qDeleteAll(sections);
}

/*!
    Construct a new QGLDisplayList with \a materials and \a parent.  If the
    \a materials argument is null, then a new collection is created internally.
*/
QGLDisplayList::QGLDisplayList(QObject *parent, QGLMaterialCollection *materials)
    : QGLSceneNode(*new QGLDisplayListPrivate, parent)
{
    setGeometry(new QGLGeometry(this));
    if (!materials)
        materials = new QGLMaterialCollection(this);
    geometry()->setPalette(materials);
}

/*!
    Destroys this QGLDisplayList recovering any resources.
*/
QGLDisplayList::~QGLDisplayList()
{
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
                      const QGLTextureModel &textureModel, bool inverted)
{
    Q_D(QGLDisplayList);
    Q_ASSERT(d->currentSection);
    Q_ASSERT(d->currentNode);
    QVector3D norm = n;
    if (norm.isNull())
        norm = QVector3D::normal(a, b, c);
    qDebug() << "Adding triangle:" << a << b << c << " - normal:"
            << n << "tex:" << textureModel.topLeft() << "-" << textureModel.bottomRight();
    QLogicalVertex va(a, norm, textureModel.bottomLeft());
    QLogicalVertex vb(b, norm, inverted ? textureModel.topRight() : textureModel.bottomRight());
    QLogicalVertex vc(c, norm, inverted ? textureModel.topLeft() : textureModel.topRight());
    d->currentSection->append(va);
    d->currentSection->append(vb);
    d->currentSection->append(vc);
    d->currentNode->setCount(d->currentNode->count() + 3);
}

/*!
    Add to this section a quad face defined by vertices \a a, \a b, \a c and
    \a d.  The face is composed of two triangles having the same normal,
    calculated from the face \a a, \a b and \a c.
*/
void QGLDisplayList::addQuad(const QVector3D &a, const QVector3D &b,
                             const QVector3D &c, const QVector3D &d,
                             const QGLTextureModel &textureModel)
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
                                    const QGL::VectorArray &edges,
                                    const QGLTextureModel &textureModel)
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
                                         const QGL::VectorArray &edges,
                                         const QGLTextureModel &textureModel)
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
QGL::VectorArray QGLDisplayList::extrude(const QGL::VectorArray &edges,
                                         const QVector3D &offset,
                                         const QGLTextureModel &textureModel)
{
    // cannot extrude just a point, need at least 2 points
    Q_ASSERT(edges.count() > 1);
    QVector3D o = offset;
    QGL::VectorArray result;
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
        // calculate texturecoords
        QGLTextureModel tx(textureModel);
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
    QGL::VectorArray temp;
    QGL::VectorArray::const_iterator it = result.constEnd();
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
        QGLGeometry *g = 0;
        QMap<QLogicalVertex::Types, QGLGeometry *> geos;
        for (int i = 0; i < d->sections.count(); ++i)
        {
            // pack sections that have the same types into one geometry
            QGLSection &s = *d->sections[i];
            QGL::IndexArray &indexData = s.d->data->indices;
            s.finalize();
            int sectionOffset = 0;
            QMap<QLogicalVertex::Types, QGLGeometry *>::const_iterator it =
                    geos.constFind(s.dataTypes());
            if (it != geos.constEnd())
            {
                g = it.value();
                QGLVertexArray va = g->vertexArray();
                sectionOffset = va.vertexCount();
                va.append(s.d->data->toVertexArray());
                QGLIndexArray ia = g->indexArray();
                for (int i = 0; i < indexData.size(); ++i)
                    ia.append(indexData[i] + sectionOffset);
                g->setVertexArray(va);
                g->setIndexArray(ia);
            }
            else
            {
                if (g == 0)
                    g = geometry();
                else
                    g = new QGLGeometry(this);
                g->setVertexArray(s.d->data->toVertexArray());
                g->setIndexArray(QGLIndexArray::fromRawData(indexData.constData(),
                                                            indexData.size()));
                g->setDrawingMode(QGL::Triangles);
                geos.insert(s.dataTypes(), g);
            }
            QMap<QGLSection *, QGLSceneNode *>::const_iterator nit =
                    d->sectionNodeMap.constFind(&s);
            while (nit != d->sectionNodeMap.constEnd() && nit.key() == &s)
            {
                QGLSceneNode *node = nit.value();
                node->setStart(node->start() + sectionOffset);
                node->setGeometry(g);
            }
            g->setBoundingBox(g->boundingBox().expanded(s.boundingBox()));
        }
        d->finalizeNeeded = false;
    }
}

/*!
    Creates a new QGLSection with smoothing mode set to \a smooth and makes
    it current on this QGLDisplayList.

    Returns the new current active QGLSection.

    Also a new QGLSceneNode is created and made current.  The new node is a
    copy of the previously current node, so any materials or effects set
    will apply to this new section.

    \sa currentSection()
*/
QGLSection *QGLDisplayList::newSection(QGL::Smoothing s)
{
    Q_D(QGLDisplayList);
    addSection(new QGLSection(this, s));
    return d->currentSection;
}

void QGLDisplayList::addSection(QGLSection *sec)
{
    Q_D(QGLDisplayList);
    d->currentSection = sec;
    d->sections.append(sec);
    newNode();
}

/*!
    Returns the current section, in which new geometry is being added.
*/
QGLSection *QGLDisplayList::currentSection() const
{
    Q_D(const QGLDisplayList);
    return d->currentSection;
}

/*!
    \fn QList<QGLSection*> &QGLDisplayList::sections()
    Returns a list of the sections of the geometry in this display list.
*/
QList<QGLSection*> QGLDisplayList::sections() const
{
    Q_D(const QGLDisplayList);
    return d->sections;
}

/*!
    Creates a new QGLSceneNode within the current section of this
    display list, and makes it current.  A pointer to the new node is
    returned.  Any current node is copied to make this new node so
    any transformations, materials or effects previously set will apply
    to this new node.
*/
QGLSceneNode *QGLDisplayList::newNode()
{
    Q_D(QGLDisplayList);
    QGLSceneNode *parentNode = this;
    if (d->nodeStack.count() > 0)
        parentNode = d->nodeStack.last();
    if (d->currentNode)
        d->currentNode = d->currentNode->clone(parentNode);
    else
        d->currentNode = new QGLSceneNode(parentNode);
    d->currentNode->setStart(d->currentSection->count());
    d->currentNode->setCount(0);
    d->sectionNodeMap.insertMulti(d->currentSection, d->currentNode);
    return d->currentNode;
}

/*!
    Returns a pointer to the current scene node.

    \sa newNode(), newSection()
*/
QGLSceneNode *QGLDisplayList::currentNode()
{
    Q_D(QGLDisplayList);
    return d->currentNode;
}

/*!
    Creates a new scene node that is a child of the current node and,
    makes it the current node.  A pointer to the new node is returned.
    The previous current node is saved on a stack and may be made
    current again by calling popNode().

    As a child of the current node, the new node will be affected by any
    transformations and effects or materials on its parent.  The new child
    has no current effects or materials set on itself.

    \sa popNode(), newNode()
*/
QGLSceneNode *QGLDisplayList::pushNode()
{
    Q_D(QGLDisplayList);
    d->nodeStack.append(d->currentNode);
    d->currentNode = new QGLSceneNode(d->currentNode);
    d->sectionNodeMap.insertMulti(d->currentSection, d->currentNode);
    return d->currentNode;
}

/*!
    Makes the node on the top of the stack current again.  If the stack
    is empty, behaviour is undefined.  In debug mode, calling this function
    when the stack is empty will cause an assert.
*/
QGLSceneNode *QGLDisplayList::popNode()
{
    Q_D(QGLDisplayList);
    d->currentNode = d->nodeStack.takeLast();
    return d->currentNode;
}

/*!
    \internal
    Mark the data as dirty and in need of loading/finalizing.
*/
void QGLDisplayList::setDirty(bool dirty)
{
    Q_D(QGLDisplayList);
    d->finalizeNeeded = dirty;
}
