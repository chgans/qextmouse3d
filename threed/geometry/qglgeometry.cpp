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

#include "qglgeometry.h"
#include "qglgeometry_p.h"
#include "qglpainter.h"
#include "qglmaterialparameters.h"
#include "qglmaterialcollection.h"

#include <QtCore/qlist.h>
#include <QtCore/qpair.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLGeometry
    \brief The QGLGeometry class provides storage for geometry meshes.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::geometry

    QGLGeometry provides facilities for applications to build
    up sets of triangles, quads, lines, etc, for drawing with
    QGLPainter::draw().  The geometry can be built on the fly while
    objects are being drawn, or built ahead of time and then cached
    until needed.

    The following example builds up a set of triangles:

    \code
    QGLGeometry geom;
    geom.setDrawingMode(QGL::Triangles);
    geom.setVertexArray(vertices);

    QGLPainter painter(this);
    geom.draw(&painter);
    \endcode

    Subclasses of QGLGeometry can specialize for certain kinds of
    objects: cubes, spheres, teapots, etc.  The constructors for
    the subclasses populate the vertexArray() of QGLGeometry
    with the actual geometry.  For example, the following draws a
    unit cube at the origin:

    \code
    QGLCube cube;
    cube.draw(&painter);
    \endcode

    QGLGeometry objects can be cached in the GL server as vertex
    buffers, which allows them to be more quickly rendered.
    Vertex buffers are uploaded to the GL server by calling upload():

    \code
    cube.upload();
    \endcode

    If the number of vertices in the QGLGeometry object is
    greater than or equal to bufferThreshold(), then a vertex
    buffer will be created automatically the first time draw()
    is called.

    \sa QGLCube, QGLBezierGeometry
*/

/*!
    Constructs a new geometry object with the given \a parent.
*/
QGLGeometry::QGLGeometry(QObject *parent)
        : QObject(*new QGLGeometryPrivate, parent)
        , mMaterial(-1)
        , mPalette(0)
{
}

/*!
    \a internal
    Constructs a new geometry object with \a parent and \a dd private
    data.  This is for use by QGLGeometry sub-classes that use the
    QObjectPrivate heierarchy.
*/
QGLGeometry::QGLGeometry(QGLGeometryPrivate &dd, QObject *parent)
    : QObject(dd, parent)
    , mMaterial(-1)
    , mPalette(0)
{
}

/*!
    Destroys this geometry object.
*/
QGLGeometry::~QGLGeometry()
{
}

/*!
    Returns the drawing mode to use for this geometry object.
    The default value is QGL::NoDrawingMode.

    \sa setDrawingMode()
*/
QGL::DrawingMode QGLGeometry::drawingMode() const
{
    Q_D(const QGLGeometry);
    return d->drawingMode;
}

/*!
    Sets the drawing mode for this geometry object to \a value.

    \sa drawingMode()
*/
void QGLGeometry::setDrawingMode(QGL::DrawingMode value)
{
    Q_D(QGLGeometry);
    d->drawingMode = value;
}

/*!
    Returns the vertex array for this geometry object.

    \sa setVertexArray(), indexArray()
*/
QGLVertexArray QGLGeometry::vertexArray() const
{
    Q_D(const QGLGeometry);
    return d->vertexArray;
}

/*!
    Sets the vertex \a array for this geometry object.

    \sa vertexArray(), setIndexArray(), isModified()
*/
void QGLGeometry::setVertexArray(const QGLVertexArray& array)
{
    Q_D(QGLGeometry);
    d->vertexArray = array;
    d->modified = true;
}

/*!
    Returns the index array that is used when drawing.  Its
    elements correspond to offsets within the vertex arrays of
    the vertices to use.

    The default value is an empty array, which indicates that all
    of the vertices in vertexArray() should be used for drawing.
    
    \sa setIndexArray(), vertexArray()
*/
QGLIndexArray QGLGeometry::indexArray() const
{
    Q_D(const QGLGeometry);
    return d->indexArray;
}

