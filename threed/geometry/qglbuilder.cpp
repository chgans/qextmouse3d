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

#include "qglbuilder.h"
#include "qglbuilder_p.h"
#include "qglsection_p.h"
#include "qglmaterialcollection.h"
#include "qglpainter.h"
#include "qgeometrydata.h"
#include "qvector_utils_p.h"

#include <QtGui/qvector2d.h>

#include <QtCore/qdebug.h>

/*!
    \class QGLBuilder
    \brief The QGLBuilder class accumulates geometry for efficient display.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    \tableofcontents

    Use a QGLBuilder to build up vertex, index, texture and other data
    when an application starts up, then it can be efficiently and flexibly
    displayed during frames of rendering.  It is suited to writing loaders
    for 3D models, and also for programatically creating geometry.

    \section1 Comparison with OpenGL fixed-functions

    QGLBuilder contains functions which provide similar functionality to
    OpenGL modes GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_QUADS and so on.  There
    is currently no support for GL_LINES or GL_POINTS.

    Also, with regard to the ordering and winding of vertices, QGLBuilder
    always starts at the origin, with (0, 0) being the bottom-left.

    The order follows with (1, 0) bottom-right, (1, 1) top-right and
    lastly (1, 0) top-left - this gives an anti-clockwise wound face.

    So, while remembering that QGLBuilder does not attempt to exactly
    replicate OpenGL, these are QGLBuilders geometry functions and the
    OpenGL mode/function they most resemble:

    \table
        \header
            \o QGLBuilder function
            \o Similar to OpenGL mode/function
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
            \o addQuads)
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
            \o addQuadStrip(), addQuadsZipped()
            \o GL_QUAD_STRIP
    \endtable

    \section1 Index and Normal Generation

    QGLBuilder automatically generates index values and normals
    on-the-fly during geometry building.  During building, simply send
    primitives to the display list as a sequence of vertices, and
    vertices that are the same will be referenced by a single index
    automatically.

    Primitives will have standard normals generated automatically
    based on vertex winding.

    Consider the following code for OpenGL to draw a quad with corner
    points A, B, C and D :

    \code
    float vertices[12] =
    {
        -1.0, -1.0, -1.0,   // A
        1.0, -1.0, -1.0,    // B
        1.0, 1.0, 1.0,      // C
        -1.0, 1.0, 1.0      // D
    };
    float normals[12] = { 0.0f };
    for (int i = 0; i < 12; i += 3)
    {
        normals[i] = 0.0;
        normals[i+1] = -sqrt(2.0);
        normals[i+2] = sqrt(2.0);
    }
    GLuint indices[6] = {
        0, 1, 2,     // triangle A-B-C
        0, 2, 3      // triangle A-C-D
    };
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glNormalPointer(3, GL_FLOAT, 0, normals);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
    \endcode

    With QGLBuilder this code becomes:

    \code
    float vertices[12] =
    {
        -1.0, -1.0, -1.0,   // A
        1.0, -1.0, -1.0,    // B
        1.0, 1.0, 1.0,      // C
        -1.0, 1.0, 1.0      // D
    };
    QGLBuilder quad;
    QGeometryData op;
    for (int i = 0; i < 12; i += 3)
        op.appendVertex(QVector3D(vertices[i], vertices[i+1], vertices[i+2]));
    quad.addQuad(op);
    \endcode

    The quad primitive is added to the list, as two triangles, indexed to
    removed the redundant double storage of B & C - just the same as the
    OpenGL code.

    It will also calculate a normal for the quad and apply it
    to the vertices.

    In this trivial example the indices are easily calculated, however
    in more complex geometry it is easy to introduce bugs by trying
    to manually control indices.  Extra work is required to generate,
    track and store the index values correctly.

    Bugs such as trying to index two vertices with different data -
    one with texture data and one without - into one triangle can
    easily result.  The picture becomes more difficult when smoothing
    groups are introduced - see below.

    Using indices is always preferred since it saves space on the GPU,
    and makes the geometry perform faster during application run time.

    \section2 Removing Epsilon Errors

    Where vertices are generated by modelling packages or tools, or
    during computation in code, very frequently rounding errors will
    result in several vertices being generated that are actually
    the same vertex but are seperated by tiny amounts.  At best these
    duplications waste space on the GPU but at worst can introduce
    visual artifacts that mar the image displayed.

    Closing paths, generating solids of rotation, or moving model
    sections out and back can all introduce these types of epsilon
    errors, resulting in "cracks" or artifacts on display.

    QGLBuilder's index generation process uses a fuzzy match that
    coalesces all vertex values at a point - even if they are out by
    a tiny amount - and references them with a single index.

    \section2 Lighting Normals and Null Triangles

    QGLBuilder functions calculate lighting normals, when building
    geometry.  This saves the application programmer from having to write
    code to calculate them.  Normals for each triangle (a, b, c) are
    calculated as the QVector3D::normal(a, b, c).

    If lighting normals are explicitly supplied when using QGLBuilder,
    then this calculation is not done.  This may save on build time.

    As an optimization, QGLBuilder skips null triangles, that is ones
    with zero area, where it can.  Such triangles generate no fragments on
    the GPU, and thus do not display but nonetheless can take up space
    and processing power.

    Null triangles can easily occur when calculating vertices results
    in two vertices coinciding, or three vertices lying on the same line.

    This skipping is done using the lighting normals cross-product.  If the
    cross-product is a null vector then the triangle is null.

    When lighting normals are specified explicitly the skipping
    optimization is suppressed, so if for some reason null triangles are
    required to be retained, then specify normals for each logical vertex,
    or via the QGeometry::setCommonNormal() function.

    See the documentation below of the individual addTriangle() and other
    functions for more details.

    \section2 Raw Triangle Mode

    Where generation of indices and normals is not needed - for example if
    porting an existing application, or where indices and normals result
    naturally; it is possible to do a very fast add of triangles, without
    using any of QGLBuilder's processing.

    Simply ensure that indices are placed in the QGeometryData passed to
    the addTriangle() function, and this will trigger "raw triangle" mode.

    When adding triangles in this way ensure that all appropriate values
    have been correctly set, and that the normals, indices and other data
    are correctly calculated, since no checking is done.

    \section1 Rendering and QGLSceneNode items.

    QGLSceneNodes are used to manage application of local transformations,
    materials and effects.

    QGLBuilder generates a root level QGLSceneNode, which can be accessed
    with the sceneNode() function.  Under this a new node is created for
    each section of geometry, and also by using pushNode() and popNode().

    \image soup.png

    Here the front can is a set of built geometry and the other two are
    scene nodes that reference it, without copying any geometry.

    \snippet displaylist/displaylist.cpp 0

    QGLSceneNodes can be used after the display list is created to cheaply
    copy and redisplay the whole list.  Or to reference parts of the list
    use the functions newNode() or pushNode() and popNode() to manage
    QGLSceneNode generation while building geometry.

    To draw the resulting built geometry simply call the draw method of the
    build geometry.

    \snippet displaylist/displaylist.cpp 1

    Call the \l{QGLSceneNode::palette()}{palette()} function on the sceneNode()
    to get the QGLMaterialCollection for the node, and place textures
    and materials into it.

    Built geometry will typically share the one palette.  Either create a
    palette, and pass it to the \l{QGLBuilder::QGLBuilder()}{constructor};
    or pass no arguments to the constructor and the QGLBuilder
    will create a palette:

    \snippet displaylist/displaylist.cpp 2

    These may then be applied as needed throughout the building of the
    geometry using the integer reference, \c{canMat} in the above code.

    \section1 Using Sections

    During initialization of the QGLBuilder, while accumulating
    geometry, the geometry data in a QGLBuilder is placed into
    sections - there must be at least one section.

    Call the newSection() function to create a new section:

    \snippet displaylist/displaylist.cpp 3

    Here seperate sections for the rounded outside cylinder and flat top and
    bottom of the soup can model makes for the appearance of a sharp edge
    between them.  If the sides and top and bottom were in the same section
    QGLBuilder would attempt to average the normals around the edge resulting
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

    Again all this is managed automatically by QGLBuilder and all
    that is required is to create smooth or faceted sections, and add
    geometry to them.

    Each QGLSection references a contiguous range of vertices in a
    QGLBuilder.

    \section1 Finalizing a QGLBuilder

    Once the geometry has been accumulated in the QGLBuilder instance,  the
    finalize() method must be called to normalize the geometry and optimize
    it for display.  The finalize() method makes passes through the data of
    each section, normalizing and then optimizing and preparing the data for
    display.  Thus it may be expensive for large geometry.

    The finalize() function only needs to be called once in the application
    lifetime.  It is automatically called by the QGLBuilder's destructor.
*/

