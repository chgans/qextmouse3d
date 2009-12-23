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
#include "qgldisplaylist_p.h"
#include "qglsection_p.h"
#include "qglmaterialcollection.h"
#include "qglpainter.h"
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

    Traditional OpenGL programming uses
    \l{http://www.opengl.org/documentation/specs/version1.1/glspec1.1/node123.html}{display lists}
    to build geometry once during an initialization phase, and then efficiently
    display that geometry each frame of rendering.

    While the underlying implementation of QGLDisplayList does not use the same
    OpenGL calls (since those calls are not implemented on all platforms) it
    provides convenience and improved performance, and can be utilized in
    the same paradigm as the OpenGL display list with an initial setup phase
    and subsequent cheap drawing operations.

    \section1 Display Lists and Scene Nodes

    QGLSceneNodes are used to manage application of local transformations,
    materials and effects, in the same way that glRotate() or glMaterial()
    might be used inside a display list.

    Since QGLDisplayList is itself a (sub-class of) QGLSceneNode materials
    and effects may be applied to the whole list, or to parts of it.  This
    is demonstrated in the displaylist example application.

    \image soup.png

    Here the front can is a display list and the other two are scene nodes
    that simply reference it, without copying any geometry.

    \snippet displaylist/displaylist.cpp 0

    QGLSceneNodes can be used after the display list is created to cheaply
    copy and redisplay the whole list.  Or to reference parts of the list
    use the functions newNode() or pushNode() and popNode() to manage
    QGLSceneNode generation inside a display list.

    To draw a display list, simply call its draw function as for any
    QGLSceneNode:

    \snippet displaylist/displaylist.cpp 1

    Call the \l{QGLGeometry::palette()}{palette()} function on the scene node's
    geometry to get the QGLMaterialCollection for the node, and record textures
    and materials into it.  (Typically a display lists nodes, and usually the
    whole application will share the one palette).

    \snippet displaylist/displaylist.cpp 2

    These may then be applied as needed throughout the building of the
    geometry using the integer reference, \c{canMat} in the above code.

    \section1 Using Sections

    During initialization of the QGLDisplayList, while accumulating
    geometry, the geometry data in a QGLDisplayList is placed into
    sections - there must be at least one section.

    Call the newSection() function to create a new section, before putting
    any geometry into a display list.

    \snippet displaylist/displaylist.cpp 3

    Here seperate sections for the rounded outside cylinder and flat top and
    bottom of the soup can model makes for the appearance of a sharp edge
    between them.  If the sides and top and bottom were in the same section
    QGLDisplayList would attempt to average the normals around the edge resulting
    in an unrealistic effect.

    In 3D applications this concept is referred to as
    \l{http://www.google.com/search?smoothing+groups}{smoothing groups}.  Within
    a section (smoothing group) all normals are averaged making it appear
    as one smoothly shaded surface.

    The can has 3 smoothing groups - bottom, top and sides.

    This mesh of a Q is a faceted model - it has 0 smoothing groups:

    \image faceted-q.png

    To create geometry with a faceted appearance call newSection() with
    an argument of QGL::Faceted thus \c{newSection(QGL::Faceted)}.

    Faceted geometry is suitable for small models, where hard edges are
    desired between every face - a dice, gem or geometric solid for example.

    \section2 Geometry Data in a Section

    Management of normals and vertices for smoothing, and other data is
    handled automatically by the display list.

    Within a section, incoming geometry data will be coalesced and
    indices created to reference the fewest possible copies of the vertex
    data.  For example, in smooth geometry all copies of a vertex are
    coalesced into one, and referenced by indices.

    One of the few exceptions to this is the case where texture data forms
    a \i seam and a copy of a vertex must be created to carry the two
    texture coordinates either side of the seam.

    \image texture-seam.png

    Coalescing has the effect of packing geometry data into the
    smallest space possible thus improving cache coherence and performance.

    Again all this is managed automatically by QGLDisplayList and all
    that is required is to create smooth or faceted sections, and add
    geometry to them.

    Each QGLSection references a contiguous range of vertices in a
    QGLDisplayList.

    \section1 Finalizing a QGLDisplayList

    Once the geometry has been accumulated in the display list,  the
    finalize() method must be called to normalize the geometry and optimize
    it for display.  The finalize() method makes passes through the data of
    each section, normalizing and then optimizing and preparing the data for
    display.  Thus it may be expensive for large geometry.

    The finalize method also destroys all the internal vertex management
    data structures, with the result that no more geometry may be added to
    the display list.  Once finalize() has finished its work, the geometry
    data in a display list is acessible as a QGLVertexArray:

    \code
    displayList->finalize();
    QGLVertexArray data = displayList->geometry()->vertexArray();
    QVector3D vec = data.vector3DAt(5);
    \endcode

    The finalize() function only needs to be called once in the application
    lifetime, and since QGLDisplayList reimplements QGLSceneNode's draw
    function to simply call finalize() and then QGLSceneNode::draw(), you do
    not need to remember to explicitly call it, unless you want to control
    when the overhead of the finalize() function is incurred.
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
    \fn void QGLDisplayList::draw(QGLPainter *painter)
    Draws the display list on the given \a painter.
*/

/*!
    Add to the current section a triangular face with vertices \a a, \a b,
    and \a c; and lighting normal \a n.

    If \a textureModel is non-null then the following texture coordinates will
    be applied to the vertices:
    \list
        \o \a a - \l{QGLTextureModel::bottomLeft()}{bottom-left}
        \o \a b - \l{QGLTextureModel::bottomRight()}{bottom-right}
        \o \a c - \l{QGLTextureModel::topRight()}{top-right}
    \endlist
    which is the case for a triangle in an upright orientation with respect to
    texture-space, with its apex \a c at the high values of the texture and its
    base \a a - \a c at the low values of the texture.

    \bold{Note:} The \a inverted parameter is intended mostly for internal use.

    If \a inverted is true, the following texture coordinates
    will be applied:
    \list
        \o \a a - \l{QGLTextureModel::bottomLeft()}{bottom-left}
        \o \a b - \l{QGLTextureModel::topRight()}{top-right}
        \o \a c - \l{QGLTextureModel::topLeft()}{top-left}
    \endlist

    The inverted case is a triangle with the vertex at \a a on the low values
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
        norm = QVector3D::crossProduct(b - a, c - a);
    if (norm.isNull())
    {
#ifndef QT_NO_DEBUG
        qDebug() << "#### Triangle" << a << b << c << "has null normal ####";
#endif
        norm = QVector3D(1.0f, 0.0f, 0.0f);
    }
    QLogicalVertex va(a, norm);
    QLogicalVertex vb(b, norm);
    QLogicalVertex vc(c, norm);
    if (!textureModel.isNull())
    {
        va.setTexCoord(textureModel.bottomLeft());
        vb.setTexCoord(inverted ? textureModel.topRight() : textureModel.bottomRight());
        vc.setTexCoord(inverted ? textureModel.topLeft() : textureModel.topRight());
    }
    d->currentSection->append(va);
    d->currentSection->append(vb);
    d->currentSection->append(vc);
    d->currentNode->setCount(d->currentNode->count() + 3);
}

/*!
    Add to this section a quad face defined by vertices \a a, \a b, \a c and
    \a d.  The face is composed of two triangles having the same normal,
    calculated from the face \a a, \a b and \a c.  The vertices of the face
    \a a, \a b, \a c and \a d must all lie in the same plane.

    If \a textureModel is non-null then the following texture coordinates will
    be applied to the vertices:
    \list
        \o \a a - \l{QGLTextureModel::bottomLeft()}{bottom-left}
        \o \a b - \l{QGLTextureModel::bottomRight()}{bottom-right}
        \o \a c - \l{QGLTextureModel::topRight()}{top-right}
        \o \a d - \l{QGLTextureModel::topLeft()}{top-left}
    \endlist
*/
void QGLDisplayList::addQuad(const QVector3D &a, const QVector3D &b,
                             const QVector3D &c, const QVector3D &d,
                             const QGLTextureModel &textureModel)
{
    QVector3D norm = QVector3D::crossProduct(b - a, c - a);
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

    \sa addTriangulatedFace()
*/
void QGLDisplayList::addTriangleFan(const QVector3D &center,
                                    const QGL::VectorArray &edges,
                                    const QGLTextureModel &textureModel)
{
    Q_ASSERT(edges.count() > 1);
    if (!textureModel.isNull())
        qWarning("NOT IMPLEMENTED YET");
    for (int i = 0; i < edges.count() - 1; ++i)
        addTriangle(edges[i], edges[(i + 1)], center, QVector3D(), textureModel);
}

/*!
    Adds to this section a face defined by \a center and \a edges.  The face
    is composed of N triangular sub-faces, where N == edges.count(), each
    formed by taking two vertices from the \a edges, eg edges[0] and edges[1]
    and then \a center.

    One normal is calculated for the face, since a faces vertices lie in
    the same plane.  The \a center and \a edges must all lie in the same
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

    The quads and their normals are generated in such a way that if \a edges
    are the perimeter vertices of a polygon F, in counter-clockwise order, and
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
    QGL::VectorArray front;
    front << a << b << c << d;
    QGL::VectorArray obverse = displayList->extrude(front);
    // obverse now contains d', c', b' and a' in that order
    displayList->addQuad(obverse[0], obverse[1], obverse[2], obverse[3]);
    \endcode
    Note that the vertices in the returned array are reversed in sequence so
    that the obverse face has the correct winding to face outwards from an
    extruded solid.

    Textures are generated according to \a textureModel, if textureModel
    is non-null.  When textures are generated all the points on \a edges
    must lie in the same plane, which is the case anyway if they are the
    perimeter vertices of a polygon.  Textures are assigned with the high Y
    values on the \a edges; low Y values on the extruded result edge;
    low X values at the left of \c{edges[0]} and high X values on the
    right of \c{edges.last()}.
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

    This function does the following:
    \list
        \o packs all geometry data from sections into QGLGeomtry instances
        \o references this data via QGLSceneNode start() and count()
        \o uploads the data to the graphics hardware, if possible
        \o deletes all QGLSection instances in this list
    \endlist

    This function may be expensive.  This function is called by the
    default implementation of draw, so you don't have to remember to call
    it, unless you explicitly want to control when the finalize cost is
    incurred.

    Finalize will exit quickly without doing anything if no modifications
    have been made to any data since the last time finalize was called.
*/
void QGLDisplayList::finalize()
{
    Q_D(QGLDisplayList);
    if (d->finalizeNeeded)
    {
        QGLGeometry *g = 0;
        QMap<QLogicalVertex::Types, QGLGeometry *> geos;
        while (d->sections.count())
        {
            // pack sections that have the same types into one geometry
            QGLSection *s = d->sections.takeFirst();
            s->finalize();

            QGL::IndexArray indices = s->indices();
            const int *vi = indices.constData();
            int vcnt = indices.count();
            int sectionOffset = 0;
            int sectionIndexOffset = 0;
            QMap<QLogicalVertex::Types, QGLGeometry *>::const_iterator it =
                    geos.constFind(s->dataTypes());
            if (it != geos.constEnd())
            {
                g = it.value();
                QGLVertexArray va = g->vertexArray();
                sectionOffset = va.vertexCount();
                va.append(s->toVertexArray());
                QGLIndexArray ia = g->indexArray();
                sectionIndexOffset = ia.size();
                for (int i = 0; i < vcnt; ++i)
                    ia.append(vi[i] + sectionOffset);
                g->setVertexArray(va);
                g->setIndexArray(ia);
            }
            else
            {
                if (g == 0)
                    g = geometry();
                else
                    g = new QGLGeometry(this);
                g->setVertexArray(s->toVertexArray());
                QGLIndexArray iry;
                iry.reserve(vcnt);
                for (int i = 0; i < vcnt; ++i)
                    iry.append(vi[i]);
                g->setIndexArray(iry);
                g->setDrawingMode(QGL::Triangles);
                g->setPalette(geometry()->palette());
                geos.insert(s->dataTypes(), g);
            }
            QMap<QGLSection *, QGLSceneNode *>::const_iterator nit =
                    d->sectionNodeMap.constFind(s);
            for ( ; nit != d->sectionNodeMap.constEnd() && nit.key() == s; ++nit)
            {
                QGLSceneNode *node = nit.value();
                node->setStart(node->start() + sectionIndexOffset);
                node->setGeometry(g);
            }
            g->setBoundingBox(g->boundingBox().expanded(s->boundingBox()));
            delete s;
        }
        d->finalizeNeeded = false;
    }
}

/*!
    Creates a new section with smoothing mode set to \a smooth and makes
    it current on this QGLDisplayList.

    Also a new QGLSceneNode is created and made current.  The new node is a
    copy of the previously current node, so any materials or effects set
    will apply to this new section.
*/
void QGLDisplayList::newSection(QGL::Smoothing smooth)
{
    new QGLSection(this, smooth);  // calls addSection
}

void QGLDisplayList::addSection(QGLSection *sec)
{
    Q_D(QGLDisplayList);
    if (d->currentNode && d->currentSection)
        d->currentNode->setCount(d->currentSection->count() - d->currentNode->start());
    if (d->currentSection)
        d->currentSection->finalize();
    d->currentSection = sec;
    d->sections.append(sec);
    newNode();
}

/*!
    \internal
    Returns the current section, in which new geometry is being added.
*/
QGLSection *QGLDisplayList::currentSection() const
{
    Q_D(const QGLDisplayList);
    return d->currentSection;
}

/*!
    \internal
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
    returned.

    The node is set to reference the geometry starting from the next
    vertex created, such that QGLSceneNode::start() will return the
    index of this next vertex.  QGLSceneNode::count() will return 0.

    Any previous node that was current is finalized at the last vertex added
    and QGLSceneNode::count() will correctly return the index of this last
    vertex.

    newSection()
*/
QGLSceneNode *QGLDisplayList::newNode()
{
    Q_D(QGLDisplayList);
    QGLSceneNode *parentNode = this;
    if (d->nodeStack.count() > 0)
        parentNode = d->nodeStack.last();
    int sectionCount = 0;
    if (d->currentSection)
        sectionCount = d->currentSection->count();
    if (d->currentNode)
    {
        if (sectionCount)
            d->currentNode->setCount(sectionCount - d->currentNode->start());
        if (d->currentNode->count() == 0)
        {
            QMap<QGLSection*, QGLSceneNode*>::iterator it = d->sectionNodeMap.begin();
            for ( ; it != d->sectionNodeMap.end(); ++it)
            {
                if (it.value() == d->currentNode)
                {
                    d->sectionNodeMap.erase(it);
                    break;
                }
            }
            Q_ASSERT(it != d->sectionNodeMap.end());  // must be here somewhere
            delete d->currentNode;
        }
    }
    d->currentNode = new QGLSceneNode(parentNode);
    d->currentNode->setStart(sectionCount);
    d->sectionNodeMap.insertMulti(d->currentSection, d->currentNode);
    return d->currentNode;
}

/*!
    Returns a pointer to the current scene node, if one exists; otherwise
    returns null.

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
    The previous current node is saved on a stack and its settings may
    be made current again by calling popNode().

    As a child of the current node, the new node will be affected by any
    transformations and effects or materials on its parent.  The new child
    has no current effects or materials set on itself.

    \sa popNode(), newNode()
*/
QGLSceneNode *QGLDisplayList::pushNode()
{
    Q_D(QGLDisplayList);
    d->nodeStack.append(d->currentNode);
    d->currentNode->setCount(d->currentSection->count() - d->currentNode->start());
    d->currentNode = new QGLSceneNode(d->currentNode);
    d->currentNode->setStart(d->currentSection->count());
    d->sectionNodeMap.insertMulti(d->currentSection, d->currentNode);
    return d->currentNode;
}

/*!
    Removes the node from the top of the stack, makes a copy of it, and makes the
    copy current.

    If the stack is empty, behaviour is undefined.  In debug mode, calling
    this function when the stack is empty will cause an assert.

    A pointer to the new current node is returned.

    The node is set to reference the geometry starting from the next
    vertex created, such that QGLSceneNode::start() will return the
    index of this next vertex.  QGLSceneNode::count() will return 0.

    Any previous node that was current is finalized at the last vertex added
    such that QGLSceneNode::count() on that node will correctly return the
    index of the last vertex added.
*/
QGLSceneNode *QGLDisplayList::popNode()
{
    Q_D(QGLDisplayList);
    int cnt = d->currentSection->count();
    d->currentNode->setCount(cnt - d->currentNode->start());
    QGLSceneNode *s = d->nodeStack.takeLast();
    QGLSceneNode *parentNode = this;
    if (d->nodeStack.count() > 0)
        parentNode = d->nodeStack.last();
    d->currentNode = s->clone(parentNode);
    d->currentNode->setStart(cnt);
    d->currentNode->setCount(0);
    d->sectionNodeMap.insertMulti(d->currentSection, d->currentNode);
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
