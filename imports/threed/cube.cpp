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

#include "cube.h"
#include "qglbuilder.h"
#include "qglcube.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Cube Cube
    \brief The Cube item represents a simple cube in 3D space.
    \since 4.8
    \ingroup qt3d::qml3d
    \inherits Item3d

    The Cube element in QML provides a simple way to create a cube
    object, usually for testing material effects.  For example,
    the following QML code displays a cube of size 1.5, centered
    on the origin and covered in the Qt logo texture:

    \code
    Cube {
        size: 1.5
        effect: Effect {
            color: "#aaca00"
            texture: "qtlogo.png"
        }
    }
    \endcode

    The cube can be moved from the origin by specifying the
    Item3d::position property:

    \code
    Cube {
        size: 1.5
        position: Qt.vector3d(1, 0, 5)
        effect: Effect {
            color: "#aaca00"
            texture: "qtlogo.png"
        }
    }
    \endcode

    \sa Item3d, Pane
*/

class CubePrivate
{
public:
    CubePrivate() : size(1.0f) {}
    ~CubePrivate() { delete cube; }

    qreal size;
    QGLSceneNode *cube;
};

Cube::Cube(QObject *parent)
    : Item3d(parent)
{
    d = new CubePrivate();
}

Cube::~Cube()
{
    delete d;
}

/*!
    \qmlproperty real Cube::size

    The size of the cube.  The default value for this property is 1.
    The cube will be centered on Item3d::position.
*/

qreal Cube::size() const
{
    return d->size;
}

void Cube::setSize(qreal value)
{
    d->size = value;
    //        delete d->cube;
    d->cube = 0;
    emit sizeChanged();
    update();
}

void Cube::drawItem(QGLPainter *painter)
{
    if (!d->cube) {
        QGLBuilder builder;
        builder << QGL::Faceted << QGLCube(d->size);
        d->cube = builder.finalizedSceneNode();
        d->cube->setParent(this);
    }
    d->cube->draw(painter);
}

QT_END_NAMESPACE
