/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef FPSWIDGET_H
#define FPSWIDGET_H

#include <QGLWidget>
#if defined(QT_OPENGL_ES_2)
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtOpenGL/qglshaderprogram.h>
#endif

class QTime;
class QTimer;

class FPSWidget : public QGLWidget
{
    Q_OBJECT
public:
    FPSWidget(QWidget *parent = 0);
    ~FPSWidget();
    void initializeGL();
    void resizeGL(int, int);
    void paintGL();
protected:
    void keyPressEvent(QKeyEvent *) { close(); }
    void mousePressEvent(QMouseEvent *) { close(); }
signals:
    void fps(int);
private slots:
    void animate();
private:
    QTime *time;
    QTime *frameTime;
    QTimer *timer;
    QTimer *goAwayTimer;
    int xrot, yrot, zrot;
    int frameElapsed;
    int frameCount;
    int totalFrameTime;
#if defined(QT_OPENGL_ES_2)
    void setupShaders();

    QGLShaderProgram program;
    QMatrix4x4 projection;
    QMatrix4x4 modelView;
    int vertexAttr;
    int normalAttr;
    int matrixUniform;
    int materialUniform;
#endif
};

#endif // FPSWIDGET_H
