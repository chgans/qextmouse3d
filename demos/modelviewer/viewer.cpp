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

#include "viewer.h"
#include "model.h"
#include "qglscenenode.h"
#include "qglcamera.h"
#include "qglpainter.h"

#include <QtGui/qmessagebox.h>
#include <QtGui/qevent.h>

#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>

#include <math.h>

Viewer::Viewer(QWidget *parent)
    : QGLWidget(parent)
    , m_timer(new QTimer(this))
    , m_model(0)
    , m_lightModel(0)
    , m_lightParameters(0)
    , m_x(0)
    , m_y(0)
    , m_z(10)
    , m_rotX(0)
    , m_rotY(0)
    , m_rotZ(0)
    , m_animate(true)
    , m_warningDisplayed(false)
    , m_dragging(false)
{
    setToolTip(tr("Drag the mouse to slide the object left-right & up-down\n"
                  "or use the mouse-wheel to push-pull the object.  Use\n"
                  "shift drag or shift-mouse-wheel to rotate."));
}

Viewer::~Viewer()
{
    // nothing to do here
}

void Viewer::setX(int x)
{
    if (m_x != x)
    {
        m_x = x;
        update();
    }
}

void Viewer::setY(int y)
{
    if (m_y != y)
    {
        m_y = y;
        update();
    }
}

void Viewer::setZ(int z)
{
    if (m_z != z)
    {
        qDebug() << "settig z:" << z;
        m_z = z;
        update();
    }
}

void Viewer::setRotX(int rx)
{
    if (m_rotX != rx)
    {
        m_rotX = rx;
        update();
    }
}

void Viewer::setRotY(int ry)
{
    if (m_rotY != ry)
    {
        m_rotY = ry;
        update();
    }
}

void Viewer::setRotZ(int rz)
{
    if (m_rotZ != rz)
    {
        m_rotZ = rz;
        update();
    }
}

void Viewer::reset()
{
    setX(0);
    setY(0);
    setZ(0);
    setRotX(0);
    setRotY(0);
    setRotZ(0);
}

void Viewer::mouseDrag(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && m_dragging && m_dragStart != e->pos())
    {
        QPoint d = m_dragStart - e->pos();
        m_dragStart = e->pos();
        if (e->modifiers() & Qt::ShiftModifier)
        {
            m_rotY += d.y();
            m_rotX += d.x();
        }
        else
        {
            m_x += d.x();
            m_y += d.y();
        }
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *e)
{
    mouseDrag(e);
}

void Viewer::mousePressEvent(QMouseEvent *e)
{
    m_dragStart = e->pos();
    m_dragging = true;
}

void Viewer::mouseReleaseEvent(QMouseEvent *e)
{
    mouseDrag(e);
    m_dragging = false;
}

void Viewer::wheelEvent(QWheelEvent *e)
{
    qDebug() << "wheelEvent" << e->delta();
    if (e->modifiers() & Qt::ShiftModifier)
        setRotZ(m_rotZ + e->delta() / 25);
    else
        setZ(m_z + e->delta() / 25);
    update();
}

void Viewer::initializeGL()
{
    if (m_model->scene() == 0)
    {
        QMessageBox::warning(this, tr("No Models Found"),
                             tr("Could not find any model to load.  Select "
                                "Open from the File menu to navigate to a model file."));
        m_warningDisplayed = true;
    }
    else
    {
        QGLPainter painter(this);
        initializeGL(&painter);
    }
}

void Viewer::initializeGL(QGLPainter *painter)
{
    painter->setClearColor(QColor(32, 32, 128));
    painter->setDepthTestingEnabled(true);
    painter->setCullFaces(QGL::CullBackFaces);

    if (!m_lightModel)
        m_lightModel = new QGLLightModel(this);
    m_lightModel->setAmbientSceneColor(QColor(196, 196, 196));
    painter->setLightModel(m_lightModel);

    if (!m_lightParameters)
        m_lightParameters = new QGLLightParameters(this);
    m_lightParameters->setPosition(QVector3D(-0.5, 1.0, 3.0));
    painter->setMainLight(m_lightParameters);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(animate()));
    m_timer->start(25);
}

/*!
    \internal
*/
void Viewer::resizeGL(int w, int h)
{
    // Set up the standard viewport for the new window size.
    glViewport(0, 0, w, h);
    update();
}

void Viewer::paintGL()
{
    if (m_model->scene())
    {
        QGLPainter painter(this);
        paintGL(&painter);
    }
    else
    {
        if (!m_warningDisplayed)
        {
            QMessageBox::warning(this,
                                 tr("Model Error"),
                                 tr("Could not load file %1").arg(m_model->fullPath()));
            m_warningDisplayed = true;
        }
    }
}

void Viewer::paintGL(QGLPainter *painter)
{
    painter->clear();

    // Set perspective transformation and position model-view matrix
    QVector3D sceneOrigin;
    qreal maxDimension = 2.0f;

    QBox3D box = m_model->scene()->boundingBox();
    if (!box.isNull())
        sceneOrigin = box.center();
    if (box.isFinite())
    {
        QVector3D ext = box.size();
        maxDimension = qMax(ext.x(), qMax(ext.y(), ext.z()));
        m_z += maxDimension;
    }

    QGLCamera camera;
    QVector3D eye(-m_x, -m_y, -m_z);
    if (!qFuzzyCompare(eye, camera.eye()))
        camera.setEye(eye);
    if (m_spin != 0)
        camera.rotateCenter(camera.pan(m_spin));
    camera.setFarPlane(qMax(m_z + 1000, 1000));
    painter->setCamera(&camera);

    static QVector3D eyeLast;
    static qreal farLast = 0.0f;
    if (eye != eyeLast || camera.farPlane() != farLast)
    {
        qDebug() << "eye:" << eye << "far plane:" << camera.farPlane();
        eyeLast = eye;
        farLast = camera.farPlane();
    }

    painter->modelViewMatrix().push();

    QQuaternion xt = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, m_rotX);
    QQuaternion yt = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, m_rotY);
    QQuaternion zt = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, m_rotZ);
    painter->modelViewMatrix().rotate(xt * yt * zt);

    painter->modelViewMatrix().translate(-sceneOrigin);

    m_model->scene()->draw(painter);

    painter->modelViewMatrix().pop();
}

void Viewer::animate()
{
    if (m_model->scene())
    {
        if (m_animate)
        {
            m_spin = (m_spin + 1) % 360;
        }
        update();
    }
}

void Viewer::enableAnimation(bool enabled)
{
    m_animate = enabled;
    m_spin = 0;
}