QGLBuilderPrivate::QGLBuilderPrivate(QGLBuilder *parent)
    : finalizeNeeded(true)
    , currentSection(0)
    , currentNode(0)
    , rootNode(0)
    , defThreshold(5)
    , q(parent)
{
}

QGLBuilderPrivate::~QGLBuilderPrivate()
{
    qDeleteAll(sections);
    if (rootNode)
    {
        qWarning("Destroying QGLBuilder but finalizedSceneNode() not called");
        delete rootNode;
    }
}

/*!
    Construct a new QGLBuilder using \a materials for the palette.  If the
    \a materials argument is null, then a new palette is created.
*/
QGLBuilder::QGLBuilder(QGLMaterialCollection *materials)
    : dptr(new QGLBuilderPrivate(this))
{
    dptr->rootNode = new QGLSceneNode;
    if (!materials)
        materials = new QGLMaterialCollection(dptr->rootNode);
    dptr->rootNode->setPalette(materials);
}

/*!
    Destroys this QGLBuilder recovering any resources.
*/
QGLBuilder::~QGLBuilder()
{
    delete dptr;
}

/*!
    \internal
    Helper function to calculate normal if required, and actually
    add the vertices to geometry.

    Note the modifiable reference p - any normal calculated is
    stored in here, and needs to be restored unless persisting it
    to the next iteration.
*/
void QGLBuilderPrivate::addTriangle(int i, int j, int k, QGeometryData &p)
{
    if (currentSection == 0)
        q->newSection();
    bool calcNormal = !p.hasField(QGL::Normal) && p.commonNormal().isNull();
    QLogicalVertex a(p, i);
    QLogicalVertex b(p, j);
    QLogicalVertex c(p, k);
    QVector3D norm;
    if (calcNormal)
        norm = QVector3D::crossProduct(b - a, c - b);
    if (qFskIsNull(norm.x()))
        norm.setX(0.0f);
    if (qFskIsNull(norm.y()))
        norm.setY(0.0f);
    if (qFskIsNull(norm.z()))
        norm.setZ(0.0f);
    norm *= 5.0f;
    // if the normal was calculated, and it was null, then this is a null
    // triangle - don't add it, it just wastes space - see class doco
    if (!calcNormal || !norm.isNull())
    {
        // use common normal to communicate the normal down the stack
        if (calcNormal)
            p.setCommonNormal(norm);
        currentSection->append(a, b, c);
        currentNode->setCount(currentNode->count() + 3);
    }
}

