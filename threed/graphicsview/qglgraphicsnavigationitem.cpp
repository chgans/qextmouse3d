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

#include "qglgraphicsnavigationitem.h"
#include "qglgraphicsviewportitem.h"
#include "qglcamera.h"
#include <QtGui/qapplication.h>
#include <QtGui/qevent.h>
#include <QtGui/qgraphicssceneevent.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLGraphicsNavigationItem
    \brief The QGLGraphicsNavigationItem class provides navigation logic for a QGLGraphicsViewportItem
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::graphicsview

    Navigation is separate from object display so that different
    navigation strategies can be applied to a viewport item.

    \sa QGLGraphicsViewportItem
*/

class QGLGraphicsNavigationItemPrivate
{
public:
    QGLGraphicsNavigationItemPrivate()
        : viewportItem(0),
          filterInstalled(false),
          panning(false),
          startPan(-1, -1),
          pressedObject(0),
          pressedButton(Qt::NoButton),
          enteredObject(0)
    {
    }

    QGLGraphicsViewportItem *viewportItem;
    bool filterInstalled;
    bool panning;
    QPointF startPan;
    QObject *pressedObject;
    Qt::MouseButton pressedButton;
    QObject *enteredObject;

    bool handleMousePress(QGraphicsSceneMouseEvent *e);
    bool handleMouseRelease(QGraphicsSceneMouseEvent *e);
    bool handleMouseDoubleClick(QGraphicsSceneMouseEvent *e);
    bool handleMouseMove(QGraphicsSceneMouseEvent *e);
    bool handleWheel(QGraphicsSceneWheelEvent *e);
    bool handleKeyPress(QKeyEvent *e);
    bool handleHover(QGraphicsSceneHoverEvent *e);
    bool handleHoverLeave();

    void sendEnterEvent(QObject *object)
    {
        QEvent event(QEvent::Enter);
        QCoreApplication::sendEvent(object, &event);
    }

    void sendLeaveEvent(QObject *object)
    {
        QEvent event(QEvent::Leave);
        QCoreApplication::sendEvent(object, &event);
    }

    void wheel(qreal delta);
    void pan(qreal deltax, qreal deltay);
    void rotate(qreal deltax, qreal deltay);
    QPointF viewDelta(qreal deltax, qreal deltay);
};

/*!
    Constructs a new navigation item and attaches it to \a parent.

    \sa setViewportItem()
*/
QGLGraphicsNavigationItem::QGLGraphicsNavigationItem(QGraphicsItem *parent)
    : QGraphicsItem(parent), d_ptr(new QGLGraphicsNavigationItemPrivate())
{
    setFlag(ItemHasNoContents, true);
}

/*!
    Constructs a new navigation item for \a viewportItem and attaches
    it to \a parent.
*/
QGLGraphicsNavigationItem::QGLGraphicsNavigationItem
        (QGLGraphicsViewportItem *viewportItem, QGraphicsItem *parent)
    : QGraphicsItem(parent), d_ptr(new QGLGraphicsNavigationItemPrivate())
{
    setViewportItem(viewportItem);
    setFlag(ItemHasNoContents, true);
}

/*!
    Destroys this navigation item.
*/
QGLGraphicsNavigationItem::~QGLGraphicsNavigationItem()
{
}

/*!
    Returns the viewport item that will be affected when this
    navigation item moves the camera.

    \sa setViewportItem()
*/
QGLGraphicsViewportItem *QGLGraphicsNavigationItem::viewportItem() const
{
    Q_D(const QGLGraphicsNavigationItem);
    return d->viewportItem;
}

/*!
    Sets the viewport item that will be affected when this
    navigation item moves the camera to \a item.

    \sa viewportItem()
*/
void QGLGraphicsNavigationItem::setViewportItem(QGLGraphicsViewportItem *item)
{
    Q_D(QGLGraphicsNavigationItem);
    if (d->viewportItem != item) {
        if (d->viewportItem && d->filterInstalled)
            d->viewportItem->removeSceneEventFilter(this);
        d->viewportItem = item;
        d->viewportItem->setAcceptHoverEvents(true); // We need HoverLeave.
        if (!d->filterInstalled && scene())
            d->filterInstalled = true;  // The item now has a scene.
        if (d->viewportItem && d->filterInstalled)
            d->viewportItem->installSceneEventFilter(this);
    }
}

/*!
    \internal
*/
QRectF QGLGraphicsNavigationItem::boundingRect() const
{
    return QRectF();
}

/*!
    \internal
*/
void QGLGraphicsNavigationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

