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

#ifndef CUBEVIEW_H
#define CUBEVIEW_H

#include "qglview.h"
#include "qglbuilder.h"

class QGLFramebufferObject;
class QGLCamera;

class CubeView : public QGLView
{
    Q_OBJECT
    Q_PROPERTY(qreal teapotAngle READ teapotAngle WRITE setTeapotAngle)
    Q_PROPERTY(qreal cubeAngle READ cubeAngle WRITE setCubeAngle)
    Q_PROPERTY(qreal orbitAngle READ orbitAngle WRITE setOrbitAngle)
public:
    CubeView(QWidget *parent = 0);
    ~CubeView();

    qreal teapotAngle() const { return tangle; }
    void setTeapotAngle(qreal angle) { tangle = angle; performUpdate(); }

    qreal cubeAngle() const { return cangle; }
    void setCubeAngle(qreal angle) { cangle = angle; performUpdate(); }

    qreal orbitAngle() const { return oangle; }
    void setOrbitAngle(qreal angle) { oangle = angle; performUpdate(); }

protected:
    void initializeGL(QGLPainter *painter);
    void paintGL(QGLPainter *painter);

private:
    QGLBuilder cube;
    QGLBuilder teapot;
    QGLTexture2D qtlogo;
    QGLFramebufferObject *fbo;
    QGLCamera *innerCamera;
    qreal tangle;
    qreal cangle;
    qreal oangle;
    bool needsUpdate;

    void performUpdate();
    void drawCube1(QGLPainter *painter, const QVector3D &posn);
    void drawCube2(QGLPainter *painter, const QVector3D &posn);
};

#endif