/*!
    Add a \a triangle or series of triangles to this display list.

    If \a triangle has indices specified then no processing of any kind is
    done and all the geometry is simply dumped in to the display list.

    This "raw triangle" mode is for advanced use, and it is assumed that
    the user knows what they are doing, in particular that the indices
    supplied are correct, and normals are supplied and correct.

    Otherwise, if no indices are supplied, then the list is broken into
    groups of 3 vertices, each processed as a triangle.

    Any vertices at the end of the list under a multiple of 3 are
    ignored.

    If no normals are supplied in \a triangle, a normal is calculated as
    the cross-product \c{(b - a) x (c - a)}, for each group of 3
    logical vertices \c{a(triangle, i), b(triangle, i+1), c(triangle, i+2)}.

    In the case of a degenerate triangle, where the cross-product is null,
    that triangle is skipped.  Supplying normals suppresses this behaviour
    and degenerate triangles will be added to the geometry.

    Normals are not calculated in "raw triangle" mode, and skipping of null
    triangles is likewise not performed.

    \sa addQuads)
*/
void QGLBuilder::addTriangles(const QGeometryData &triangle)
{
    if (triangle.indexCount() > 0)
    {
        // raw triangle mode
        dptr->currentSection->appendGeometry(triangle);
        dptr->currentSection->appendIndices(triangle.indices());
        dptr->currentNode->setCount(dptr->currentNode->count() + triangle.indexCount());
    }
    else
    {
        QGeometryData t = triangle;
        QVector3D save = t.commonNormal();
        for (int i = 0; i < t.count() - 2; i += 3)
        {
            dptr->addTriangle(i, i+1, i+2, t);
            t.setCommonNormal(save);
        }
    }
}

