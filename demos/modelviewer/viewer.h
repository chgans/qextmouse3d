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
#include <QtGui/qtreeview.h>

#include "qglview.h"

class Model;
class QGLPainter;
class QGLLightModel;
class QGLLightParameters;
class QTimer;
class QGLBuilder;

class Viewer : public QGLView
{
    Q_OBJECT
public:
    enum View
    {
        FrontView,
        TopView
    };

    Viewer(QWidget *parent = 0);
    ~Viewer();
    void setModel(Model *model);

    View view() const { return m_view; }
    void setView(View view);
    void setTreeView(QTreeView *view) { m_treeView = view; }
    bool floorEnabled() const { return m_drawFloor; }
    void setFloorEnabled(bool enabled);

    QVector3D position() const;
    void setPosition(const QVector3D &t);
    QVector3D orientation() const;
    void setOrientation(const QVector3D &r);
    QVector3D scale() const;
    void setScale(const QVector3D &s);
    void resetView();
    int zoomScale() { return m_zoomScale; }
    void setZoomScale(int scale);

signals:
    void manualControlEngaged();

public slots:
    void enableAnimation(bool enable);
    void registerPicking();
    void unregisterPicking();
    void objectPicked();

protected:
    void paintGL(QGLPainter *painter);
    void initializeGL(QGLPainter *painter);
    void wheelEvent(QWheelEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void animate();

private:
    void mouseDrag(QMouseEvent *e);
    void buildFloor();

    QVector3D m_orientation;
    QVector3D m_position;
    QVector3D m_scale;
    QTimer *m_timer;
    Model *m_model;
    QTreeView *m_treeView;
    QGLLightModel *m_lightModel;
    QGLLightParameters *m_lightParameters;
    View m_view;
    bool m_animate;
    bool m_warningDisplayed;
    QGLBuilder *m_floor;
    bool m_drawFloor;
    int m_zoomScale;
    bool m_pickDirty;
    QVector3D m_lastEye;
};


#endif // VIEWER_H
