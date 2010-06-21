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

#ifndef PLANETVIEW_H
#define PLANETVIEW_H

#include "qglview.h"
#include "qgldisplaylist.h"
#include "qsphericalanimation.h"

class PlanetView : public QGLView
{
    Q_OBJECT
    Q_PROPERTY(QVector3D planet1Pos READ planet1Pos WRITE setPlanet1Pos)
    Q_PROPERTY(QVector3D planet2Pos READ planet2Pos WRITE setPlanet2Pos)
    Q_PROPERTY(QVector3D planet3Pos READ planet3Pos WRITE setPlanet3Pos)
public:
    PlanetView(QWidget *parent = 0);
    ~PlanetView() {}

    QVector3D planet1Pos() const { return planet1Posn; }
    void setPlanet1Pos(const QVector3D& value)
        { planet1Posn = value; updateGL(); }

    QVector3D planet2Pos() const { return planet2Posn; }
    void setPlanet2Pos(const QVector3D& value)
        { planet2Posn = value; updateGL(); }

    QVector3D planet3Pos() const { return planet3Posn; }
    void setPlanet3Pos(const QVector3D& value)
        { planet3Posn = value; updateGL(); }

protected:
    void initializeGL(QGLPainter *painter);
    void paintGL(QGLPainter *painter);

private:
    QGLDisplayList list;
    QGLSceneNode *sun;
    QGLSceneNode *planet1;
    QGLSceneNode *planet2;
    QGLSceneNode *planet3;
    QVector3D planet1Posn;
    QSphericalAnimation *animPlanet1;
    QVector3D planet2Posn;
    QSphericalAnimation *animPlanet2;
    QVector3D planet3Posn;
    QSphericalAnimation *animPlanet3;
};

#endif