/*!
    Add a \a quad or series of quads to this display list.  Each quad
    is broken up into two triangles.

    One normal per quad is calculated and applied to both triangles,
    if \a quad does not have normals, either via \c{hasField(QGL::Normal)}
    or commonNormal().  Degenerate triangles are skipped in the same way
    as addTriangle().

    \sa addTriangle()
*/
void QGLBuilder::addQuads(const QGeometryData &quad)
{
    QGeometryData q = quad;
    QVector3D save = q.commonNormal();
    for (int i = 0; i < q.count(); i += 4)
    {
        dptr->addTriangle(i, i+1, i+2, q);
        dptr->addTriangle(i, i+2, i+3, q);
        q.setCommonNormal(save);
    }
}

/*!
    Adds to this section a set of connected triangles defined by \a fan.

    N triangular faces are generated, where \c{N == fan.count() - 2}. Each
    face contains the 0th vertex in \a fan, followed by the i'th and i+1'th
    vertex - where i takes on the values from 1 to \c{fan.count() - 1}.

    If \a fan has less than three vertices this function exits without
    doing anything.

    This function is similar to the OpenGL mode GL_TRIANGLE_FAN.  It
    generates a number of triangles all sharing one common vertex, which
    is the 0'th vertex of the \a fan.

    Normals are calculated as for addTriangle(), given the above ordering.
    Note that the central() vector on \a fan is ignored.

    \sa addTriangulatedFace()
*/
void QGLBuilder::addTriangleFan(const QGeometryData &fan)
{
    QGeometryData f = fan;
    QVector3D save = f.commonNormal();
    for (int i = 1; i < f.count() - 1; ++i)
    {
        dptr->addTriangle(0, i, i+1, f);
        f.setCommonNormal(save);
    }
}

/*!
    Adds to this section a set of connected triangles defined by \a strip.

    N triangular faces are generated, where \c{N == strip.count() - 2}.
    The triangles are generated from vertices 0, 1, & 2, then 2, 1 & 3,
    then 2, 3 & 4, and so on.  In other words every second triangle has
    the first and second vertices switched, as a new triangle is generated
    from each successive set of three vertices.

    If \a strip has less than three vertices this function exits without
    doing anything.

    Normals are calculated as for addTriangle(), given the above ordering.

    This function is very similar to the OpenGL mode GL_TRIANGLE_STRIP.  It
    generates triangles along a strip whose two sides are the even and odd
    vertices.

    \sa addTriangulatedFace()
*/
void QGLBuilder::addTriangleStrip(const QGeometryData &strip)
{
    QGeometryData s = strip;
    QVector3D save = s.commonNormal();
    for (int i = 0; i < s.count() - 2; ++i)
    {
        if (i % 2)
            dptr->addTriangle(i+1, i, i+2, s);
        else
            dptr->addTriangle(i, i+1, i+2, s);
        s.setCommonNormal(save);
    }
}

/*!
    Adds to this section a set of quads defined by \a strip.

    The first quad is formed from the 0'th, 2'nd, 3'rd and 1'st vertices.
    The second quad is formed from the 2'nd, 4'th, 5'th and 3'rd vertices,
    and so on, as shown in this diagram:

    \image quads.png

*/
void QGLBuilder::addQuadStrip(const QGeometryData &strip)
{
    QGeometryData s = strip;
    QVector3D save = s.commonNormal();
    for (int i = 0; i < s.count() - 3; i += 2)
    {
        dptr->addTriangle(i, i+2, i+3, s);
        dptr->addTriangle(i, i+3, i+1, s);
        s.setCommonNormal(save);
    }
}

/*!
    Adds to this section a polygonal face, made of triangular sub-faces
    defined by \a face.  This function provides functionality similar to the
    OpenGL mode GL_POLYGON, except it divides the face into sub-faces
    around a \bold{central point}.  The 0'th vertex is used for the center.

    \image triangulated-face.png

    Here the sub-faces are shown divided by green lines.  Note how this
    function handles some re-entrant (non-convex) polygons, whereas
    addTriangleFan will not support such polygons.

    If required, the center point can be calculated using the center() function
    of QGeometryData:

    \code
    QGeometryData face;
    face.appendVertex(perimeter.center()); // perimeter is a QGeometryData
    face.appendVertices(perimeter);
    builder.addTriangulatedFace(face);
    \endcode

    N sub-faces are generated where \c{N == face.count() - 1}.
    Each triangular sub-face consists the center; followed by the \c{i'th}
    and \c{((i + 1) % N)'th} vertex.  The last face generated then is
    \c{(center, face[N - 1], face[0]}, the closing face.

    If N is 0, this function exits without doing anything.

    If no normals are supplied in the vertices of \a face, normals are
    calculated as per addTriangle().  One normal is calculated, since a
    faces vertices lie in the same plane.
*/
void QGLBuilder::addTriangulatedFace(const QGeometryData &face)
{
    QGeometryData f;
    f.appendGeometry(face);
    int cnt = f.count();
    if (cnt > 1)
    {
        for (int i = 1; i < cnt; ++i)
        {
            int n = i + 1;
            if (n == cnt)
                n = 1;
            dptr->addTriangle(0, i, n, f);
        }
    }
}

/*!
    Add a series of quads by 'interleaving' \a top and \a bottom.

    This function behaves like quadStrip(), where the odd-numbered vertices in
    the input primitive are from \a top and the even-numbered vertices from
    \a bottom.

    It is trivial to do extrusions using this function:

    \code
    // create a series of quads for an extruded edge
    addQuadsInterleaved(topEdge, topEdge.translated(QVector3D(0, -1, 0));
    \endcode

    N quad faces are generated where \c{N == min(top.count(), bottom.count() - 1}.
    If \a top or \a bottom has less than 2 elements, this functions does
    nothing.

    Each face is formed by the \c{i'th} and \c{(i + 1)'th}
    vertices of \a bottom, followed by the \c{(i + 1)'th} and \c{i'th}
    vertices of \a top.

    If the vertices in \a top and \a bottom are the perimeter vertices of
    two polygons then this function can be used to generate quads which form
    the sides of a \l{http://en.wikipedia.org/wiki/Prism_(geometry)}{prism}
    with the polygons as the prisms top and bottom end-faces.

    \image quad-extrude.png

    In the diagram above, the \a top is shown in orange, and the \a bottom in
    dark yellow.  The first generated quad, (a, b, c, d) is generated in
    the order shown by the blue arrow.

    To create such a extruded prismatic solid, complete with top and bottom cap
    polygons, given just the top edge do this:
    \code
    QGeometryData top = buildTopEdge();
    QGeometryData bottom = top.translated(QVector3D(0, 0, -1));
    displayList->addQuadsInterleaved(top, bottom);
    displayList->addTriangulatedFace(top);
    displayList->addTriangulatedFace(bottom.reversed());
    \endcode
    The \a bottom QGeometryData must be \bold{reversed} so that the correct
    winding for an outward facing polygon is obtained.
*/
void QGLBuilder::addQuadsInterleaved(const QGeometryData &top,
                                     const QGeometryData &bottom)
{
    QGeometryData zipped = bottom.interleavedWith(top);
    QVector3D norm = top.commonNormal() + bottom.commonNormal();
    zipped.setCommonNormal(norm);
    for (int i = 0; i < zipped.count() - 2; i += 2)
    {
        dptr->addTriangle(i, i+2, i+3, zipped);
        dptr->addTriangle(i, i+3, i+1, zipped);
        zipped.setCommonNormal(norm);
    }
}