/*!
    Sets the index array that is used for drawing to \a array.
    Its elements correspond to offsets within the vertex arrays of
    the vertices to use.

    If \a array is an empty array, then all of the vertices in
    vertexArray() will be used for drawing.

    \sa indexArray(), setVertexArray(), isModified()
*/
void QGLGeometry::setIndexArray(const QGLIndexArray& array)
{
    Q_D(QGLGeometry);
    d->indexArray = array;
    d->modified = true;
}

/*!
    Returns the threshold for determining if the geometry should
    be uploaded to the GL server as a vertex buffer.

    Geometries with less than this many vertices will be drawn from
    client-side buffers.  Geometries with this many or more vertices
    will be uploaded to the GL server the first time they are drawn.

    The default value is 32.  If the value is zero, then the geometry
    will always be uploaded.

    \sa setBufferThreshold(), draw(), upload()
*/
int QGLGeometry::bufferThreshold() const
{
    Q_D(const QGLGeometry);
    return d->bufferThreshold;
}

/*!
    Sets the threshold for determining if the geometry should
    be uploaded to the GL server as a vertex buffer to \a value.

    Geometries with less than \a value vertices will be drawn from
    client-side buffers.  Geometries with \a value or more vertices
    will be uploaded to the GL server the first time they are drawn.

    Specifying a \a value of zero will cause the geometry to be
    always uploaded to the GL server.

    \sa bufferThreshold(), draw(), upload()
*/
void QGLGeometry::setBufferThreshold(int value)
{
    Q_D(QGLGeometry);
    d->bufferThreshold = value;
}

/*!
    Returns the axis-aligned box that bounds this object.  This box
    is used for object culling within draw() to quickly determine
    that the object should not be drawn.

    The default value of a null box indicates that boundary checks
    should not performed and the object always drawn.

    \sa draw(), setBoundingBox()
*/
QBox3D QGLGeometry::boundingBox() const
{
    Q_D(const QGLGeometry);
    return d->boundingBox;
}

/*!
    Sets the axis-aligned box that bounds this object to \a value.
    The box \a value is used for object culling within draw() to quickly
    determine that the object should not be drawn.

    If \a value is a null box, then boundary checks will not performed
    and the object will always be drawn.

    \sa draw(), boundingBox()
*/
void QGLGeometry::setBoundingBox(const QBox3D& value)
{
    Q_D(QGLGeometry);
    d->boundingBox = value;
}

/*!
    Draws the geometry within this object on \a painter.

    This is a convenience method which simply calls
    draw(QGLPainter *painter, int start, int count) with both start and
    count set to 0.
*/
void QGLGeometry::draw(QGLPainter *painter)
{
    draw(painter, 0, 0);
}

/*!
    Draws the geometry within this object on \a painter.

    Drawing starts at the \a start vertex, and \a count vertices are drawn.
    If count is 0 then all vertices are drawn.

    If upload() has been called, or the number of vertices is greater
    than or equal to bufferThreshold(), then server-side vertex
    and index buffers will be used to draw the geometry.

    Subclasses may implement their own caching or geometry
    subdivision policy by overriding draw().  The isModified() function
    can be used from the subclass to determine if the vertexArray()
    or indexArray() values have changed since the last call to draw().

    If a material has been set for this geometry with setMaterial(), then
    this is used to render the front faces.

    \sa upload(), isModified()
*/
void QGLGeometry::draw(QGLPainter *painter, int start, int count)
{
    Q_D(QGLGeometry);

    // Performing a bounding box check first.
    if (!d->boundingBox.isNull() && !painter->isVisible(d->boundingBox))
        return;

    const QGLMaterialParameters *save = 0;
    if (mPalette && mMaterial != -1)
    {
        save = painter->faceMaterial(QGL::FrontFaces);
        painter->setFaceMaterial(QGL::FrontFaces,
                                 mPalette->materialByIndex(mMaterial));
        QGLTexture2D *tex = mPalette->texture(mMaterial);
        if (tex)
            painter->setTexture(tex);
    }

    // Determine if we need to recreate the vertex buffers in the GL server.
    // If the last upload attempt failed, then don't try again.
    if (d->uploadState) {
        if (!d->vertexArray.isUploaded()) {
            if (d->vertexArray.vertexCount() >= d->bufferThreshold)
                d->uploadState = upload();
        } else if (d->modified) {
            d->uploadState = upload();
        }
    }
    d->modified = false;

    // Draw the geometry.
    painter->setVertexArray(d->vertexArray);
    if (d->indexArray.isEmpty()) {
        if (count == 0)
            count = d->vertexArray.vertexCount();
        painter->draw(d->drawingMode, count, start);
    } else {
        if (count == 0)
            count = d->indexArray.size();
        painter->draw(d->drawingMode, d->indexArray, start, count);
    }

    if (save)
        painter->setFaceMaterial(QGL::FrontFaces, save);

    // TODO: this is a possible performance hit.  If a lot of scene nodes
    // reference the same material & texture, the painter->setTexture(tex)
    // call above will happen for every node, and that call is expensive
    // even if the same texture was already active.  Ideal solution is
    // for the painter->setTexture() call to be cheap if setting a texture
    // that is already bound.  This setting texture to null is needed in
    // the case where textured geometry is mixed with geometry that is
    // lit materials (for example) otherwise texture bleeding occurs.
    painter->setTexture((QGLTexture2D*)0);
}

