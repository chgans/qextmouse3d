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

#include "qglteapot.h"
#include "qglteapot_data_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLTeapot
    \brief The QGLTeapot class represents a 3D teapot object.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::geometry

    The classic 3D "Utah Teapot" was originally drawn by Martin Newell
    in 1975.  The vertex data was made publicly available by him and
    it has been a standard 3D test object ever since.

    For more information on the history of the "Utah Teapot", see
    Wikipedia, http://en.wikipedia.org/wiki/Utah_teapot.

    The following example code uses QGLTeapot to draw a teapot of size
    0.5 at the origin:

    \code
    QGLTeapot teapot;
    painter.modelViewMatrix().scale(0.5f);
    teapot.draw(painter);
    \endcode

    The QGLTeapot object contains a lot of vertex data once it has
    been subdivided into triangles.  It is recommended that instances
    of this class be created at startup and then be reused over and over,
    rather than creating and destroying the object every frame.

    The teapot geometry will be uploaded to the GL server as a vertex
    buffer the first time the object is drawn, which also improves
    performance.

    \sa QGLBezierGeometry
*/

/*!
    Constructs a new 3D teapot geometry object, populates it,
    and attaches it to \a parent.
*/
QGLTeapot::QGLTeapot(QObject *parent)
    : QGLBezierGeometry(parent)
{
    teapotLoadBezier(*this);
}

/*!
    Destroys this teapot geometry object.
*/
QGLTeapot::~QGLTeapot()
{
}

QT_END_NAMESPACE
