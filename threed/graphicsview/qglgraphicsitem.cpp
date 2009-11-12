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

#include "qglgraphicsitem.h"
#include "qglpainter.h"
#include <QtGui/qpainter.h>
#include <QtGui/qgraphicsscene.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLGraphicsItem
    \brief The QGLGraphicsItem class provides a rectangular item for arbitrary OpenGL painting.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::graphicsview
*/

class QGLGraphicsItemPrivate
{
public:
    QGLGraphicsItemPrivate() {}
    QGLGraphicsItemPrivate(const QRectF& r) : rect(r) {}

    QRectF rect;
};

/*!
    Constructs a new GL graphics item and attaches it to \a parent.
*/
QGLGraphicsItem::QGLGraphicsItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    d = new QGLGraphicsItemPrivate();
}

/*!
    Constructs a new GL graphics item and attaches it to \a parent.
    The boundingRect() is initially set to \a rect.
*/
QGLGraphicsItem::QGLGraphicsItem(const QRectF& rect, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    d = new QGLGraphicsItemPrivate(rect);
}

/*!
    Constructs a new GL graphics item and attaches it to \a parent.
    The boundingRect() is initially set to (\a x, \a y, \a w, \a h).
*/
QGLGraphicsItem::QGLGraphicsItem
        (qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    d = new QGLGraphicsItemPrivate(QRectF(x, y, w, h));
}

/*!
    Destroys this GL graphics item.
*/
QGLGraphicsItem::~QGLGraphicsItem()
{
    delete d;
}

/*!
    Returns the rectangular area of the view that is occupied by
    this GL graphics item.

    \sa setRect(), boundingRect()
*/
QRectF QGLGraphicsItem::rect() const
{
    return d->rect;
}

/*!
    Sets the rectangular area of the view that is occupied by
    this GL graphics item to \a rect.

    \sa rect(), boundingRect()
*/
void QGLGraphicsItem::setRect(const QRectF &rect)
{
    if (d->rect == rect)
        return;
    prepareGeometryChange();
    d->rect = rect;
    update();
}

/*!
    \fn void QGLGraphicsItem::setRect(qreal x, qreal y, qreal w, qreal h)
    \overload

    Sets the rectangular area of the view that is occupied by
    this GL graphics item to (\a x, \a y, \a w, \a h).

    \sa rect(), boundingRect()
*/

/*!
    Returns the bounding rectangle for this GL graphics item,
    which is the same as rect().

    \sa rect()
*/
QRectF QGLGraphicsItem::boundingRect() const
{
    return d->rect;
}

/*!
    Returns the combination of the rotation, scale, and translation
    properties on this graphics item in 3D space.

    The graphics item's transformOrigin() is used as the center of
    the transformation.  The transformOrigin() only includes x and y
    components, so z will be assumed to be zero.

    The graphics item's scaleX() and scaleY() are used as the
    scaling factors for x and y.  The z scaling factor will be
    the average of the x and y factors, which will result in
    uniform scaling in all directions when scaleX() is the
    same as scaleY().

    Axis rotations will be applied in the order Z, Y, and then X.
*/
QMatrix4x4 QGLGraphicsItem::transform3D() const
{
    QMatrix4x4 m;
#if 0   // FIXME
    QVector3D origin(transformOrigin());
    m.translate(origin);
    m.rotate(xRotation(), QVector3D(1, 0, 0));
    m.rotate(yRotation(), QVector3D(0, 1, 0));
    m.rotate(zRotation(), QVector3D(0, 0, 1));
    qreal xscale = xScale();
    qreal yscale = yScale();
    qreal zscale = (xscale + yscale) / 2.0f;
    m.scale(xscale, yscale, zscale);
    m.translate(-origin);
#endif
    return m;
}

/*!
    Paints this GL graphics item onto \a painter, with the specified
    \a option and \a widget parameters.

    This override creates a QGLPainter for \a painter and passes
    it to paintGL().

    The projection and modelview matrices for the QGLPainter will
    be set to match the settings on \a painter.  Drawing GL fragments
    will use the standard 2D transformation.

    Rotations about the Z axis will be correct in 3D space.  Rotations
    around the X and Y axes will not be correct in 3D space, because
    QTransform converts X and Y axis rotations into projective shearing
    transformations rather than true 3D rotations.

    The transform3D() function can be used to obtain the graphics
    item rotation, scale, and translation properties in a form suitable
    for objects in 3D space.  Rotations around the X, Y, and Z axes
    will be correct when transform3D() is used.

    \sa paintGL()
*/
void QGLGraphicsItem::paint
    (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (d->rect.isEmpty())
        return;

    // Initialize a QGLPainter for the surface and bail out if not active.
    QGLPainter glpainter;
    if (!glpainter.begin(painter)) {
        qWarning("GL graphics system is not active; cannot use 3D items");
        return;
    }

    // Set up the GL viewport to limit drawing to the bounds of this item.
    QRect viewport = painter->deviceTransform().mapRect(rect()).toRect();
    glpainter.setViewport(viewport);

    // Set the projection matrix and modelview matrix in the QGLPainter
    // to match the underlying QPainter transformation.
    glpainter.projectionMatrix().setToIdentity();
    glpainter.projectionMatrix().ortho(viewport);
    glpainter.modelViewMatrix() = QMatrix4x4(painter->deviceTransform());

    // Paint the GL contents.
    paintGL(&glpainter, option, widget);

    // Disable the current drawing effect so that QGLPainter will
    // forcibly update the GL context the next time QGLPainter is used.
    glpainter.disableEffect();
}

/*!
    \fn void QGLGraphicsItem::paintGL(QGLPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)

    Paints this GL graphics item onto \a painter, with the specified
    \a option and \a widget parameters.  This function is called
    from paint().

    \sa paint()
*/

QT_END_NAMESPACE