/*!
    \internal
*/
void QGLBuilderPrivate::adjustSectionNodes(QGLSection *sec,
                                       int offset, const QGeometryData &geom)
{
    QList<QGLSceneNode*> children = sec->nodes();
    QList<QGLSceneNode*>::iterator it = children.begin();
    QList<QGLSceneNode*> deleted;
    for ( ; it != children.end(); ++it)
        adjustNodeTree(*it, offset, geom, deleted);
}

/*!
    \internal
    Adjust \a top by incrementing its start by \a offset, and setting its
    geometry to \a geom.  Find the cumulative total of indexes -
    QGLSceneNode::count() - for \a top and all its children.  If this total is
    equal to zero, then delete that node.
*/
int QGLBuilderPrivate::adjustNodeTree(QGLSceneNode *top,
                                   int offset, const QGeometryData &geom,
                                   QList<QGLSceneNode*> &deleted)
{
    int totalItems = 0;
    if (top && !deleted.contains(top))
    {
        top->setStart(top->start() + offset);
        top->setGeometry(geom);
        totalItems = top->count();
        QList<QGLSceneNode*> children = top->childNodeList();
        QList<QGLSceneNode*>::iterator it = children.begin();
        for ( ; it != children.end(); ++it)
        {
            totalItems += adjustNodeTree(*it, offset, geom, deleted);
        }
        if (totalItems == 0 && top->objectName().isEmpty())
        {
            delete top;
            deleted.append(top);
        }
    }
    return totalItems;
}

/*!
    \internal
    Returns a count of all the items referenced by this node
    and all its children.
*/
static int recursiveCount(QGLSceneNode *top)
{
    int totalItems = 0;
    if (top)
    {
        totalItems = top->count();
        QList<QGLSceneNode*> children = top->childNodeList();
        QList<QGLSceneNode*>::const_iterator it = children.constBegin();
        for ( ; it != children.constEnd(); ++it)
            totalItems += recursiveCount(*it);
    }
    return totalItems;
}

static int nodeCount(const QList<QGLSceneNode*> &list)
{
    int total = 0;
    QList<QGLSceneNode*>::const_iterator it = list.constBegin();
    for  ( ; it != list.constEnd(); ++it)
        total += recursiveCount(*it);
    return total;
}

static inline void warnIgnore(int secCount, QGLSection *s, int vertCount, int nodeCount,
                              const char *msg)
{
    qWarning("Ignoring section %d (%p) with %d vertices and"
             " %d indexes - %s", secCount, s, vertCount, nodeCount, msg);
}