/*!
    \internal
*/
bool QGLGraphicsNavigationItem::sceneEventFilter
        (QGraphicsItem *watched, QEvent *event)
{
    Q_D(QGLGraphicsNavigationItem);

    if (watched != d->viewportItem || !d->viewportItem)
        return false;

    switch (event->type()) {

    case QEvent::GraphicsSceneMouseMove:
        return d->handleMouseMove(static_cast<QGraphicsSceneMouseEvent *>(event));

    case QEvent::GraphicsSceneMousePress:
        return d->handleMousePress(static_cast<QGraphicsSceneMouseEvent *>(event));

    case QEvent::GraphicsSceneMouseRelease:
        return d->handleMouseRelease(static_cast<QGraphicsSceneMouseEvent *>(event));

    case QEvent::GraphicsSceneMouseDoubleClick:
        return d->handleMouseDoubleClick(static_cast<QGraphicsSceneMouseEvent *>(event));

    case QEvent::GraphicsSceneWheel:
        return d->handleWheel(static_cast<QGraphicsSceneWheelEvent *>(event));

    case QEvent::KeyPress:
        return d->handleKeyPress(static_cast<QKeyEvent *>(event));

    case QEvent::GraphicsSceneHoverEnter:
    case QEvent::GraphicsSceneHoverMove:
        return d->handleHover(static_cast<QGraphicsSceneHoverEvent *>(event));

    case QEvent::GraphicsSceneHoverLeave:
        return d->handleHoverLeave();

    default: break;
    }

    return false;
}

/*!
    \internal
*/
QVariant QGLGraphicsNavigationItem::itemChange
        (GraphicsItemChange change, const QVariant &value)
{
    Q_D(QGLGraphicsNavigationItem);
    if (change == QGraphicsItem::ItemSceneHasChanged &&
            !d->filterInstalled && d->viewportItem) {
        d->filterInstalled = true;
        d->viewportItem->installSceneEventFilter(this);
    }
    return QGraphicsItem::itemChange(change, value);
}

