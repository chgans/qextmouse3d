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

#ifndef SphereView_H
#define SphereView_H

#include "qglview.h"
#include "qgltexture2d.h"
#include "qgldisplaylist.h"

class QTimer;
class QGLSphere;
class QGLMaterialParameters;
class QGLLightModel;

class SphereView : public QGLView
{
    Q_OBJECT
public:
    SphereView(QWidget *parent = 0);
    ~SphereView();

protected:
    void initializeGL(QGLPainter *painter);
    void paintGL(QGLPainter *painter);

    void keyPressEvent(QKeyEvent *);

private slots:
    void rotate();

private:
    QImage pointsImage;
    bool textured;
    QGLTexture2D cubeTexture, icoTexture, uvTexture;
    QGLDisplayList list;
    QVector<QGLSceneNode *> spheres;
    QTimer *timer;
    int angle;
    QGLLightParameters *lp;
    QGLMaterialParameters *mat;
    QGLLightModel *mdl;
};

#endif