/*!
    Finish the building of this geometry, optimize it for rendering, and return a
    pointer to the detached top-level scene node (root node).

    The root node will have a child node for each section that was created
    during geometry building.

    This method must be called exactly once after building the scene.

    Calling code takes ownership of the scene.  If this builder is destroyed
    (deleted or goes out of scope) without calling this method to take
    ownership of the scene, a warning will be printed on the console and the
    scene will be deleted.  If this method is called more than once, on the
    second and subsequent calls a warning is printed and NULL is returned.

    Since the scene is detached from the builder object, the builder itself
    may be deleted or go out of scope while the scene lives on:

    \code
    void MyView::~MyView()
    {
        delete m_thing;
    }

    void MyView::initializeGL()
    {
        QGLBuilder builder;
        // construct geometry
        m_thing = builder.finalizedSceneNode();
    }

    void MyView::paintGL()
    {
        m_thing->draw(painter);
    }
    \endcode

    This function does the following:
    \list
        \o packs all geometry data from sections into QGLSceneNode instances
        \o references this data via QGLSceneNode start() and count()
        \o deletes all internal data structures
        \o returns the top level scene node that references the geometry
        \o sets the internal pointer to the top level scene node to NULL
    \endlist

    \sa sceneNode()
*/
QGLSceneNode *QGLBuilder::finalizedSceneNode()
{
    if (dptr->rootNode == 0)
    {
        qWarning("QGLBuilder::finalizedSceneNode() called twice");
        return 0;
    }
    QGeometryData g;
    QMap<quint32, QGeometryData> geos;
    QMap<QGLSection*, int> offsets;
    for (int i = 0; i < dptr->sections.count(); ++i)
    {
        // pack sections that have the same fields into one geometry
        QGLSection *s = dptr->sections.at(i);
        QGL::IndexArray indices = s->indices();
        int icnt = indices.size();
        int ncnt = nodeCount(s->nodes());
        int scnt = s->count();
        if (scnt == 0 || icnt == 0 || ncnt == 0)
        {
            if (!qgetenv("Q_WARN_EMPTY_MESH").isEmpty())
            {
                if (ncnt == 0)
                    warnIgnore(scnt, s, icnt, ncnt, "nodes empty");
                else if (scnt == 0)
                    warnIgnore(scnt, s, icnt, ncnt, "geometry count zero");
                else
                    warnIgnore(scnt, s, icnt, ncnt, "index count zero");
            }
            continue;
        }
        s->normalizeNormals();
        int sectionOffset = 0;
        int sectionIndexOffset = 0;
        if (geos.contains(s->fields()))
        {
            QGeometryData &gd = geos[s->fields()];
            sectionOffset = gd.count();
            sectionIndexOffset = gd.indexCount();
            offsets.insert(s, sectionIndexOffset);
            gd.appendGeometry(*s);
            for (int i = 0; i < icnt; ++i)
                indices[i] += sectionOffset;
            gd.appendIndices(indices);
        }
        else
        {
            g = QGeometryData(*s);
            geos.insert(s->fields(), g);
        }
    }
    while (dptr->sections.count() > 0)
    {
        QGLSection *s = dptr->sections.takeFirst();
        dptr->adjustSectionNodes(s, offsets[s], geos[s->fields()]);
        delete s;
    }
    QGLSceneNode *tmp = dptr->rootNode;
    dptr->rootNode = 0;  // indicates root node detached
    return tmp;
}

/*!
    Creates a new section with smoothing mode set to \a smooth.  By default
    \a smooth is QGL::Smooth.

    A section must be created before any geometry or new nodes can be added
    to the builder.  However one is created automatically by addTriangle()
    and the other add functions; and also by newNode(), pushNode() or popNode()
    if needed.

    The internal node stack - see pushNode() and popNode() - is cleared,
    and a new top-level QGLSceneNode is created for this section by calling
    newNode().

    \sa newNode(), pushNode()
*/
void QGLBuilder::newSection(QGL::Smoothing smooth)
{
    new QGLSection(this, smooth);  // calls addSection
}

void QGLBuilder::addSection(QGLSection *sec)
{
    dptr->currentSection = sec;
    sec->setMapThreshold(dptr->defThreshold);
    dptr->sections.append(sec);
    //qDebug() << "Adding section - clear node stack:" << sec;
    dptr->nodeStack.clear();
    newNode();
}

/*!
    \internal
    Returns the current section, in which new geometry is being added.
*/
QGLSection *QGLBuilder::currentSection() const
{
    return dptr->currentSection;
}

/*!
    \internal
    Returns a list of the sections of the geometry in this builder.
*/
QList<QGLSection*> QGLBuilder::sections() const
{
    return dptr->sections;
}

/*!
    \internal
    Test function only.
*/
void QGLBuilder::setDefaultThreshold(int t)
{
    dptr->defThreshold = t;
}

/*!
    Returns the root scene node of the geometry created by this builder.

    \sa newNode(), newSection()
*/
QGLSceneNode *QGLBuilder::sceneNode()
{
    return dptr->rootNode;
}

