/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "spotlight.h"
#include "qglpainter.h"
#include "qglabstracteffect.h"

#include <math.h>

/*
  This code is intended to produce the same lighting effects as the
  spots.c program by Mark Kilgard:

  http://www.opengl.org/resources/code/samples/glut_examples/examples/spots.c

  This code is a new C++ implementation, which uses no code from the original
*/

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef TWO_PI
#define TWO_PI 6.283185307179586477
#endif

/*
    Create a new spotlight object with the given \a color.
    The spotlight object consists of a an OpenGL light, and also some
    information about the lights motion characteristics.
*/
Spotlight::Spotlight(QColor color)
    : translate(0.0f, 1.25f, 0.0f)
    , rotate(0.0f, 0.0f, 0.0f)
    , swing(20.0f, 0.0f, 4.0f)
    , arc(0.0f, 0.0f, 0.0f)
    , arcIncr(0.0f, 0.0f)
{
    light.setAmbientColor(QColor::fromRgbF(color.redF() * 0.20,
                                           color.greenF() * 0.20,
                                           color.blueF() * 0.20));
    light.setDiffuseColor(QColor::fromRgbF(color.redF() * 0.80,
                                           color.greenF() * 0.80,
                                           color.blueF() * 0.80));
    light.setSpecularColor(QColor::fromRgbF(color.redF() * 0.40,
                                            color.greenF() * 0.40,
                                            color.blueF() * 0.40));
    light.setPosition(QVector3D(0.0, 0.0, 0.0));
    light.setSpotDirection(QVector3D(0.0f, -1.0f, 0.0f));
    light.setSpotExponent(20.0);
    light.setSpotAngle(60.0);
}

/*
    Transform painting into the frame origin of this light, by applying
    the translate and rotates for this spotlight to the \a painter.
*/
void Spotlight::transform(QGLPainter *painter)
{
    painter->modelViewMatrix().translate(translate);
    painter->modelViewMatrix().rotate(rotate.x(), 1.0f, 0.0f, 0.0f);
    painter->modelViewMatrix().rotate(rotate.y(), 0.0f, 1.0f, 0.0f);
    painter->modelViewMatrix().rotate(rotate.z(), 0.0f, 0.0f, 1.0f);
}

/*
    Increment the rotational displacement of the lights, by adding the
    arc increment to the current arc value.
*/
void Spotlight::increment()
{
    QVector3D sinArc(sin(arc.x()), sin(arc.y()), sin(arc.z()));
    rotate = swing * sinArc;
    arc = arc + arcIncr;
    if (arc.x() > TWO_PI)
        arc.setX(0.0f);
    if (arc.y() > TWO_PI)
        arc.setY(0.0f);
    if (arc.z() > TWO_PI)
        arc.setZ(0.0f);
}

/*
    Update the position of the lights using \a painter.
*/
void Spotlight::position(QGLPainter *painter)
{
    painter->modelViewMatrix().push();
    transform(painter);
    painter->update();
    painter->modelViewMatrix().pop();
}

/*
    Draw the lines representing the spotlight rays using \a painter.
    The lines are drawn using a painted colour instead of a light model,
    so the current effect should be disabled during this function.
*/
void Spotlight::draw(QGLPainter *painter)
{
    // glDisable(GL_LIGHTING);
    painter->effect()->setActive(false);
    painter->setColor(light.diffuseColor());

    painter->modelViewMatrix().push();
    transform(painter);
    QVector3DArray verts;
    verts.append(light.position());
    // this is a bit dodgy, treating a vector as a point - might work as
    // long as you're sure that light.spatialData() is the origin always
    // also you need to be scaled so that the -1 is long enough...
    verts.append(light.spotDirection());
    painter->setVertexAttribute(QGL::Position, verts);
    painter->draw(QGL::Lines, 2);
    painter->update();
    painter->modelViewMatrix().pop();
    painter->effect()->setActive(true);
    // glEnable(GL_LIGHTING);
}
