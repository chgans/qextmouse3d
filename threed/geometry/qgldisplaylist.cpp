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
#include "qglprimitive.h"
#include "qglindexarray.h"

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

    QGLDisplayList contains functions which provide similar functionality to
    OpenGL's GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_QUADS and so on.  There is no
    support for lines or points - only primitives that can be built from
    triangles.

    It is an important difference that QGLDisplayList builds quads, and all
    other structures out of triangles, since many systems do not support
    quads natively.

    Also, with regard to the ordering and winding of vertices, QGLDisplayList
    always starts at the origin, with (0, 0) being the bottom-left, since it
    is the bottom-left of the positive quadrant.

    The order follows with (1, 0) bottom-right, (1, 1) top-right and
    lastly (1, 0) top-left - this gives an anti-clockwise wound face,
    as required by 3D graphics systems.  See the addQuadStrip() documentation
    for details on how this makes vertex ordering different from GL_QUAD_STRIP.

    So, while remembering that QGLDisplayList does not attempt to exactly
    replicate OpenGL, these are QGLDisplayLists geometry functions and the
    OpenGL mode/function they most resemble:

    \table
        \header
            \o QGLDisplayList function
            \o Similar to OpenGL mode/function
        \row
            \o begin()
            \o glBegin()
        \row
            \o end()
            \o glEnd()
        \row
            \o addVertex()
            \o glVertex()
        \row
            \o addNormal()
            \o glNormal()
        \row
            \o addColor()
            \o glColor()
        \row
            \o addTexCoord()
            \o glTexCoord()
        \row
            \o addAttribute()
            \o glVertexAttrib()
        \row
            \o addTriangle()
            \o GL_TRIANGLES
        \row
            \o addQuad()
            \o GL_QUADS
        \row
            \o addTriangleFan()
            \o GL_TRIANGLE_FAN
        \row
            \o addTriangulatedFace()
            \o GL_POLYGON
        \row
            \o addTriangleStrip()
            \o GL_TRIANGLE_STRIP
        \row
            \o addQuadStrip(), extrude()
            \o GL_QUAD_STRIP
        \row
            \o finalize()
            \o glEndList()
        \row
            \o glCallList()
            \o draw(QGLPainter *)            
    \endtable

    In addition to this function based API, you can also use the QGLOperation
    class, which provides an object based API.

    \section1 Display Lists and Scene Nodes

    QGLSceneNodes are used to manage application of local transformations,
    materials and effects, similar to how glRotate() or glMaterial()
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
    Add a triangle defined by \a a, \a b and \c to this display list.
    When \a normal is null (the default) normals supplied in \a a, \a b and
    \a c are used.  If any of those are null they are set to the triangle
    normal calculated as cross-product \c{(b - a) x (c - a)}.  In the case
    of a degenerate triangle, where this cross-product is null, \c{(1, 0, 0}
    is used.
*/
void QGLDisplayList::addTriangle(QLogicalVertex a, QLogicalVertex b, QLogicalVertex c,
                                 const QVector3D &normal)
{
    Q_D(QGLDisplayList);
    Q_ASSERT(d->currentSection);
    Q_ASSERT(d->currentNode);
    QVector3D norm = normal;
    if (!norm.isNull())
    {
        a.setNormal(norm);
        b.setNormal(norm);
        c.setNormal(norm);
    }
    else if ((!a.hasField(QGL::Normal) || !b.hasField(QGL::Normal) || !c.hasField(QGL::Normal)))
    {
        norm = QVector3D::crossProduct(b - a, c - a);
        if (norm.isNull())
            norm = QVector3D(1.0f, 0.0f, 0.0f);
        if (!a.hasField(QGL::Normal) || a.normal().isNull())
            a.setNormal(norm);
        if (!b.hasField(QGL::Normal) || b.normal().isNull())
            b.setNormal(norm);
        if (!c.hasField(QGL::Normal) || c.normal().isNull())
            c.setNormal(norm);
    }
    d->currentSection->append(a);
    d->currentSection->append(b);
    d->currentSection->append(c);
    d->currentNode->setCount(d->currentNode->count() + 3);
}

/*!
    Add a \a triangle or list of triangles to this display list.  Normals
    are handled for each group of three vertices as per addTriangle(),
    except triangle.commonNormal() is passed as the normal argument.
    Any vertices at the end of the list under a multiple of 3 are
    ignored.
    \sa addQuad()
*/
void QGLDisplayList::addTriangle(const QGLPrimitive &triangle)
{
    for (int i = 0; i < triangle.count(); i += 3)
    {
        addTriangle(triangle.vertexAt(i), triangle.vertexAt(i+1),
                    triangle.vertexAt(i+2), triangle.commonNormal());
    }
}

void addQuad(QLogicalVertex a, QLogicalVertex b,
                 QLogicalVertex c, QLogicalVertex d,
                 const QVector3D &normal = QVector3D())
{
    QVector3D norm = normal;
    if (!quad.hasField(QGL::Normal) && norm.isNull())
    {
        norm = QVector3D::crossProduct(b - a, c - a);
        if (norm.isNull())
            norm = QVector3D(1.0f, 0.0f, 0.0f);
    }
    addTriangle(a, b, c, norm);
    addTriangle(a, c, d, norm);
}

/*!
    Add a \a quad or list of quads to this display list.  Each quad
    is broken up into two triangles, and added via the addTriangle()
    function.

    One normal for each quad is calculated and applied to both triangles,
    if \a quad does not have normals, either via \c{hasField(QGL::Normal)}
    or commonNormal().  For this reason this function may be slightly
    faster than the other addQuad() which must calculate the normal twice.

    \sa addTriangle()
*/
void QGLDisplayList::addQuad(const QGLPrimitive &quad)
{
    for (int i = 0; i < quad.count(); i += 4)
    {
        QLogicalVertex a(quad, i);
        QLogicalVertex b(quad, i+1);
        QLogicalVertex c(quad, i+2);
        QLogicalVertex d(quad, i+3);
        QVector3D norm = quad.commonNormal();
        if (!quad.hasField(QGL::Normal) && norm.isNull())
        {
            norm = QVector3D::crossProduct(b - a, c - a);
            if (norm.isNull())
                norm = QVector3D(1.0f, 0.0f, 0.0f);
        }
        addTriangle(a, b, c, norm);
        addTriangle(a, c, d, norm);
    }
}

/*!
    Adds to this section a set of connected triangles defined by \a fan.

    N triangular faces are generated, where \c{N == fan.count() - 2}. Each
    face contains the 0th vertex in \a fan, followed by the i'th and i+1'th
    vertex - where i takes on the values from 1 to fan.count() - 1.

    If \a fan has less than three vertices this function exits without
    doing anything.

    This function is very similar to the OpenGL mode GL_TRIANGLE_FAN.  It
    generates a number of triangles all sharing one common vertex, which
    is the 0'th vertex of the \a fan.

    Note that the control() vector on \a fan is ignored.

    \sa addTriangulatedFace()
*/
void QGLDisplayList::addTriangleFan(const QGLPrimitive &fan)
{
    QLogicalVertex center(fan, 0);
    for (int i = 1; i < fan.count() - 1; ++i)
        addTriangle(center, fan.vertexAt(i), fan.vertexAt(i+1));
}

/*!
    Adds to this section a set of connected triangles defined by \a strip.

    N triangular faces are generated, where \c{N == strip.count() - 2}.
    The triangles are generated from vertices 0, 1, & 2, then 2, 1 & 3,
    then 2, 3 & 4, and so on.  In other words every second triangle has
    the first and second vertices switched, as a new triangle is generated
    from each successive set of three vertices.

    If \a fan has less than three vertices this function exits without
    doing anything.

    Normals are calculated as for addTriangle, given the above ordering.

    This function is very similar to the OpenGL mode GL_TRIANGLE_STRIP.  It
    generates triangles along a strip whose two sides are the even and odd
    vertices.

    \sa addTriangulatedFace()
*/
void QGLDisplayList::addTriangleStrip(const QGLPrimitive &strip)
{
    for (int i = 0; i < strip.count() - 1; ++i)
    {
        if (i % 2)
            addTriangle(strip.vertexAt(i), strip.vertexAt(i+1), strip.vertexAt(i+2));
        else
            addTriangle(strip.vertexAt(i+1), strip.vertexAt(i), strip.vertexAt(i+2));
    }
}

/*!
    Adds to this section a set of quads defined by \a strip.

    The first quad is formed from the 0'th, 2'nd, 3'rd and 1'st vertices.
    The second quad is formed from the 2'nd, 4'th, 5'th and 3'rd vertices,
    and so on, as shown in this diagram:

    \image quads.png

    This is consistent with all the other QGLDisplayList functions, as
    described above, but different from the order used by OpenGL's
    GL_QUAD_STRIP mode.
*/
void QGLDisplayList::addQuadStrip(const QGLPrimitive &strip)
{
    for (int i = 0; i < strip.count(); i += 2)
    {
        QLogicalVertex a(strip, i);
        QLogicalVertex b(strip, i+2);
        QLogicalVertex c(strip, i+3);
        QLogicalVertex d(strip, i+1);
        QVector3D norm = strip.commonNormal();
        if (!strip.hasField(QGL::Normal) && norm.isNull())
        {
            norm = QVector3D::crossProduct(b - a, c - a);
            if (norm.isNull())
                norm = QVector3D(1.0f, 0.0f, 0.0f);
        }
        addTriangle(a, b, c, norm);
        addTriangle(a, c, d, norm);
    }
}

/*!
    Adds to this section a set of triangles defined by \a face.  The
    vertices in face are treated as first a central vertex, followed by
    the perimeter vertices of a polygonal face.

    In general all vertices in \a face should all lie in the same
    plane, and the 0'th (center) vertex must lie strictly within the region
    bounded by other vertices, otherwise unexpected behaviour may result.

    If \a face has three or fewer vertices this function behaves exactly
    like addTriangleFan().

    When \a face has four or more vertices, N triangular faces are generated
    where \c{N == fan.count() - 1}.  Each face contains the 0th vertex in
    \a face; followed by the \c{i'th} and \c{((i + 1) % N)'th} vertex.

    To supress the closing face, use \c{face.setFlags(QGL::NO_CLOSE_PATH)}.

    This function works similarly to the OpenGL mode GL_POLYGON, except it
    requires an additional center vertex as the 0th vertex, since it breaks
    the polygonal face into triangles.

    If no normals are supplied in the vertices of \a face, normals are
    calculated as per addTriangle().

    One normal is calculated, since a faces vertices lie in the same plane.

    The sub-faces will have normals and windings all equal to the triangle
    \c{(face[0], face[1], face[2])}.  For this reason the vertices of \a face
    should proceed as a counter-clockwise perimeter of the face.

    Note that since the normal calculation can be expensive this function
    may be faster than addTriangleFan() which must calculate a new normal
    for every sub-face.
*/
void QGLDisplayList::addTriangulatedFace(const QGLPrimitive &face)
{
    if (face.count() < 4)
    {
        addTriangleFan(face);
    }
    else
    {
        QDataArray<QVector3D> v = face.vertices();
        QVector3D norm = QVector3D::normal(v[0], v[1], v[2]);
        QLogicalVertex center(face, 0);
        int cnt = (face.flags() & QGL::NO_CLOSE_PATH) ? face.count() - 1 : face.count();
        for (int i = 1; i < cnt; ++i)
        {
            int n = (i + 1) % face.count();
            addTriangle(face.vertexAt(n), face.vertexAt(n+1), center, norm);
        }
    }
}

/*!
    Add a series of quads by 'zipping' \a top and \a bottom.

    This function behaves like quadStrip(), where the odd-numbered vertices in
    the input primitive are from \a top and the even-numbered vertices from
    \a bottom.  The main difference is that this function adds a closing face.

    \code
    // addQuadsZipped(top, bottom) is the same as:
    QGLPrimitive zipped;
    for (int i = 0; i < qMin(top.count(), bottom.count()); ++i)
    {
        zipped.appendVertex(bottom.vertexAt(i));
        zipped.appendVertex(top.vertexAt(i));
    }
    zipped.appendVertex(bottom.vertexAt(0);
    zipped.appendVertex(top.vertexAt(0);
    displayList->addQuadStrip(zipped);
    \endcode

    If the vertices in \a top and \a bottom are the perimeter vertices of
    two polygons then this function can be used to generate quads which form
    the sides of a \l{http://en.wikipedia.org/wiki/Prism_(geometry)}{prism}
    with the polygons as the prisms top and bottom end-faces.

    N quad faces are generated where \c{N == min(top.count(), bottom.count()}.
    If \a top or \a bottom has less than 2 elements, this functions does
    nothing.

    Each face is formed by the \c{i'th} and \c{((i + 1) % N)'th}
    vertices of \a bottom, followed by the \c{((i + 1) % N)'th} and \c{i'th}
    vertices of \a top.

    To supress the closing face, use \c{face.setFlags(QGL::NO_CLOSE_PATH)}.
    In this case, \c{N == min(top.count(), bottom.count()) - 1}.

    If \a top or \a bottom has the QGL::FACE_SENSE_REVERSED flag set then
    vertices are treated as being in clockwise order for the purpose of
    generating the winding and face normals.

    \image quad-extrude.png

    In the diagram above, the \a top is shown in orange, and the \a bottom in
    dark yellow.  The first generated quad, (a, b, c, d) is generated in
    the order shown by the blue arrow.

    To create a complete prismatic solid given the top edge do this:
    \code
    QGLPrimitive top = generateEdge();
    QGLPrimitive bottom = top.translated(QVector3D(0, 0, -1));
    displayList->addQuadsZipped(top, bottom);
    displayList->addTriangulatedFace(top);
    displayList->addTriangulatedFace(bottom.reversed());
    \endcode
    The \a bottom QGLPrimitive must be \bold{reversed} so that the correct
    winding for an outward facing polygon is obtained.
*/
void QGLDisplayList::addQuadsZipped(const QGLPrimitive &top,
                                    const QGLPrimitive &bottom)
{
    int cnt = qMin(top.count(), bottom.count());
    if (cnt >= 3)
    {
        int max = cnt;
        if ((top.flags() | bottom.flags()) & QGL::NO_CLOSE_PATH)
            --cnt;
        bool reverse = ((top.flags() | bottom.flags()) & QGL::FACE_SENSE_REVERSED);
        for (int i = 0; i < cnt; ++i)
        {
            int n = (i + 1) % max;
            if (reverse)
                addQuad(bottom.vertexAt(n), bottom.vertexAt(i),
                        top.vertexAt(i), top.vertexAt(n));
            else
                addQuad(bottom.vertexAt(i), bottom.vertexAt(n),
                        top.vertexAt(n), top.vertexAt(i));
        }
    }
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
        QMap<quint32, QGLGeometry *> geos;
        while (d->sections.count())
        {
            // pack sections that have the same fields into one geometry
            QGLSection *s = d->sections.takeFirst();
            s->finalize();
            QGLIndexArray indices = s->indices();
            const int *vi = indices.constData();
            int vcnt = indices.count();
            int sectionOffset = 0;
            int sectionIndexOffset = 0;
            QMap<quint32, QGLGeometry *>::const_iterator it =
                    geos.constFind(s->fields());
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
                geos.insert(s->fields(), g);
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


void QGLDisplayList::begin(QGLDisplayList::Operation operation,
                           const QVector3D &vector)
{
    Q_D(QGLDisplayList);
    end();
    switch (operation)
    {
    case NO_OP:
        break;
    case TRIANGLE:
        d->currentOperation = new QGLTriangle(this); break;
    case TRIANGLE_STRIP:
        d->currentOperation = new QGLTriangleStrip(this); break;
    case QUAD:
        d->currentOperation = new QGLQuad(this); break;
    case QUAD_STRIP:
        d->currentOperation = new QGLQuadStrip(this); break;
    case TRIANGLE_FAN:
        d->currentOperation = new QGLTriangleFan(this, vector); break;
    case TRIANGULATED_FACE:
        d->currentOperation = new QGLTriangulatedFace(this); break;
    case EXTRUSION:
        d->currentOperation = new QGLExtrusion(this); break;
    }
}

void QGLDisplayList::end()
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
    {
        d->currentOperation->finalize();
        delete d->currentOperation;
        d->currentOperation = 0;
    }
}

void QGLDisplayList::addVertex(const QVector3D &vertex)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->addVertex(vertex);
}

void QGLDisplayList::addNormal(const QVector3D &normal)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->addNormal(normal);
}

void QGLDisplayList::addColor(const QColor4b &color)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->addColor(color);
}

void QGLDisplayList::addTexCoord(const QVector2D &texCoord)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->addTexCoord(texCoord);
}

void QGLDisplayList::addVertexArray(const QGL::VectorArray &vertices)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->addVertexArray(vertices);
}

void QGLDisplayList::addNormalArray(const QGL::VectorArray &normals)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->addNormalArray(normals);
}

void QGLDisplayList::addColorArray(const QGL::ColorArray &colors)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->addColorArray(colors);
}

void QGLDisplayList::addTexCoordArray(const QGL::TexCoordArray &texCoords)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->addTexCoordArray(texCoords);
}

QGL::VectorArray QGLDisplayList::endResult()
{
    Q_D(QGLDisplayList);
    QGL::VectorArray result;
    if (d->currentOperation)
    {
        d->currentOperation->finalize();
        result = d->currentOperation->vertices();
        delete d->currentOperation;
        d->currentOperation = 0;
    }
    return result;
}

void QGLDisplayList::addTextureModel(const QGLTextureModel &model)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->setTextureModel(model);
}

void QGLDisplayList::addColorModel(const QGLColorModel &model)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->setColorModel(model);
}

