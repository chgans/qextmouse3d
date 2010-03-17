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
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::geometry

    \tableofcontents

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

    \section1 Comparison with OpenGL fixed-functions

    QGLDisplayList contains functions which provide similar functionality to
    OpenGL modes GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_QUADS and so on.  There
    is currently no support for GL_LINES or GL_POINTS.

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
            \o addQuadStrip(), addQuadsZipped()
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

    \section1 Index and Normal Generation

    QGLDisplayList automatically generates index values and normals
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

    With QGLDisplayList this code becomes:

    \code
    float vertices[12] =
    {
        -1.0, -1.0, -1.0,   // A
        1.0, -1.0, -1.0,    // B
        1.0, 1.0, 1.0,      // C
        -1.0, 1.0, 1.0      // D
    };
    QGLDisplayList quad;
    {
        QGLOperation op(&quad, QGL::QUAD);
        for (int i = 0; i < 12; i += 3)
            op << QVector3D(vertices[i], vertices[i+1], vertices[i+2]);
    }
    \endcode

    When the \c{op} variable goes out of scope it adds the
    quad primitive to the list, as two triangles, indexed to
    removed the redundant double storage of B & C - just the
    same as the OpenGL code.

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

    QGLDisplayList's index generation process uses a fuzzy match that
    coalesces all vertex values at a point - even if they are out by
    a tiny amount - and references them with a single index.

    \section2 Lighting Normals and Null Triangles

    QGLDisplayList functions calculate lighting normals, when building
    geometry.  This saves the application programmer from having to write
    code to calculate them.  Normals for each triangle (a, b, c) are
    calculated as the QVector3D::normal(a, b, c).

    If lighting normals are explicitly supplied when using QGLDisplayList,
    then this calculation is not done.  This may save on build time.

    As an optimization, QGLDisplayList skips null triangles, that is ones
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
    using any of QGLDisplayList's processing.

    Simply ensure that indices are created in the QGLPrimitive passed to
    the addTriangle() function, and this will trigger "raw triangle" mode.

    When adding triangles in this way ensure that all appropriate values
    have been correctly set, and that the normals, indices and other data
    are correctly calculated, since no checking is done.

    \section1 Display Lists and Scene Nodes

    QGLSceneNodes are used to manage application of local transformations,
    materials and effects, similar to how glRotate() or glMaterial()
    might be used.

    Since QGLDisplayList is itself a (sub-class of) QGLSceneNode, materials
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
    and materials into it.  Typically a display lists nodes, and usually the
    whole application will share the one palette, so if you have a top-level
    palette, you can pass it to the \l{QGLDisplayList::QGLDisplayList()}{constructor}.
    Normally, pass no arguments to the constructor and the QGLDisplayList
    will create its own internal palette:

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
    data in a display list is acessible as a QGLVertexBuffer:

    \code
    displayList->finalize();
    QGLVertexBuffer *data = displayList->geometry()->vertexBuffer();
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
    , currentOperation(0)
    , operation(QGL::NO_OP)
    , defThreshold(5)
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
    if (!materials)
        materials = new QGLMaterialCollection(this);
    setPalette(materials);
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
    Returns the QGLPrimitive for the current operation.  All add functions
    such as addVertex() and addNormal() accumulate data into this instance.
    Note that the pointer is only valid during the current operation.
*/
QGLPrimitive *QGLDisplayList::currentPrimitive()
{
    Q_D(QGLDisplayList);
    return d->currentOperation;
}

