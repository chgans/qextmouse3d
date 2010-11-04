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

#ifndef QGLABSTRACTSURFACE_H
#define QGLABSTRACTSURFACE_H

#include "qt3dglobal.h"
#include <QtOpenGL/qgl.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class Q_QT3D_EXPORT QGLAbstractSurface
{
public:
    virtual ~QGLAbstractSurface();

    enum SurfaceType
    {
        Widget,
        FramebufferObject,
        PixelBuffer,
        Subsurface,
        Masked,
        Other
    };

    QGLAbstractSurface::SurfaceType surfaceType() const { return m_type; }

    virtual QPaintDevice *device() const = 0;
    virtual bool activate(QGLAbstractSurface *prevSurface = 0) = 0;
    virtual void deactivate(QGLAbstractSurface *nextSurface = 0) = 0;
    virtual QRect viewportGL() const = 0;
    QRect viewportRect() const;

    bool switchTo(QGLAbstractSurface *nextSurface);

    static QGLAbstractSurface *createSurfaceForDevice(QPaintDevice *device);
    static QGLAbstractSurface *createSurfaceForContext(const QGLContext *context);

protected:
    QGLAbstractSurface(QGLAbstractSurface::SurfaceType surfaceType)
        : m_type(surfaceType) {}

private:
    QGLAbstractSurface::SurfaceType m_type;

    Q_DISABLE_COPY(QGLAbstractSurface)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif