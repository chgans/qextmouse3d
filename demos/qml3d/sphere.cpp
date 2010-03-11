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

#include "sphere.h"
#include "qglsphere.h"
#include "qgldisplaylist.h"

QT_BEGIN_NAMESPACE

//QML_DEFINE_TYPE(Qt,4,6,Sphere,Sphere)

class SpherePrivate
{
public:
    SpherePrivate()
        : type(Sphere::UV), size(1.0f), faces(5000.0f),
          geometry(0), valid(false)
    {
    }
    ~SpherePrivate()
    {
        delete geometry;
    }

    Sphere::Type type;
    qreal size;
    qreal faces;
    QGLDisplayList *geometry;
    bool valid;
};

Sphere::Sphere(QObject *parent)
    : Item3d(parent)
{
    d = new SpherePrivate();
}

Sphere::~Sphere()
{
    delete d;
}

Sphere::Type Sphere::type() const
{
    return d->type;
}

void Sphere::setType(Type value)
{
    if (d->type != value) {
        d->type = value;
        d->valid = false;
        emit typeChanged();
        update();
    }
}

qreal Sphere::size() const
{
    return d->size;
}

void Sphere::setSize(qreal value)
{
    if (d->size != value) {
        d->size = value;
        d->valid = false;
        emit sizeChanged();
        update();
    }
}

qreal Sphere::faces() const
{
    return d->faces;
}

void Sphere::setFaces(qreal value)
{
    if (d->faces != value) {
        d->faces = value;
        d->valid = false;
        emit facesChanged();
        update();
    }
}

static int depthFromFaceCount(int faces)
{
    int maxFaces = 20;
    int depth = 1;
    while (maxFaces < faces) {
        ++depth;
        maxFaces *= 4;
    }
    return depth;
}

void Sphere::drawItem(QGLPainter *painter)
{
    if (!d->geometry || !d->valid) {
        delete d->geometry;
        d->geometry = new QGLDisplayList();
        int depth = depthFromFaceCount(qRound(d->faces));
        if (d->type == Ico)
            (*d->geometry) << QGLIcoSphere(d->size, depth);
        else if (d->type == Cube)
            (*d->geometry) << QGLCubeSphere(d->size, depth);
        else
            (*d->geometry) << QGLSphere(d->size, depth);
        d->geometry->finalize();
        d->valid = true;
    }
    d->geometry->draw(painter);
}

QT_END_NAMESPACE