/*!
    \internal
    Helper function to calculate normal if required, and actually
    add the vertices to geometry.

    Note the modifiable reference p - any normal calculated is
    stored in here, and needs to be restored unless persisting it
    to the next iteration.
*/
void QGLDisplayListPrivate::addTriangle(int i, int j, int k, QGLPrimitive &p)
{
    Q_ASSERT(currentSection);
    Q_ASSERT(currentNode);
    bool calcNormal = !p.hasField(QGL::Normal) && p.commonNormal().isNull();
    QLogicalVertex a(p, i);
    QLogicalVertex b(p, j);
    QLogicalVertex c(p, k);
    QVector3D norm;
    if (calcNormal)
        norm = QVector3D::normal(a, b, c);
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

    \sa addQuad()
*/
void QGLDisplayList::addTriangle(const QGLPrimitive &triangle)
{
    Q_D(QGLDisplayList);
    if (triangle.indexCount() > 0)
    {
        d->currentSection->appendGeometry(triangle);
        d->currentSection->appendIndices(triangle.indices());
        d->currentNode->setCount(d->currentNode->count() + triangle.indexCount());
    }
    else
    {
        QGLPrimitive t = triangle;
        QVector3D save = t.commonNormal();
        for (int i = 0; i < t.count() - 2; i += 3)
        {
            d->addTriangle(i, i+1, i+2, t);
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
void QGLDisplayList::addQuad(const QGLPrimitive &quad)
{
    Q_D(QGLDisplayList);
    QGLPrimitive q = quad;
    QVector3D save = q.commonNormal();
    for (int i = 0; i < q.count(); i += 4)
    {
        d->addTriangle(i, i+1, i+2, q);
        d->addTriangle(i, i+2, i+3, q);
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

    This function is very similar to the OpenGL mode GL_TRIANGLE_FAN.  It
    generates a number of triangles all sharing one common vertex, which
    is the 0'th vertex of the \a fan.

    Normals are calculated as for addTriangle(), given the above ordering.
    Note that the central() vector on \a fan is ignored.

    \sa addTriangulatedFace()
*/
void QGLDisplayList::addTriangleFan(const QGLPrimitive &fan)
{
    Q_D(QGLDisplayList);
    QGLPrimitive f = fan;
    QVector3D save = f.commonNormal();
    for (int i = 1; i < f.count() - 1; ++i)
    {
        d->addTriangle(0, i, i+1, f);
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
void QGLDisplayList::addTriangleStrip(const QGLPrimitive &strip)
{
    Q_D(QGLDisplayList);
    QGLPrimitive s = strip;
    QVector3D save = s.commonNormal();
    for (int i = 0; i < s.count() - 2; ++i)
    {
        if (i % 2)
            d->addTriangle(i+1, i, i+2, s);
        else
            d->addTriangle(i, i+1, i+2, s);
        s.setCommonNormal(save);
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
    Q_D(QGLDisplayList);
    QGLPrimitive s = strip;
    QVector3D save = s.commonNormal();
    for (int i = 0; i < s.count(); i += 2)
    {
        d->addTriangle(i, i+2, i+3, s);
        d->addTriangle(i, i+3, i+1, s);
        s.setCommonNormal(save);
    }
}

/*!
    Adds to this section a polygonal face, made of triangular sub-faces
    defined by \a face.  This function provides functionality similar to the
    OpenGL mode GL_POLYGON, except it divides the face into sub-faces
    around a \bold{central point}.

    As a convenience the central point is by default set to the return
    value of \l{QGLPrimitive::center()}{face.center()}.  If \a face has texture
    coordinates or other data, set the QGL::USE_VERTEX_0_AS_CTR flag, in which
    case the 0'th vertex is used for the center.

    \image triangulated-face.png

    Here the sub-faces are shown divided by green lines.  Note how this
    function handles some re-entrant (non-convex) polygons, whereas
    addTriangleFan will not support such polygons.

    N sub-faces are generated where \c{N == face.count() - 1}.
    Each triangular sub-face consists the center; followed by the \c{i'th}
    and \c{((i + 1) % N)'th} vertex.  The last face generated then is
    \c{(center, face[N - 1], face[0]}, the closing face.

    To supress the closing face, use \c{face.setFlags(QGL::NO_CLOSE_PATH)}
    in which case \c{N == face.count() - 2}.

    If N is 0, this function exits without doing anything.

    If no normals are supplied in the vertices of \a face, normals are
    calculated as per addTriangle().  One normal is calculated, since a
    faces vertices lie in the same plane.
*/
void QGLDisplayList::addTriangulatedFace(const QGLPrimitive &face)
{
    Q_D(QGLDisplayList);
    QGLPrimitive f;
    if (!(face.flags() & QGL::USE_VERTEX_0_AS_CTR))
    {
        f.appendVertex(face.vertexAt(0));
        f.vertexRef(0) = f.center();
    }
    f.appendGeometry(face);
    int cnt = f.count();
    if (f.flags() & QGL::NO_CLOSE_PATH)
        cnt = f.count() - 1;
    if (cnt > 1)
    {
        for (int i = 1; i < cnt; ++i)
        {
            int n = i + 1;
            if (n == cnt)
                n = 1;
            d->addTriangle(0, i, n, f);
        }
    }
}

/*!
    Add a series of quads by 'zipping' \a top and \a bottom.

    This function behaves like quadStrip(), where the odd-numbered vertices in
    the input primitive are from \a top and the even-numbered vertices from
    \a bottom.

    If the vertices in \a top and \a bottom are the perimeter vertices of
    two polygons then this function can be used to generate quads which form
    the sides of a \l{http://en.wikipedia.org/wiki/Prism_(geometry)}{prism}
    with the polygons as the prisms top and bottom end-faces.

    N quad faces are generated where \c{N == min(top.count(), bottom.count() - 1}.
    If \a top or \a bottom has less than 2 elements, this functions does
    nothing.

    Each face is formed by the \c{i'th} and \c{(i + 1)'th}
    vertices of \a bottom, followed by the \c{(i + 1)'th} and \c{i'th}
    vertices of \a top.

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
    Q_D(QGLDisplayList);
    QGLPrimitive zipped = bottom.zippedWith(top);
    QVector3D norm = top.commonNormal() + bottom.commonNormal();
    zipped.setCommonNormal(norm);
    for (int i = 0; i < zipped.count() - 2; i += 2)
    {
        d->addTriangle(i, i+2, i+3, zipped);
        d->addTriangle(i, i+3, i+1, zipped);
        zipped.setCommonNormal(norm);
    }
}

/*!
    \internal
*/
void QGLDisplayListPrivate::adjustSectionNodes(QGLSection *sec,
                                       int offset, QGeometryData *geom)
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
int QGLDisplayListPrivate::adjustNodeTree(QGLSceneNode *top,
                                   int offset, QGeometryData *geom,
                                   QList<QGLSceneNode*> &deleted)
{
    Q_Q(QGLDisplayList);
    int totalItems = 0;
    if (top && !deleted.contains(top))
    {
        Q_ASSERT(geom);
        top->setStart(top->start() + offset);
        top->setGeometry(geom);
        totalItems = top->count();
        QList<QGLSceneNode*> children = top->childNodes();
        QList<QGLSceneNode*>::iterator it = children.begin();
        for ( ; it != children.end(); ++it)
        {
            totalItems += adjustNodeTree(*it, offset, geom, deleted);
        }
        if (totalItems == 0 && top->objectName().isEmpty())
        {
            top->disconnect(q, SLOT(deleteNode(QObject*)));
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
        QList<QGLSceneNode*> children = top->childNodes();
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

static inline void warnIgnore(int secCount, int vertCount, int nodeCount,
                              const char *msg)
{
    qWarning("Ignoring section %d with %d vertices and"
             " %d indexes - %s", secCount, vertCount, nodeCount, msg);
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
        end();
        QGeometryData *g = 0;
        QMap<quint32, QGeometryData *> geos;
        while (d->sections.count())
        {
            // pack sections that have the same fields into one geometry
            QGLSection *s = d->sections.takeFirst();
            QGLIndexArray indices = s->indices();
            int icnt = indices.size();
            int ncnt = nodeCount(s->nodes());
            int scnt = s->count();
            if (scnt == 0 || icnt == 0 || ncnt == 0)
            {
#ifndef QT_NO_DEBUG
                    if (ncnt == 0)
                        warnIgnore(scnt, icnt, ncnt, "nodes empty");
                    else if (scnt == 0)
                        warnIgnore(scnt, icnt, ncnt, "geometry count zero");
                    else
                        warnIgnore(scnt, icnt, ncnt, "index count zero");
#endif
                continue;
            }
            s->normalizeNormals();
            int sectionOffset = 0;
            int sectionIndexOffset = 0;
            QMap<quint32, QGeometryData *>::const_iterator it =
                    geos.constFind(s->fields());
            if (it != geos.constEnd())
            {
                g = it.value();
                sectionOffset = g->count();
                sectionIndexOffset = g->indexCount();
                g->appendGeometry(*s);
                for (int i = 0; i < icnt; ++i)
                    indices[i] += sectionOffset;
                g->appendIndices(indices);
            }
            else
            {
                g = new QGeometryData(*s);
                geos.insert(s->fields(), g);
            }
            d->adjustSectionNodes(s, sectionIndexOffset, g);
            delete s;
        }
        d->finalizeNeeded = false;
    }
}

/*!
    Creates a new section with smoothing mode set to \a smooth.  By default
    \a smooth is QGL::Smooth.  The new section is made
    current on this QGLDisplayList.  A section must be created before
    any geometry or new nodes can be added to the displaylist.

    The internal node stack is cleared, a section-level QGLSceneNode is
    created for this section by calling newNode().

    \sa newNode(), pushNode()
*/
void QGLDisplayList::newSection(QGL::Smoothing smooth)
{
    new QGLSection(this, smooth);  // calls addSection
}

void QGLDisplayList::addSection(QGLSection *sec)
{
    Q_D(QGLDisplayList);
    d->currentSection = sec;
    sec->setMapThreshold(d->defThreshold);
    d->sections.append(sec);
    d->nodeStack.clear();
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
    \internal
    Test function only.
*/
void QGLDisplayList::setDefaultThreshold(int t)
{
    Q_D(QGLDisplayList);
    d->defThreshold = t;
}

/*!
    Creates a new QGLSceneNode and makes it current.  A pointer to the new
    node is returned.  The node is added into the scene at the same level
    as the currentNode().

    The node is set to reference the geometry starting from the next
    vertex created, such that currentNode()->start() will return the
    index of this next vertex.

    Note: the display list is designed to handle deletion of the resulting
    QGLSceneNode instances by client code as gracefully as possible.

    Nonetheless the deletion of nodes in this way will have a performance
    impact and generally should be avoided.

    It is not necessary to clean up empty or unused nodes since these
    will be removed from the final scene during the finalize() function.

    newSection()
*/
QGLSceneNode *QGLDisplayList::newNode()
{
    Q_D(QGLDisplayList);
    Q_ASSERT(d->currentSection);
    QGLSceneNode *parentNode = this;
    if (d->nodeStack.count() > 0)
        parentNode = d->nodeStack.last();
    d->currentNode = new QGLSceneNode(geometry(), parentNode);
    d->currentNode->setPalette(parentNode->palette());
    d->currentNode->setStart(d->currentSection->indexCount());
    connect(d->currentNode, SIGNAL(destroyed(QObject*)),
            this, SLOT(deleteNode(QObject*)));
    if (d->nodeStack.count() == 0)
        d->currentSection->addNode(d->currentNode);
    return d->currentNode;
}

/*!
    \internal
    Private slot for handling nodes deleted externally.  This is done
    in a fairly naive inefficient way, but the assumption is that this
    is a very rare thing to happen, and this is really here just to
    make sure nothing crashes if nodes are deleted.
*/
void QGLDisplayList::deleteNode(QObject *object)
{
    QGLSceneNode *node = qobject_cast<QGLSceneNode*>(object);
    if (node)
    {
        Q_D(QGLDisplayList);
        d->nodeStack.removeOne(node);
        QList<QGLSection*>::iterator sx = d->sections.begin();
        for ( ; sx != d->sections.end(); ++sx)
        {
            QGLSection *s = *sx;
            if (s->deleteNode(node))
                break;
        }
    }
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
    Q_ASSERT(d->currentSection);
    QGLSceneNode *parentNode = d->currentNode;
    d->nodeStack.append(parentNode);
    d->currentNode = new QGLSceneNode(geometry(), parentNode);
    d->currentNode->setStart(d->currentSection->indexCount());
    d->currentNode->setPalette(parentNode->palette());
    return d->currentNode;
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
QGLSceneNode *QGLDisplayList::popNode()
{
    Q_D(QGLDisplayList);
    int cnt = d->currentSection->indexCount();
    QGLSceneNode *s = d->nodeStack.takeLast();
    QGLSceneNode *parentNode = this;
    if (d->nodeStack.count() > 0)
        parentNode = d->nodeStack.last();
    d->currentNode = s->clone(parentNode);
    d->currentNode->setChildNodes(QList<QGLSceneNode*>());
    d->currentNode->setStart(cnt);
    d->currentNode->setCount(0);
    d->currentNode->setPalette(parentNode->palette());
    if (d->nodeStack.count() == 0)
        d->currentSection->addNode(d->currentNode);
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

/*!
    Returns the current operation, as set by begin().  Data added
    for example by addVertex() will be accumulated for use in this
    operation.
*/
QGL::Operation QGLDisplayList::currentOperation() const
{
    Q_D(const QGLDisplayList);
    return d->operation;
}

/*!
    Sets the current \a operation.  After this call all displaylist data
    add functions, for example addVertex(), will accumulate data for that
    \a operation.  Any previous operation is ended.

    \sa end()
*/
void QGLDisplayList::begin(QGL::Operation operation)
{
    Q_D(QGLDisplayList);
    end();
    d->currentOperation = new QGLPrimitive();
    d->operation = operation;
}

/*!
    Completes the current operation set with begin(), posting the data
    to the display lists internal store and clearing the operation's
    data structures.
*/
void QGLDisplayList::end()
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
    {
        switch (d->operation)
        {
        case QGL::NO_OP:
            break;
        case QGL::TRIANGLE:
            addTriangle(*d->currentOperation); break;
        case QGL::TRIANGLE_STRIP:
            addTriangleStrip(*d->currentOperation); break;
        case QGL::QUAD:
            addQuad(*d->currentOperation); break;
        case QGL::QUAD_STRIP:
            addQuadStrip(*d->currentOperation); break;
        case QGL::TRIANGLE_FAN:
            addTriangleFan(*d->currentOperation); break;
        case QGL::TRIANGULATED_FACE:
            addTriangulatedFace(*d->currentOperation); break;
        case QGL::QUADS_ZIPPED:
            if (d->primitiveQueue.count() < 1)
            {
                qWarning("QUADS_ZIPPED mode - use QGL::NEXT_PRIMITIVE flag"
                         " to specify bottom array");
            }
            else
            {
                addQuadsZipped(*d->primitiveQueue.last(), *d->currentOperation);
            }
        }
        if (d->primitiveQueue.count())
        {
            qDeleteAll(d->primitiveQueue);
            d->primitiveQueue.clear();
        }
        if (!(d->currentOperation->flags() & QGL::RETAIN_PRIMITIVE))
        {
            delete d->currentOperation;
            d->currentOperation = 0;
            d->operation = QGL::NO_OP;
        }
    }
}

/*!
    Sets the the current operation's internal QGLPrimitive to have \a flags.
    Also, if \a flags is QGL::NEXT_PRIMITIVE creates a new internal QGLPrimitive
    for a multi-primitve operation, such QGL::QUADS_ZIPPED.

    \sa QGLPrimitive::setFlags(), flags()
*/
void QGLDisplayList::setFlags(QGL::OperationFlags flags)
{
    Q_D(QGLDisplayList);
    if (flags & QGL::NEXT_PRIMITIVE)
    {
        if (d->currentOperation)
        {
            QGLPrimitive *next = new QGLPrimitive();
            next->setFlags(flags);
            d->primitiveQueue.append(d->currentOperation);
            d->currentOperation = next;
        }
#ifndef QT_NO_DEBUG
        else
        {
            qWarning("Add data to the first primitive before QGL::NEXT_PRIMITIVE");
        }
#endif
    }
    else
    {
        if (d->currentOperation)
            d->currentOperation->setFlags(flags);
    }
}

/*!
    Returns the current operation primitives flags.

    \sa QGLPrimitive::flags(), setFlags()
*/
QGL::OperationFlags QGLDisplayList::flags() const
{
    Q_D(const QGLDisplayList);
    if (d->currentOperation)
        return d->currentOperation->flags();
    return 0;
}

/*!
    Adds the \a vertex to the current operation.
*/
void QGLDisplayList::addVertex(const QVector3D &vertex)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->appendVertex(vertex);
}

/*!
    \fn void QGLDisplayList::addVertex(qreal x, qreal y)
    Adds the vertex (\a x, \a y, 0) to the current operation.
*/

/*!
    \fn void QGLDisplayList::addVertex(qreal x, qreal y, qreal z)
    Adds the vertex (\a x, \a y, \a z) to the current operation.
*/

/*!
    Adds the \a normal to the current operation.
*/
void QGLDisplayList::addNormal(const QVector3D &normal)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->appendNormal(normal);
}

/*!
    Adds the \a color to the current operation.
*/
void QGLDisplayList::addColor(const QColor4B &color)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->appendColor(color);
}

/*!
    Adds the texture coordinate \a texCoord into the field \a attr for the current operation.
*/
void QGLDisplayList::addTexCoord(const QVector2D &texCoord, QGL::VertexAttribute attr)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->appendTexCoord(texCoord, attr);
}

/*!
    \fn void QGLDisplayList::addTexCoord(qreal s, qreal t, QGL::VertexAttribute attr)
    \overload
    Adds the texture coordinate \a s, \a t into the field \a attr for the current operation.
*/

/*!
    Adds the attribute \a value into the field \a attr for the current operation.
*/
void QGLDisplayList::addAttribute(const QVector3D &value, QGL::VertexAttribute attr)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->appendAttribute(value, attr);
}

/*!
    Adds all the \a vertices to the current operation.
*/
void QGLDisplayList::addVertexArray(const QVector3DArray &vertices)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->appendVertexArray(vertices);
}

/*!
    Adds all the \a normals to the current operation.
*/
void QGLDisplayList::addNormalArray(const QVector3DArray &normals)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->appendNormalArray(normals);
}

/*!
    Adds all the \a colors to the current operation.
*/
void QGLDisplayList::addColorArray(const QArray<QColor4B> &colors)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->appendColorArray(colors);
}

/*!
    Adds all the texture coordinates in \a texCoords to the field \a attr for the current operation.
*/
void QGLDisplayList::addTexCoordArray(const QVector2DArray &texCoords, QGL::VertexAttribute attr)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->appendTexCoordArray(texCoords, attr);
}

/*!
    Adds all the attribute \a values to the field \a attr for the current operation.

*/
void QGLDisplayList::addAttributeArray(const QCustomDataArray &values, QGL::VertexAttribute attr)
{
    Q_D(QGLDisplayList);
    if (d->currentOperation)
        d->currentOperation->appendAttributeArray(values, attr);
}