bool QGLGraphicsNavigationItemPrivate::handleMousePress(QGraphicsSceneMouseEvent *e)
{
    QObject *object;
    if (!panning && (viewportItem->options() &
                        QGLGraphicsViewportItem::ObjectPicking) != 0)
        object = viewportItem->objectForPosition(e->pos());
    else
        object = 0;
    if (pressedObject) {
        // Send the press event to the pressed object.  Use a position
        // of (0, 0) if the mouse is still within the pressed object,
        // or (-1, -1) if the mouse is no longer within the pressed object.
        QMouseEvent event
            (QEvent::MouseButtonPress,
             (pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
             e->screenPos(), e->button(), e->buttons(), e->modifiers());
        QCoreApplication::sendEvent(pressedObject, &event);
    } else if (object) {
        // Record the object that was pressed and forward the event.
        pressedObject = object;
        enteredObject = 0;
        pressedButton = e->button();

        // Send a mouse press event for (0, 0).
        QMouseEvent event(QEvent::MouseButtonPress, QPoint(0, 0),
                          e->screenPos(), e->button(), e->buttons(),
                          e->modifiers());
        QCoreApplication::sendEvent(object, &event);
    } else if ((viewportItem->options() &
                        QGLGraphicsViewportItem::CameraNavigation) != 0 &&
                    e->button() == Qt::LeftButton) {
        panning = true;
        startPan = e->pos();
#ifndef QT_NO_CURSOR
        viewportItem->setCursor(Qt::ClosedHandCursor);
#endif
    } else {
        return false;
    }
    return true;
}

bool QGLGraphicsNavigationItemPrivate::handleMouseRelease(QGraphicsSceneMouseEvent *e)
{
    if (panning && e->button() == Qt::LeftButton) {
        panning = false;
#ifndef QT_NO_CURSOR
        viewportItem->unsetCursor();
#endif
    }
    if (pressedObject) {
        // Notify the previously pressed object about the release.
        QObject *object = viewportItem->objectForPosition(e->pos());
        QObject *pressed = pressedObject;
        if (e->button() == pressedButton) {
            pressedObject = 0;
            pressedButton = Qt::NoButton;
            enteredObject = object;

            // Send the release event to the pressed object.  Use a position
            // of (0, 0) if the mouse is still within the pressed object,
            // or (-1, -1) if the mouse is no longer within the pressed object.
            QMouseEvent event
                (QEvent::MouseButtonRelease,
                 (pressed == object) ? QPoint(0, 0) : QPoint(-1, -1),
                 e->screenPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(pressed, &event);

            // Send leave and enter events if necessary.
            if (object != pressed) {
                sendLeaveEvent(pressed);
                if (object)
                    sendEnterEvent(object);
            }
        } else {
            // Some other button than the original was released.
            // Forward the event to the pressed object.
            QMouseEvent event
                (QEvent::MouseButtonRelease,
                 (pressed == object) ? QPoint(0, 0) : QPoint(-1, -1),
                 e->screenPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(pressed, &event);
        }
    }
    return true;
}

bool QGLGraphicsNavigationItemPrivate::handleMouseDoubleClick(QGraphicsSceneMouseEvent *e)
{
    if ((viewportItem->options() &
                QGLGraphicsViewportItem::ObjectPicking) != 0) {
        QObject *object = viewportItem->objectForPosition(e->pos());
        if (object) {
            // Simulate a double click event for (0, 0).
            QMouseEvent event
                (QEvent::MouseButtonDblClick, QPoint(0, 0),
                 e->screenPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(object, &event);
        }
        return true;
    } else {
        return false;
    }
}

bool QGLGraphicsNavigationItemPrivate::handleMouseMove(QGraphicsSceneMouseEvent *e)
{
    if (panning) {
        QPointF delta = e->pos() - startPan;
        startPan = e->pos();
        if ((e->modifiers() & Qt::ControlModifier) != 0)
            wheel(delta.y() * -60);
        else if ((e->modifiers() & Qt::ShiftModifier) != 0)
            pan(delta.x(), delta.y());
        else
            rotate(delta.x(), delta.y());
    } else if ((viewportItem->options() &
                    QGLGraphicsViewportItem::ObjectPicking) != 0) {
        QObject *object = viewportItem->objectForPosition(e->pos());
        if (pressedObject) {
            // Send the move event to the pressed object.  Use a position
            // of (0, 0) if the mouse is still within the pressed object,
            // or (-1, -1) if the mouse is no longer within the pressed object.
            QMouseEvent event
                (QEvent::MouseMove,
                 (pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
                 e->screenPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(pressedObject, &event);
        } else if (object) {
            if (object != enteredObject) {
                if (enteredObject)
                    sendLeaveEvent(enteredObject);
                enteredObject = object;
                sendEnterEvent(enteredObject);
            }
            QMouseEvent event
                (QEvent::MouseMove, QPoint(0, 0),
                 e->screenPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(object, &event);
        } else if (enteredObject) {
            sendLeaveEvent(enteredObject);
            enteredObject = 0;
        }
    } else {
        return false;
    }
    return true;
}

bool QGLGraphicsNavigationItemPrivate::handleWheel(QGraphicsSceneWheelEvent *e)
{
    if ((viewportItem->options() &
            QGLGraphicsViewportItem::CameraNavigation) != 0) {
        wheel(e->delta());
        return true;
    } else {
        return false;
    }
}

bool QGLGraphicsNavigationItemPrivate::handleKeyPress(QKeyEvent *e)
{
    QGLCamera *camera;
    qreal sep;

    if ((viewportItem->options() &
            QGLGraphicsViewportItem::CameraNavigation) == 0) {
        return false;
    }

    switch (e->key()) {

        case Qt::Key_Left:
        {
            if ((e->modifiers() & Qt::ShiftModifier) != 0) {
                pan(-10, 0);
            } else if ((e->modifiers() & Qt::ControlModifier) != 0) {
                camera = viewportItem->camera();
                sep = camera->eyeSeparation();
                sep -= (sep / 10.0f);
                if (sep < 0.0f)
                    sep = 0.0f;
                camera->setEyeSeparation(sep);
                e->accept();
            } else {
                rotate(-10, 0);
            }
        }
        break;

        case Qt::Key_Right:
        {
            if ((e->modifiers() & Qt::ShiftModifier) != 0) {
                pan(10, 0);
            } else if ((e->modifiers() & Qt::ControlModifier) != 0) {
                camera = viewportItem->camera();
                sep = camera->eyeSeparation();
                sep += (sep / 10.0f);
                camera->setEyeSeparation(sep);
                e->accept();
                return true;
            } else {
                rotate(10, 0);
            }
        }
        break;

        case Qt::Key_Up:
        {
            if ((e->modifiers() & Qt::ControlModifier) != 0)
                wheel(120);
            else if ((e->modifiers() & Qt::ShiftModifier) != 0)
                pan(0, -10);
            else
                rotate(0, -10);
        }
        break;

        case Qt::Key_Down:
        {
            if ((e->modifiers() & Qt::ControlModifier) != 0)
                wheel(-120);
            else if ((e->modifiers() & Qt::ShiftModifier) != 0)
                pan(0, 10);
            else
                rotate(0, 10);
        }
        break;

        default: return false;
    }

    return true;
}

bool QGLGraphicsNavigationItemPrivate::handleHover(QGraphicsSceneHoverEvent *e)
{
    if (!panning && (viewportItem->options() &
            QGLGraphicsViewportItem::ObjectPicking) != 0) {
        QObject *object = viewportItem->objectForPosition(e->pos());
        if (pressedObject) {
            // Send the move event to the pressed object.  Use a position
            // of (0, 0) if the mouse is still within the pressed object,
            // or (-1, -1) if the mouse is no longer within the pressed object.
            QMouseEvent event
                (QEvent::MouseMove,
                 (pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
                 e->screenPos(), Qt::NoButton, Qt::NoButton, e->modifiers());
            QCoreApplication::sendEvent(pressedObject, &event);
        } else if (object) {
            if (object != enteredObject) {
                if (enteredObject)
                    sendLeaveEvent(enteredObject);
                enteredObject = object;
                sendEnterEvent(enteredObject);
            }
            QMouseEvent event
                (QEvent::MouseMove, QPoint(0, 0),
                 e->screenPos(), Qt::NoButton, Qt::NoButton, e->modifiers());
            QCoreApplication::sendEvent(object, &event);
        } else if (enteredObject) {
            sendLeaveEvent(enteredObject);
            enteredObject = 0;
        }
        return true;
    }
    return false;
}

bool QGLGraphicsNavigationItemPrivate::handleHoverLeave()
{
    if (!pressedObject && enteredObject) {
        sendLeaveEvent(enteredObject);
        enteredObject = 0;
        return true;
    } else {
        return false;
    }
}

// Zoom in and out according to the change in wheel delta.
void QGLGraphicsNavigationItemPrivate::wheel(qreal delta)
{
    QGLCamera *camera = viewportItem->camera();
    qreal scale = qAbs(viewDelta(delta, delta).x());
    if (delta < 0)
        scale = -scale;
    if (scale >= 0.0f)
        scale += 1.0f;
    else
        scale = 1.0f / (1.0f - scale);
    qreal fov = camera->fieldOfView();
    if (fov != 0.0f)
        camera->setFieldOfView(camera->fieldOfView() / scale);
    else
        camera->setViewSize(camera->viewSize() / scale);
}

// Pan left/right/up/down without rotating about the object.
void QGLGraphicsNavigationItemPrivate::pan(qreal deltax, qreal deltay)
{
    QGLCamera *camera = viewportItem->camera();
    QPointF delta = viewDelta(deltax, deltay);
    QVector3D t = camera->translation(delta.x(), -delta.y(), 0.0f);

    // Technically panning the eye left should make the object appear to
    // move off to the right, but this looks weird on-screen where the user
    // actually thinks they are picking up the object and dragging it rather
    // than moving the eye.  We therefore apply the inverse of the translation
    // to make it "look right".
    camera->setEye(camera->eye() - t);
    camera->setCenter(camera->center() - t);
}

// Rotate about the object being viewed.
void QGLGraphicsNavigationItemPrivate::rotate(qreal deltax, qreal deltay)
{
    QGLCamera *camera = viewportItem->camera();
    QRectF rect = viewportItem->rect();
    int rotation = camera->screenRotation();
    if (rotation == 90 || rotation == 270) {
        qSwap(deltax, deltay);
    }
    if (rotation == 90 || rotation == 180) {
        deltax = -deltax;
    }
    if (rotation == 180 || rotation == 270) {
        deltay = -deltay;
    }
    qreal anglex = deltax * 90.0f / rect.width();
    qreal angley = deltay * 90.0f / rect.height();
    QQuaternion q = camera->pan(-anglex);
    q *= camera->tilt(-angley);
    camera->rotateCenter(q);
}

// Convert deltas in the X and Y directions into percentages of
// the view width and height.
QPointF QGLGraphicsNavigationItemPrivate::viewDelta(qreal deltax, qreal deltay)
{
    QRectF rect = viewportItem->rect();
    qreal w = rect.width();
    qreal h = rect.height();
    QGLCamera *camera = viewportItem->camera();
    bool scaleToWidth;
    qreal scaleFactor, scaleX, scaleY;
    QSizeF viewSize = camera->viewSize();
    if (w >= h) {
        if (viewSize.width() >= viewSize.height())
            scaleToWidth = true;
        else
            scaleToWidth = false;
    } else {
        if (viewSize.width() >= viewSize.height())
            scaleToWidth = false;
        else
            scaleToWidth = true;
    }
    int rotation = camera->screenRotation();
    if (rotation == 90 || rotation == 270) {
        scaleToWidth = !scaleToWidth;
        qSwap(deltax, deltay);
    }
    if (rotation == 90 || rotation == 180) {
        deltax = -deltax;
    }
    if (rotation == 180 || rotation == 270) {
        deltay = -deltay;
    }
    if (scaleToWidth) {
        scaleFactor = 2.0f / viewSize.width();
        scaleX = scaleFactor * h / w;
        scaleY = scaleFactor;
    } else {
        scaleFactor = 2.0f / viewSize.height();
        scaleX = scaleFactor;
        scaleY = scaleFactor * w / h;
    }
    return QPointF(deltax * scaleX / w, deltay * scaleY / h);
}

QT_END_NAMESPACE
