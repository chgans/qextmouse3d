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

#include "qglsubsurface.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLSubsurface
    \brief The QGLSubsurface class represents a sub-surface of another OpenGL drawing surface.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::painting
*/

class QGLSubsurfacePrivate
{
public:
    QGLSubsurfacePrivate() : surface(0) {}
    QGLSubsurfacePrivate(QGLAbstractSurface *surf, const QRect &rgn)
        : surface(surf), region(rgn) {}

    QGLAbstractSurface *surface;
    QRect region;
};

/*!
    Constructs a new subsurface.  This constructor should be followed
    by calls to setSurface() and setRegion().
*/
QGLSubsurface::QGLSubsurface()
    : QGLAbstractSurface(QGLAbstractSurface::Subsurface)
    , d_ptr(new QGLSubsurfacePrivate)
{
}

/*!
    Constructs a new subsurface that occupies \a region within
    \a surface.
*/
QGLSubsurface::QGLSubsurface
        (QGLAbstractSurface *surface, const QRect &region)
    : QGLAbstractSurface(QGLAbstractSurface::Subsurface)
    , d_ptr(new QGLSubsurfacePrivate(surface, region))
{
}

/*!
    Destroys this subsurface.
*/
QGLSubsurface::~QGLSubsurface()
{
}

/*!
    Returns the surface behind this subsurface, or null if the
    surface has not been set.

    \sa setSurface(), region()
*/
QGLAbstractSurface *QGLSubsurface::surface() const
{
    Q_D(const QGLSubsurface);
    return d->surface;
}

/*!
    Sets the \a surface behind this subsurface.

    \sa surface(), setRegion()
*/
void QGLSubsurface::setSurface(QGLAbstractSurface *surface)
{
    Q_D(QGLSubsurface);
    d->surface = surface;
}

/*!
    Returns the region within surface() that is occupied by this
    subsurface, relative to the viewportRect() of surface().

    \sa setRegion(), surface()
*/
QRect QGLSubsurface::region() const
{
    Q_D(const QGLSubsurface);
    return d->region;
}

/*!
    Sets the \a region within surface() that is occupied by this
    subsurface, relative to the viewportRect() of surface().

    \sa region(), setSurface()
*/
void QGLSubsurface::setRegion(const QRect &region)
{
    Q_D(QGLSubsurface);
    d->region = region;
}

/*!
    \reimp
*/
QPaintDevice *QGLSubsurface::device() const
{
    Q_D(const QGLSubsurface);
    if (d->surface)
        return d->surface->device();
    else
        return 0;
}

/*!
    \reimp
*/
bool QGLSubsurface::activateNoViewport(QGLAbstractSurface *prevSurface)
{
    Q_D(QGLSubsurface);
    if (d->surface)
        return d->surface->activateNoViewport(prevSurface);
    else
        return false;
}

/*!
    \reimp
*/
void QGLSubsurface::deactivate(QGLAbstractSurface *nextSurface)
{
    Q_D(QGLSubsurface);
    if (d->surface)
        d->surface->deactivate(nextSurface);
}

/*!
    \reimp
*/
QRect QGLSubsurface::viewportRect() const
{
    Q_D(const QGLSubsurface);
    if (d->surface) {
        QRect rect = d->surface->viewportRect();
        return d->region.translated(rect.x(), rect.y());
    } else {
        return d->region;
    }
}

QT_END_NAMESPACE
