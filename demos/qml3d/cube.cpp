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
#include "qglcube.h"

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,Cube,Cube)

class CubePrivate
{
public:
    CubePrivate() : width(1.0f), height(1.0f), depth(1.0f), cube(0) {}
    ~CubePrivate() { delete cube; }

    qreal width;
    qreal height;
    qreal depth;
    QGLCube *cube;
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

qreal Cube::width() const
{
    return d->width;
}

void Cube::setWidth(qreal value)
{
    d->width = value;
    delete d->cube;
    d->cube = 0;
    emit sizeChanged();
    update();
}

qreal Cube::height() const
{
    return d->height;
}

void Cube::setHeight(qreal value)
{
    d->height = value;
    delete d->cube;
    d->cube = 0;
    emit sizeChanged();
    update();
}

qreal Cube::depth() const
{
    return d->depth;
}

void Cube::setDepth(qreal value)
{
    d->depth = value;
    delete d->cube;
    d->cube = 0;
    emit sizeChanged();
    update();
}

qreal Cube::size() const
{
    return qMax(d->width, qMax(d->height, d->depth));
}

void Cube::setSize(qreal value)
{
    d->width = value;
    d->height = value;
    d->depth = value;
    delete d->cube;
    d->cube = 0;
    emit sizeChanged();
    update();
}

void Cube::drawItem(QGLPainter *painter)
{
    if (!d->cube)
        d->cube = new QGLCube(d->width, d->height, d->depth);
    d->cube->draw(painter);
}

QT_END_NAMESPACE
