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

#include "qglblendmaterial.h"
#include <QtOpenGL/qgl.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLBlendMaterial
    \since 4.8
    \brief The QGLBlendMaterial class enables alpha blending during material rendering.
    \ingroup qt3d
    \ingroup qt3d::materials

    Use this class to turn an ordinary material into one that
    is alpha blended over previous screen content.  The bind() function
    will enable \c GL_BLEND and then bind the underlying wrap() material.
    The release() function releases the underlying wrap() material and
    then disables \c GL_BLEND.

    \sa QGLAbstractWrappedMaterial
*/

/*!
    Constructs a blend material and attaches it to \a parent.
*/
QGLBlendMaterial::QGLBlendMaterial(QObject *parent)
    : QGLAbstractWrappedMaterial(parent)
{
}

/*!
    Destroy this blend material.
*/
QGLBlendMaterial::~QGLBlendMaterial()
{
}

/*!
    \reimp
*/
void QGLBlendMaterial::bind(QGLPainter *painter)
{
    glEnable(GL_BLEND);
    if (wrap())
        wrap()->bind(painter);
}

/*!
    \reimp
*/
void QGLBlendMaterial::release(QGLPainter *painter, QGLAbstractMaterial *next)
{
    if (wrap())
        wrap()->release(painter, next);
    glDisable(GL_BLEND);
}

/*!
    \reimp
*/
int QGLBlendMaterial::compare(const QGLAbstractMaterial *other) const
{
    const QGLBlendMaterial *blend =
        qobject_cast<const QGLBlendMaterial *>(other);
    if (!blend)
        return QGLAbstractMaterial::compare(other);
    return QGLAbstractMaterial::compare(wrap(), blend->wrap());
}

QT_END_NAMESPACE
