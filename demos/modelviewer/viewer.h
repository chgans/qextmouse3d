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

#ifndef VIEWER_H
#define VIEWER_H

#include <QtOpenGL/qgl.h>

#include "qglview.h"

class Model;
class QGLPainter;
class QGLLightModel;
class QGLLightParameters;
class QTimer;
class QGLDisplayList;
class WheelData;

class Viewer : public QGLView
{
    Q_OBJECT
public:
    enum View
    {
        TopView,
        FrontView
    };

    Viewer(QWidget *parent = 0);
    ~Viewer();
    void setModel(Model *model) { m_model = model; }

    int x() const { return m_x; }
    void setX(int x);
    int y() { return m_y; }
    void setY(int y);
    int z() const { return m_z; }
    void setZ(int z);
    int rotX() const { return m_rotX; }
    void setRotX(int rx);
    int rotY() const { return m_rotY; }
    void setRotY(int ry);
    int rotZ() const { return m_rotZ; }
    void setRotZ(int rz);
    int zoom() const { return m_zoom; }
    void setZoom(int zoom);
    void reset();
    View view() const { return m_view; }
    void setView(View view);
    bool floorEnabled() const { return m_drawFloor; }
    void setFloorEnabled(bool enabled);

public slots:
    void enableAnimation(bool enable);

protected:
    void paintGL(QGLPainter *painter);
    void initializeGL(QGLPainter *painter);
    //void initializeGL();
    //void paintGL();
    //void resizeGL(int, int);
    //void wheelEvent(QWheelEvent *e);
    //void mousePressEvent(QMouseEvent *e);
    //void mouseMoveEvent(QMouseEvent *e);
    //void mouseReleaseEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void animate();

private:
    void mouseDrag(QMouseEvent *e);
    void buildFloor();

    QTimer *m_timer;
    Model *m_model;
    QGLLightModel *m_lightModel;
    QGLLightParameters *m_lightParameters;
    int m_x;
    int m_y;
    int m_z;
    int m_rotX;
    int m_rotY;
    int m_rotZ;
    int m_spin;
    float m_zoom;
    WheelData *m_wd;
    View m_view;
    bool m_animate;
    bool m_warningDisplayed;
    bool m_dragging;
    QPointF m_dragStart;
    QGLDisplayList *m_floor;
    bool m_drawFloor;
};


#endif // VIEWER_H