/*!
    Uploads this geometry object to the GL server as static vertex
    and index buffers for quicker drawing with draw().  Returns
    true if the geometry was uploaded; false if the GL server does
    not support vertex and index buffers.

    This function must be called with a current QGLContext.
    The same QGLContext must be used later with draw().

    This function will be called automatically by the default implementation
    of draw() if the number of vertices in vertexArray() is greater than
    or equal to bufferThreshold().  Calling upload() explicitly allows
    the geometry to be uploaded ahead of time before the first draw(),
    regardless of the setting of bufferThreshold().

    \sa draw(), bufferThreshold()
*/
bool QGLGeometry::upload()
{
    static bool haveVBOs = true;

    Q_D(QGLGeometry);

    // If we already have buffers, then bail out if the geometry is unchanged.
    if (d->vertexArray.isUploaded() && !d->modified)
        return true;

    // If we know that vertex buffers are not supported, then stop now.
    if (!haveVBOs)
        return false;

    // Try to create the vertex buffer.
    if (!d->vertexArray.upload()) {
        if (haveVBOs) {
            qWarning() << "QGLGeometry: vertex buffer objects are not "
                          "supported by the GL server";
        }
        haveVBOs = false;
        return false;
    }

    // Create the index buffer if necessary.
    if (!d->indexArray.upload()) {
        if (haveVBOs) {
            qWarning() << "QGLGeometry: index buffer objects are not "
                          "supported by the GL server";
        }
        haveVBOs = false;
        return false;
    }

    // The buffers have been created in the GL server.
    return true;
}

/*!
    Returns true if setVertexArray() or setIndexArray() was called
    since the last call to draw() or isModified(); false otherwise.
    
    This function is intended for use in subclasses that override
    draw() to determine if the geometry has changed since the last
    time it was drawn.

    \sa setVertexArray(), setIndexArray()
*/
bool QGLGeometry::isModified() const
{
    Q_D(const QGLGeometry);

    bool modified = d->modified;
    d->modified = false;
    return modified;
}

/*!
    \fn int QGLGeometry::material() const
    Returns the material index for this geometry, or -1
    if this node has no material specified.

    The default value is -1.

    The value returned is an index into the associated
    QGLMaterialCollection.

    This material is applied uniformly to the geometry.  However sub-classes
    may use it that way, or implement their own material handling using this
    material as a default or base color.

    \sa setMaterial()
*/

/*!
    \fn void QGLGeometry::setMaterial(int material)
    Sets the material index associated with this geometry to be \a material.
    The \a material is interpreted as an offset into the associated
    QGLMaterialCollection.

    \sa material()
*/

/*!
    \fn QGLMaterialCollection *QGLGeometry::palette() const
    Returns the QGLMaterialCollection associated with this geometry, or
    null (0) if no collection is associated.  The default is null.

    \sa setPalette()
*/

/*!
    \fn void QGLGeometry::setPalette(QGLMaterialCollection *palette)
    Sets the QGLMaterialCollection associated with this geometry to be \a palette.
    This geometry object does not take ownership of the palette.
*/

QT_END_NAMESPACE
