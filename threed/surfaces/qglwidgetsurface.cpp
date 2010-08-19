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

#include "qglwidgetsurface.h"
#include <QtGui/private/qwidget_p.h>
#include <QtGui/private/qwindowsurface_p.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpaintengine.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLWidgetSurface
    \brief The QGLWidgetSurface class represents a widget that is begin used as an OpenGL drawing surface.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::painting
*/

class QGLWidgetSurfacePrivate
{
public:
    QGLWidgetSurfacePrivate()
        : widget(0), glWidget(0), activePainter(0), painterContext(0) {}
    QGLWidgetSurfacePrivate(QWidget *w)
        : widget(w), activePainter(0), painterContext(0)
    {
        glWidget = qobject_cast<QGLWidget *>(w);
    }

    QWidget *widget;
    QGLWidget *glWidget;
    QPainter *activePainter;
    const QGLContext *painterContext;
};

/*!
    Constructs a widget surface.  This constructor should be followed
    by a call to setWidget().
*/
QGLWidgetSurface::QGLWidgetSurface()
    : QGLAbstractSurface(QGLAbstractSurface::Widget)
    , d_ptr(new QGLWidgetSurfacePrivate)
{
}

/*!
    Constructs a widget surface for \a widget.
*/
QGLWidgetSurface::QGLWidgetSurface(QWidget *widget)
    : QGLAbstractSurface(QGLAbstractSurface::Widget)
    , d_ptr(new QGLWidgetSurfacePrivate(widget))
{
}

/*!
    Destroys this widget surface.
*/
QGLWidgetSurface::~QGLWidgetSurface()
{
}

/*!
    Returns the widget that is underlying this surface.

    \sa setWidget()
*/
QWidget *QGLWidgetSurface::widget() const
{
    Q_D(const QGLWidgetSurface);
    return d->widget;
}

/*!
    Sets the \a widget that is underlying this surface.

    \sa widget()
*/
void QGLWidgetSurface::setWidget(QWidget *widget)
{
    Q_D(QGLWidgetSurface);
    d->widget = widget;
    d->glWidget = qobject_cast<QGLWidget *>(widget);
}

/*!
    \reimp
*/
QPaintDevice *QGLWidgetSurface::device() const
{
    Q_D(const QGLWidgetSurface);
    return d->widget;
}

/*!
    Activates widget() for OpenGL drawing operations.

    If widget() is a QGLWidget, then this function will call
    QGLWidget::makeCurrent() on the widget and return true.

    If widget() is not a QGLWidget, then it must be currently
    active in a paint event with an OpenGL paint engine.
    This function will return false if the active paint engine
    is not OpenGL or it is not associated with widget().
    Otherwise, QPainter::beginNativePainting() will be called
    on the painter associated with widget().

    If \a prevSurface is not null, then QPainter::beginNativePainting()
    will be skipped because it is assumed to have been performed
    previously.  This way, switching from a widget surface to another
    surface and back again will not result in multiple calls to
    QPainter::endNativePainting() and QPainter::beginNativePainting().

    \sa deactivate()
*/
bool QGLWidgetSurface::activate(QGLAbstractSurface *prevSurface)
{
    Q_D(QGLWidgetSurface);
    if (d->glWidget) {
        // Can directly make the context on a QGLWidget current.
        const QGLContext *context = d->glWidget->context();
        if (QGLContext::currentContext() != context)
            const_cast<QGLContext *>(context)->makeCurrent();
        return true;
    } else {
        // See if there is a QPainter active for the widget's window surface.
        QWindowSurface *surf = d->widget->windowSurface();
        if (surf) {
            QPaintDevice *surfDevice = surf->paintDevice();
            if (surfDevice && surfDevice->paintingActive()) {
                // Check that the QPaintEngine is OpenGL and that the
                // currently active QPainter on that engine is this widget.
                QPaintEngine *engine = surfDevice->paintEngine();
                if (engine &&
                        (engine->type() == QPaintEngine::OpenGL ||
                         engine->type() == QPaintEngine::OpenGL2)) {
                    QPainter *painter = engine->painter();
                    if (painter && painter->device() == d->widget) {
                        d->activePainter = painter;
                        if (!prevSurface) {
                            // First time the surface was activiated,
                            // so just turn off the paint engine.
                            painter->beginNativePainting();
                            d->painterContext = QGLContext::currentContext();
                        } else if (d->painterContext) {
                            // We're switching back to the widget surface,
                            // so reactivate the painter's context.  But don't
                            // call beginNativePainting() again.
                            if (d->painterContext !=
                                    QGLContext::currentContext())
                                const_cast<QGLContext *>(d->painterContext)
                                    ->makeCurrent();
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    }
}

/*!
    Deactivates widget() from the current context.

    If widget() is not a QGLWidget, and \a nextSurface is null,
    then QPainter::endNativePainting() will be called on the painter
    associated with widget().

    The current context will still be current after calling this function.

    \sa activate()
*/
void QGLWidgetSurface::deactivate(QGLAbstractSurface *nextSurface)
{
    Q_D(QGLWidgetSurface);
    if (!nextSurface && d->activePainter) {
        d->activePainter->endNativePainting();
        d->activePainter = 0;
        d->painterContext = 0;
    }
}

/*!
    \reimp
*/
QRect QGLWidgetSurface::viewportGL() const
{
    Q_D(const QGLWidgetSurface);
    if (d->widget)
        return d->widget->rect();   // Origin assumed to be (0, 0).
    else
        return QRect();
}

QT_END_NAMESPACE