/*!
    Creates a new QGLSceneNode and makes it current.  A pointer to the new
    node is returned.  The node is added into the scene at the same level
    as the currentNode().

    The node is set to reference the geometry starting from the next
    vertex created, such that currentNode()->start() will return the
    index of this next vertex.

    \sa newSection()
*/
QGLSceneNode *QGLBuilder::newNode()
{
    if (dptr->currentSection == 0)
    {
        newSection();  // calls newNode()
        return dptr->currentNode;
    }
    QGLSceneNode *parentNode = dptr->rootNode;
    if (dptr->nodeStack.count() > 0)
        parentNode = dptr->nodeStack.last();
    dptr->currentNode = new QGLSceneNode(parentNode);
    //qDebug() << "newNode()" << dptr->currentNode << "parent:" << parentNode
    //        << "--- nodestack count:" << dptr->nodeStack.count();
    //if (dptr->nodeStack.count() > 0)
    //        qDebug() << "--- last:" << dptr->nodeStack.last();
    dptr->currentNode->setPalette(parentNode->palette());
    dptr->currentNode->setStart(dptr->currentSection->indexCount());
    if (dptr->nodeStack.count() == 0)
        dptr->currentSection->addNode(dptr->currentNode);
    return dptr->currentNode;
}

/*!
    Returns a pointer to the current scene node.

    \sa newNode(), newSection()
*/
QGLSceneNode *QGLBuilder::currentNode()
{
    if (dptr->currentSection == 0)
        newSection();  // calls newNode()
    return dptr->currentNode;
}

/*!
    Creates a new scene node that is a child of the current node and,
    makes it the current node.  A pointer to the new node is returned.
    The previous current node is saved on a stack and it may
    be made current again by calling popNode().

    \sa popNode(), newNode()
*/
QGLSceneNode *QGLBuilder::pushNode()
{
    if (dptr->currentSection == 0)
        newSection();  // calls newNode()
    //qDebug() << "pushNode() - current:" << dptr->currentNode;
    QGLSceneNode *parentNode = dptr->currentNode;
    dptr->nodeStack.append(parentNode);
    //qDebug() << "    currentNode:" << dptr->currentNode <<
    //        "--- nodeStack:" << dptr->nodeStack.count();
    dptr->currentNode = new QGLSceneNode(parentNode);
    dptr->currentNode->setStart(dptr->currentSection->indexCount());
    dptr->currentNode->setPalette(parentNode->palette());
    return dptr->currentNode;
}

/*!
    Removes the node from the top of the stack, makes a copy of it, and
    makes the copy current.

    If the stack is empty, behaviour is undefined.  In debug mode, calling
    this function when the stack is empty will cause an assert.

    A pointer to the new current node is returned.

    The node is set to reference the geometry starting from the next
    vertex created, such that QGLSceneNode::start() will return the
    index of this next vertex.

    \sa pushNode(), newNode()
*/
QGLSceneNode *QGLBuilder::popNode()
{
    if (dptr->currentSection == 0)
        newSection();  // calls newNode()
    int cnt = dptr->currentSection->indexCount();
    QGLSceneNode *s = dptr->nodeStack.takeLast();  // assert here
    //qDebug() << "popNode()" << s;
    QGLSceneNode *parentNode = dptr->rootNode;
    if (dptr->nodeStack.count() > 0)
        parentNode = dptr->nodeStack.last();
    dptr->currentNode = s->clone(parentNode);
    dptr->currentNode->setChildNodeList(QList<QGLSceneNode*>());
    dptr->currentNode->setStart(cnt);
    dptr->currentNode->setCount(0);
    dptr->currentNode->setPalette(parentNode->palette());
    if (dptr->nodeStack.count() == 0)
        dptr->currentSection->addNode(dptr->currentNode);
    return dptr->currentNode;
}

/*!
    Returns the palette for this builder.  This is the QGLMaterialCollection
    pointer that was passed to the constructor; or if that was null a new
    QGLMaterialCollection.  This function returns the same result as
    \c{sceneNode()->palette()}.

    \sa sceneNode()
*/
QGLMaterialCollection *QGLBuilder::palette()
{
    return dptr->rootNode->palette();
}

/*!
    \internal
    Mark the data as dirty and in need of loading/finalizing.
*/
void QGLBuilder::setDirty(bool dirty)
{
    dptr->finalizeNeeded = dirty;
}
