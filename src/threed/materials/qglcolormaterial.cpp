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

#include "qglcolormaterial.h"
#include "qglpainter.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLColorMaterial
    \since 4.8
    \brief The QGLColorMaterial class implements flat color materials for 3D rendering.
    \ingroup qt3d
    \ingroup qt3d::materials

    When bound to a QGLPainter, QGLColorMaterial will select a flat
    color drawing effect, to render fragments with color(), ignoring
    any lights or textures that may be active on the QGLPainter state.

    \sa QGLMaterial
*/

class QGLColorMaterialPrivate
{
public:
    QGLColorMaterialPrivate() : color(255, 255, 255, 255) {}

    QColor color;
};

/*!
    Constructs a new flat color material and attaches it to \a parent.
*/
QGLColorMaterial::QGLColorMaterial(QObject *parent)
    : QGLAbstractMaterial(parent)
    , d_ptr(new QGLColorMaterialPrivate)
{
}

/*!
    Destroys this flat color material.
*/
QGLColorMaterial::~QGLColorMaterial()
{
}

/*!
    \property QGLColorMaterial::color
    \brief the flat color to use to render the material.  The default
    color is white.

    \sa colorChanged()
*/

QColor QGLColorMaterial::color() const
{
    Q_D(const QGLColorMaterial);
    return d->color;
}

void QGLColorMaterial::setColor(const QColor &color)
{
    Q_D(QGLColorMaterial);
    if (d->color != color) {
        d->color = color;
        emit colorChanged();
        emit materialChanged();
    }
}

/*!
    \reimp
*/
void QGLColorMaterial::bind(QGLPainter *painter)
{
    Q_D(const QGLColorMaterial);
    painter->setColor(d->color);
    painter->setStandardEffect(QGL::FlatColor);
}

/*!
    \reimp
*/
void QGLColorMaterial::release(QGLPainter *painter, QGLAbstractMaterial *next)
{
    // No textures or other modes, so nothing to do here.
    Q_UNUSED(painter);
    Q_UNUSED(next);
}

/*!
    \reimp
*/
int QGLColorMaterial::compare(const QGLAbstractMaterial *other) const
{
    const QGLColorMaterial *colmat =
        qobject_cast<const QGLColorMaterial *>(other);
    if (!colmat)
        return QGLAbstractMaterial::compare(other);
    Q_D(const QGLColorMaterial);
    QRgb col1 = d->color.rgba();
    QRgb col2 = colmat->d_ptr->color.rgba();
    if (col1 < col2)
        return -1;
    else if (col1 > col2)
        return 1;
    else
        return 0;
}

/*!
    \fn void QGLColorMaterial::colorChanged()

    Signal that is emitted when color() changes.

    \sa color()
*/

QT_END_NAMESPACE