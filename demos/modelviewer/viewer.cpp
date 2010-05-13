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
#include "qgldisplaylist.h"
#include "qgloperation.h"

#include <QtGui/qmessagebox.h>
#include <QtGui/qevent.h>

#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>

#include <math.h>

Viewer::Viewer(QWidget *parent)
    : QGLView(parent)
    , m_timer(new QTimer(this))
    , m_model(0)
    , m_lightModel(0)
    , m_lightParameters(0)
    , m_animate(true)
    , m_warningDisplayed(false)
    , m_floor(0)
    , m_drawFloor(true)
{
    setToolTip(tr("Drag the mouse to slide the object left-right & up-down\n"
                  "or use the mouse-wheel to move the camera nearer/farther.\n"
                  "Use shift-drag or shift-mouse-wheel to rotate."));
}

Viewer::~Viewer()
{
    // nothing to do here
}

void Viewer::setModel(Model *model)
{
    if (m_model != model)
    {
        m_model = model;
        update();
    }
}

QVector3D Viewer::position() const
{
    return m_position;
}

void Viewer::setPosition(const QVector3D &t)
{
    if (!qFuzzyCompare(t, m_position))
    {
        m_position = t;
        update();
    }
}

QVector3D Viewer::orientation() const
{
    return m_orientation;
}

void Viewer::setOrientation(const QVector3D &r)
{
    if (!qFuzzyCompare(r, m_orientation))
    {
        m_orientation = r;
        update();
    }
}

void Viewer::setView(View view)
{
    if (m_view != view)
    {
        m_view = view;
        resetView();
    }
}

void Viewer::setFloorEnabled(bool enable)
{
    if (m_drawFloor != enable)
    {
        m_drawFloor = enable;
        update();
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *e)
{
    emit manualControlEngaged();
    QGLView::mouseMoveEvent(e);
}

void Viewer::mousePressEvent(QMouseEvent *e)
{
    emit manualControlEngaged();
    QGLView::mousePressEvent(e);
}

void Viewer::mouseReleaseEvent(QMouseEvent *e)
{
    emit manualControlEngaged();
    QGLView::mouseReleaseEvent(e);
}

void Viewer::wheelEvent(QWheelEvent *e)
{
    emit manualControlEngaged();
    QGLView::wheelEvent(e);
}

void Viewer::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Space)
    {
        emit manualControlEngaged();
    }
    else if (e->key() == Qt::Key_Escape)
    {
        resetView();
        emit manualControlEngaged();
    }
    else
    {
        QGLView::keyPressEvent(e);
    }
}

void Viewer::buildFloor()
{
    m_floor = new QGLDisplayList(this);
    m_floor->newSection();
    for (int z = -5; z < 5; ++z)
    {
        QGLOperation op(m_floor, QGL::QUAD_STRIP);
        for (int x = -5; x <= 5; ++x)
        {
            op << QVector3D(x, 0, z+1);
            op << QVector2D(float(x+5) / 10.0f, float(z+6) / 10.0f);
            op << QVector3D(x, 0, z);
            op << QVector2D(float(x+5) / 10.0f, float(z+5) / 10.0f);
        }
    }
    m_floor->finalize();
    int sz = 512;
    QImage uv(sz, sz, QImage::Format_ARGB32);
    QPoint ctr(sz/2, sz/2);
    uv.fill(qRgba(196, 212, 212, 0));
    QPainter painter;
    painter.begin(&uv);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen = painter.pen();
    pen.setWidth(2.0);
    pen.setColor(qRgba(128,16,16,0));
    painter.setPen(pen);
    painter.drawEllipse(ctr, sz/2, sz/2);
    painter.drawEllipse(ctr, sz/4, sz/4);
    painter.drawEllipse(ctr, 3*sz/4, 3*sz/4);
    painter.drawLine(QPointF(0, sz/2), QPointF(sz, sz/2));
    painter.drawLine(QPointF(sz/2, 0), QPointF(sz/2, sz));
    painter.end();
    QGLMaterial *mat = new QGLMaterial(m_floor);
    QGLTexture2D *tex = new QGLTexture2D(mat);
    tex->setImage(uv);
    mat->setTexture(tex);
    m_floor->setMaterial(mat);
}

void Viewer::initializeGL(QGLPainter *painter)
{
    setFocus();
    painter->setClearColor(QColor(32, 32, 128));
    painter->setDepthTestingEnabled(true);
    painter->setCullFaces(QGL::CullBackFaces);

    buildFloor();

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

void Viewer::paintGL(QGLPainter *painter)
{
    if (m_model->scene())
    {
        painter->clear();

        painter->modelViewMatrix().push();

        if (m_drawFloor)
            m_floor->draw(painter);

        painter->modelViewMatrix().translate(m_position);

        m_model->scene()->draw(painter);

        painter->modelViewMatrix().pop();
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

void Viewer::animate()
{
    if (m_model->scene())
    {
        if (m_animate)
            camera()->panCenter(0.5f);
    }
}

void Viewer::enableAnimation(bool enabled)
{
    m_animate = enabled;
}

void Viewer::resetView()
{
    QVector3D e = camera()->eye();
    QVector3D c = camera()->center();
    qreal z = (e - c).length();
    camera()->setEye(QVector3D(0.0f, 0.0f, -z));
    qreal tilt = (m_view == Viewer::TopView) ? 90.0 : 15.0;
    camera()->tiltCenter(tilt);
    update();
}
