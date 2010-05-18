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
#include <QTextItem>

#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qmath.h>

#include <math.h>

Viewer::Viewer(QWidget *parent)
    : QGLView(parent)
    , m_timer(new QTimer(this))
    , m_model(0)
    , m_lightModel(0)
    , m_lightParameters(0)
    , m_animate(false)
    , m_warningDisplayed(false)
    , m_floor(0)
    , m_drawFloor(true)
    , m_zoomScale(1)
{
    setToolTip(tr("Drag the mouse to slide the object left-right & up-down\n"
                  "or use the mouse-wheel to move the camera nearer/farther.\n"
                  "Use shift-drag or shift-mouse-wheel to rotate."));
}

Viewer::~Viewer()
{
    // nothing to do here
}

void Viewer::setZoomScale(int scale)
{
    if (scale != m_zoomScale)
    {
        m_zoomScale = scale;
    }
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

QVector3D Viewer::scale() const
{
    return m_scale;
}

void Viewer::setScale(const QVector3D &s)
{
    if (!qFuzzyCompare(s, m_scale))
    {
        m_scale = s;
        qDebug() << "scale set to:" << s;
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
    e->accept();
    QVector3D viewVec = camera()->eye() - camera()->center();
    qreal zoomMag = viewVec.length();
    qreal inc = float(m_zoomScale * e->delta()) / 50.0f;
    zoomMag += inc;
    if (zoomMag < 5.0f)
        zoomMag = 5.0f;
    QLine3D viewLine(camera()->center(), viewVec);
    camera()->setEye(viewLine.point(zoomMag));
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
    for (int x = -5; x < 5; ++x)
    {
        QGLOperation op(m_floor, QGL::QUAD_STRIP);
        for (int z = -5; z <= 5; ++z)
        {
            op << QVector3D(x, 0, z);
            op << QVector2D(float(-x - 5) / 10.0f, float(z + 5) / 10.0f);
            op << QVector3D(x + 1, 0, z);
            op << QVector2D(float(-x - 6) / 10.0f, float(z + 5) / 10.0f);
        }
    }
    for (int z = -5; z < 5; ++z)
    {
        QGLOperation op(m_floor, QGL::QUAD_STRIP);
        for (int x = -5; x <= 5; ++x)
        {
            op << QVector3D(x, -0.01, z);
            op << QVector2D(float(-x - 5) / 10.0f, float(z + 5) / 10.0f);
            op << QVector3D(x, -0.01, z + 1);
            op << QVector2D(float(-x - 5) / 10.0f, float(z + 6) / 10.0f);
        }
    }
    m_floor->finalize();
    m_floor->setEffect(QGL::LitDecalTexture2D);
    int sz = 512;
    QImage uv(sz, sz, QImage::Format_ARGB32);
    QPoint ctr(sz/2, sz/2);
    uv.fill(qRgba(128, 128, 96, 1));
    QPainter painter;
    painter.begin(&uv);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen = painter.pen();
    pen.setWidth(2.0);
    pen.setColor(qRgba(128,16,16,1));
    painter.setPen(pen);
    painter.drawEllipse(ctr, sz/2, sz/2);
    painter.drawEllipse(ctr, sz/4, sz/4);
    painter.drawEllipse(ctr, 3*sz/4, 3*sz/4);
    QPointF leftMiddle(0, sz/2);
    QPointF rightMiddle(sz, sz/2);
    painter.drawLine(leftMiddle, rightMiddle);
    QPointF bottomMiddle(sz/2, 0);
    QPointF topMiddle(sz/2, sz);
    painter.drawLine(bottomMiddle, topMiddle);
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(font.pixelSize() * 1.5);
    QFontMetrics fm = painter.fontMetrics();
    QRectF rmx = fm.boundingRect("-X");
    QRectF rx = fm.boundingRect("X");
    topMiddle.setX(topMiddle.x() + 2);
    topMiddle.setY(topMiddle.y() - rmx.height());
    bottomMiddle.setX(bottomMiddle.x() + 2);
    bottomMiddle.setY(bottomMiddle.y() + rx.height() + 2);
    leftMiddle.setX(leftMiddle.x() + 2);
    leftMiddle.setY(leftMiddle.y() + rx.height() + 2);
    rightMiddle.setX(rightMiddle.x() - rx.width() - 2);
    rightMiddle.setY(rightMiddle.y() + rx.height() + 2);
    painter.drawText(topMiddle, "-Z");
    painter.drawText(bottomMiddle, "Z");
    painter.drawText(leftMiddle, "-X");
    painter.drawText(rightMiddle, "X");
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

        if (!m_orientation.isNull())
        {
            QQuaternion xt = QQuaternion::fromAxisAndAngle(
                    QVector3D(1.0f, 0.0f, 0.0f), m_orientation.x());
            QQuaternion yt = QQuaternion::fromAxisAndAngle(
                    QVector3D(0.0f, 1.0f, 0.0f), m_orientation.y());
            QQuaternion zt = QQuaternion::fromAxisAndAngle(
                    QVector3D(0.0f, 0.0f, 1.0f), m_orientation.z());
            painter->modelViewMatrix().rotate(zt * yt * xt);
        }

        if (!m_scale.isNull())
        {
            QVector3D s = m_scale;
            if (qFuzzyIsNull(s.x()))
                s.setX(1.0f);
            else if (s.x() < 0.0f)
                s.setX(1.0f / qAbs(s.x()));
            if (qFuzzyIsNull(s.y()))
                s.setY(1.0f);
            else if (s.y() < 0.0f)
                s.setY(1.0f / qAbs(s.y()));
            if (qFuzzyIsNull(s.z()))
                s.setZ(1.0f);
            else if (s.z() < 0.0f)
                s.setZ(1.0f / qAbs(s.z()));
            painter->modelViewMatrix().scale(s);
        }

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
    resetView();
    m_animate = enabled;
}

void Viewer::resetView()
{
    QVector3D origin = camera()->center();
    QVector3D eye = camera()->eye();
    QVector3D up = camera()->upVector();
    QVector3D viewVec = eye - origin;
    qreal zoomMag = qAbs(viewVec.length());
    eye = origin;
    if (m_view == TopView)
    {
        eye.setY(eye.y() + zoomMag);
        up = QVector3D(0.0f, 0.0f, 1.0f);
    }
    else
    {
        const qreal FRONT_VIEW_ANGLE = (M_PI / 12.0f);
        qreal y = zoomMag * qSin(FRONT_VIEW_ANGLE);
        qreal z = zoomMag * qCos(FRONT_VIEW_ANGLE);
        eye = QVector3D(0.0f, y, z);
        up = QVector3D(0.0f, z, y);
    }
    camera()->setEye(eye);
    camera()->setCenter(QVector3D());
    camera()->setUpVector(up);
    update();
}
