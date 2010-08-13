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

#include "qglabstractsurface.h"
#include "qglcontextsurface_p.h"
#include "qglframebufferobjectsurface.h"
#include "qglpixelbuffersurface.h"
#include "qglsubsurface.h"
#include "qglwidgetsurface.h"
#include <QtGui/qpaintdevice.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLAbstractSurface
    \brief The QGLAbstractSurface class represents an OpenGL drawing surface.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::painting
*/

/*!
    \enum QGLAbstractSurface::SurfaceType
    This enum defines the type of a QGLAbstractSurface.

    \value Widget Instance of QGLWidgetSurface.
    \value FramebufferObject Instance of QGLFramebufferObjectSurface.
    \value PixelBuffer Instance of QGLPixelBufferSurface.
    \value Subsurface Instance of QGLSubsurface.
    \value Other Some other kind of user-defined surface.
*/

/*!
    \fn QGLAbstractSurface::QGLAbstractSurface(QGLAbstractSurface::SurfaceType surfaceType)

    Constructs an OpenGL drawing surface of the specified \a surfaceType.
*/

/*!
    Destroys this OpenGL drawing surface.
*/
QGLAbstractSurface::~QGLAbstractSurface()
{
}

/*!
    \fn QGLAbstractSurface::SurfaceType QGLAbstractSurface::surfaceType() const

    Returns the type of this surface.
*/

/*!
    \fn QPaintDevice *QGLAbstractSurface::device() const

    Returns the raw device that this surface will draw on.

    If the surface is an instance of QGLSubsurface, then this will
    return the device of the surface that underlies the subsurface.
    The viewportRect() defines the region to render into.

    \sa viewportRect()
*/

/*!
    Activate this surface by making its context current, binding
    the associated framebuffer object, if any, and adjusting the
    \c{glViewport()} to restrict painting to viewportRect().

    If \a prevSurface is null, then it will first be deactivated
    with deactivate().  If \a prevSurface and this surface are
    both framebuffer objects for the same context, then the
    deactivate() on \a prevSurface will be skipped as it is unnecessary.

    Returns true if the surface was activated; false otherwise.

    \sa deactivate(), activateNoViewport()
*/
bool QGLAbstractSurface::activate(QGLAbstractSurface *prevSurface)
{
    if (prevSurface)
        prevSurface->deactivate(this);
    if (!activateNoViewport(prevSurface))
        return false;
    QRect viewport = viewportRect();
    int height = device()->height();
    glViewport(viewport.x(), height - (viewport.y() + viewport.height()),
               viewport.width(), viewport.height());
    return true;
}

/*!
    \fn bool QGLAbstractSurface::activateNoViewport(QGLAbstractSurface *prevSurface)

    Activate this surface by making its context current and binding
    the associated framebuffer object, if any.  The \c{glViewport()}
    is not modified.

    If \a prevSurface is null, then it indicates the previous surface
    that was active.

    Returns true if the surface was activated; false otherwise.
    
    \sa activate()
*/

/*!
    \fn void QGLAbstractSurface::deactivate(QGLAbstractSurface *nextSurface)

    Deactivate this surface from the current context, but leave the
    context current.  Typically this will release the framebuffer
    object associated with the surface.

    If \a nextSurface is not null, then it indicates that the caller
    is switching to \a nextSurface.

    \sa activate()
*/

/*!
    \fn QRect QGLAbstractSurface::viewportRect() const

    Returns the rectangle of the surface device() that is occupied by
    the viewport for this surface.  The origin is at the top-left.

    Normally the viewport rectangle is the full extent of the device(),
    but it could be smaller if the application only wishes to draw
    into a subpart of the device().  An example would be rendering left
    and right stereo eye images into the two halves of a QGLWidget.
    The eye surfaces would typically be instances of QGLSubsurface.

    \sa device()
*/

/*!
    Creates an OpenGL drawing surface for the specified paint \a device.
    Returns null if it is not possible to create a surface for \a device.

    \sa createSurfaceForContext()
*/
QGLAbstractSurface *QGLAbstractSurface::createSurfaceForDevice
    (QPaintDevice *device)
{
    Q_ASSERT(device);
    switch (device->devType()) {
    case QInternal::Widget:
        return new QGLWidgetSurface(static_cast<QWidget *>(device));
    case QInternal::Pbuffer:
        return new QGLPixelBufferSurface(static_cast<QGLPixelBuffer *>(device));
    case QInternal::FramebufferObject:
        return new QGLFramebufferObjectSurface
            (static_cast<QGLFramebufferObject *>(device));
    default:
        return 0;
    }
}

/*!
    Creates an OpenGL drawing surface for the paint device
    underlying \a context.  If the paint device is not recognized,
    then a generic surface will be created that makes \a context
    current when the surface is activated.

    \sa createSurfaceForDevice()
*/
QGLAbstractSurface *QGLAbstractSurface::createSurfaceForContext
    (const QGLContext *context)
{
    Q_ASSERT(context);
    QGLAbstractSurface *surface = createSurfaceForDevice(context->device());
    if (!surface)
        surface = new QGLContextSurface(context);
    return surface;
}

QT_END_NAMESPACE
