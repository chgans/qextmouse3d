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
#include "qglcameraanimation.h"
#include "qglpainter.h"
#include "qglbuilder.h"
#include "qglpicknode.h"
#include "qglabstractscene.h"
#include "qmouse3devent.h"
#include "qmouse3deventprovider.h"

#include <QtGui/qmessagebox.h>
#include <QtGui/qevent.h>
#include <QTextItem>

#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qmath.h>
#include <QtCore/qpropertyanimation.h>

#include <math.h>

Viewer::Viewer(QWidget *parent)
    : QGLView(parent)
    , m_model(0)
    , m_treeView(0)
    , m_lightModel(0)
    , m_lightParameters(0)
    , m_animate(false)
    , m_warningDisplayed(false)
    , m_floor(0)
    , m_drawFloor(true)
    , m_zoomScale(1)
    , m_pickDirty(true)
    , m_lastWasZero(true)
    , m_spinAngle(0.0f)
{
    m_eventProvider = new QMouse3DEventProvider(this);
    m_eventProvider->setWidget(this);
    m_eventProvider->toggleFilter(QMouse3DEventProvider::Translations);
    m_eventProvider->toggleFilter(QMouse3DEventProvider::DominantAxis);
    m_eventProvider->setKeyFilters(QMouse3DEventProvider::Sensitivity);
    m_lastEventTime.start();

    m_cameraAnimation = new QGLCameraAnimation(this);
    m_cameraAnimation->setCamera(camera());

    m_spinAnimation = new QPropertyAnimation(this, "spinAngle", this);
    m_spinAnimation->setStartValue(qreal(0.0f));
    m_spinAnimation->setEndValue(qreal(360.0f));
    m_spinAnimation->setLoopCount(-1);
    m_spinAnimation->setDuration(5000);

    setOption(QGLView::ObjectPicking, true);
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
        connect(model, SIGNAL(modelLoaded(QString)),
                this, SLOT(registerPicking()));
        connect(model, SIGNAL(modelNotLoaded(QString)),
                this, SLOT(resetWarnings()));
        connect(model, SIGNAL(modelUnloaded(QString)),
                this, SLOT(unregisterPicking()));
        update();
        m_pickDirty = true;
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
        m_pickDirty = true;
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
        m_pickDirty = true;
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
        update();
        m_pickDirty = true;
    }
}

void Viewer::engageManualControl()
{
    if (m_view != SelectView) {
        m_view = SelectView;
        emit viewTypeChanged();
    }
    emit manualControlEngaged();
}

void Viewer::setView(View view)
{
    if (m_view != view && view != SelectView)
    {
        m_view = view;
        resetView();
        if (m_animate)
            m_view = SelectView;
        emit viewTypeChanged();
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
    engageManualControl();
    QGLView::mousePressEvent(e);
}

void Viewer::mouseReleaseEvent(QMouseEvent *e)
{
    engageManualControl();
    QGLView::mouseReleaseEvent(e);
}

void Viewer::wheelEvent(QWheelEvent *e)
{
    engageManualControl();
    QGLView::wheelEvent(e);
}

void Viewer::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Space:
        engageManualControl();
        break;

    case Qt::Key_Escape:
        resetView();
        engageManualControl();
        break;

    case QGL::Key_FrontView:
        setView(FrontView);
        break;

    case QGL::Key_BackView:
        setView(BackView);
        break;

    case QGL::Key_TopView:
        setView(TopView);
        break;

    case QGL::Key_BottomView:
        setView(BottomView);
        break;

    case QGL::Key_LeftView:
        setView(LeftView);
        break;

    case QGL::Key_RightView:
        setView(RightView);
        break;

    case QGL::Key_ISO1:
        setView(FrontRightView);
        break;

    case QGL::Key_ISO2:
        setView(BackLeftView);
        break;

    case QGL::Key_RotateCW90:
        rotateView(-90.0f);
        break;

    case QGL::Key_RotateCCW90:
        rotateView(90.0f);
        break;

    default:
        QGLView::keyPressEvent(e);
        break;
    }
}

void Viewer::rotateView(qreal angle)
{
    QQuaternion q = camera()->roll(angle);
    QVector3D upVector = q.rotatedVector(camera()->upVector());

    m_cameraAnimation->stop();
    m_cameraAnimation->setStartEye(camera()->eye());
    m_cameraAnimation->setStartUpVector(camera()->upVector());
    m_cameraAnimation->setStartCenter(camera()->center());
    m_cameraAnimation->setEndEye(camera()->eye());
    m_cameraAnimation->setEndUpVector(upVector);
    m_cameraAnimation->setEndCenter(camera()->center());
    m_cameraAnimation->setDuration(500);
    m_cameraAnimation->setEasingCurve(QEasingCurve::OutQuad);
    m_cameraAnimation->start();

    engageManualControl();
}

bool Viewer::event(QEvent *e)
{
    if (e->type() == QMouse3DEvent::type) {
        // Convert the input values into angular velocity at the
        // rate of 0.008 radians per second.  Also re-order the
        // incoming values into a right-handed co-ordinate system.
        QMouse3DEvent *event = static_cast<QMouse3DEvent *>(e);
        //qreal translateX = 0.008f * event->translateX();
        //qreal translateY = -0.008f * event->translateZ();
        //qreal translateZ = 0.008f * event->translateY();
        qreal rotateX = 0.008f * event->rotateX();
        qreal rotateY = -0.008f * event->rotateZ();
        qreal rotateZ = 0.008f * event->rotateY();

        // Determine the number of milliseconds since the last 3D mouse event.
        int elapsed;
        if (m_lastWasZero) {
            elapsed = 10;
        } else {
            elapsed = m_lastEventTime.elapsed();
            if (elapsed > 300)
                elapsed = 10;   // Probably was released.
        }

        // Create a quaternion representing the rotation.
        QQuaternion q;
        if (rotateX != 0.0f || rotateY != 0.0f || rotateZ != 0.0f) {
            qreal angleX = (rotateX * elapsed * 180.0f) / (1000.0f * M_PI);
            qreal angleY = (rotateY * elapsed * 180.0f) / (1000.0f * M_PI);
            qreal angleZ = (rotateZ * elapsed * 180.0f) / (1000.0f * M_PI);
            QGLCamera *camera = this->camera();
            q = camera->tilt(-angleX) *
                camera->pan(-angleY) *
                camera->roll(angleZ);
            camera->rotateCenter(q);
            m_lastWasZero = false;
            m_lastEventTime.restart();
        } else {
            m_lastWasZero = true;
        }

        engageManualControl();
        return true;
    }
    return QGLView::event(e);
}

void Viewer::buildFloor()
{
    if (m_floor)
        return;
    QGLBuilder builder;
    for (int x = -5; x < 5; ++x)
    {
        QGeometryData op;
        for (int z = -5; z <= 5; ++z)
        {
            op.appendVertex(QVector3D(x, 0, z));
            op.appendTexCoord(QVector2D(float(-x - 5) / 10.0f, float(z + 5) / 10.0f));
            op.appendVertex(QVector3D(x + 1, 0, z));
            op.appendTexCoord(QVector2D(float(-x - 6) / 10.0f, float(z + 5) / 10.0f));
        }
        builder.addQuadStrip(op);
    }
    for (int z = -5; z < 5; ++z)
    {
        QGeometryData op;
        for (int x = -5; x <= 5; ++x)
        {
            op.appendVertex(QVector3D(x, -0.01, z));
            op.appendTexCoord(QVector2D(float(-x - 5) / 10.0f, float(z + 5) / 10.0f));
            op.appendVertex(QVector3D(x, -0.01, z + 1));
            op.appendTexCoord(QVector2D(float(-x - 5) / 10.0f, float(z + 6) / 10.0f));
        }
        builder.addQuadStrip(op);
    }
    m_floor = builder.finalizedSceneNode();
    m_floor->setParent(this);
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
    font.setPixelSize(sz / 20);
    painter.setFont(font);
    QFontMetrics fm = painter.fontMetrics();
    QRectF rmx = fm.boundingRect(QLatin1String("-X"));
    QRectF rx = fm.boundingRect(QLatin1String("X"));
    topMiddle.setX(topMiddle.x() + 2);
    topMiddle.setY(topMiddle.y() - rmx.height());
    bottomMiddle.setX(bottomMiddle.x() + 2);
    bottomMiddle.setY(bottomMiddle.y() + rx.height() + 2);
    leftMiddle.setX(leftMiddle.x() + 4);
    leftMiddle.setY(leftMiddle.y() + rx.height() + 2);
    rightMiddle.setX(rightMiddle.x() - rx.width() - 4);
    rightMiddle.setY(rightMiddle.y() + rx.height() + 2);
    painter.drawText(topMiddle, QLatin1String("-Z"));
    painter.drawText(bottomMiddle, QLatin1String("Z"));
    painter.drawText(leftMiddle, QLatin1String("-X"));
    painter.drawText(rightMiddle, QLatin1String("X"));
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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    buildFloor();

    if (!m_lightModel)
        m_lightModel = new QGLLightModel(this);
    m_lightModel->setAmbientSceneColor(QColor(196, 196, 196));
    painter->setLightModel(m_lightModel);

    if (!m_lightParameters)
        m_lightParameters = new QGLLightParameters(this);
    m_lightParameters->setPosition(QVector3D(-0.5, 1.0, 3.0));
    painter->setMainLight(m_lightParameters);
}

void Viewer::paintGL(QGLPainter *painter)
{
    if (m_model->scene())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        painter->modelViewMatrix().push();

        if (m_spinAngle != 0.0f) {
            painter->modelViewMatrix().rotate
                (m_spinAngle, camera()->upVector());
        }

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
            m_warningDisplayed = true;
            QMessageBox::warning(this,
                                 tr("Model Error"),
                                 tr("Could not load file %1").arg(m_model->fullPath()));                        
        }
    }
}

/*
void Viewer::pickGL(QGLPainter *painter)
{
    if (m_model->scene())
    {
        painter->clear();
        painter->modelViewMatrix().push();
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
    m_pickDirty = false;
}

bool Viewer::needsPickGL()
{
    return m_pickDirty;
}
*/

void Viewer::enableAnimation(bool enabled)
{
    if (m_animate == enabled)
        return;
    m_animate = enabled;
    if (m_view != SelectView) {
        m_view = SelectView;
        emit viewTypeChanged();
    }
    if (enabled) {
        m_spinAngle = 0.0f;
        m_spinAnimation->start();
    } else {
        m_spinAnimation->stop();
        camera()->rotateCenter(camera()->pan(-m_spinAngle));
        m_spinAngle = 0.0f;
    }
}

void Viewer::resetView()
{
    QVector3D origin = camera()->center();
    QVector3D eye = camera()->eye();
    QVector3D up = camera()->upVector();
    QVector3D viewVec = eye - origin;
    qreal zoomMag = qAbs(viewVec.length());
    switch (m_view) {
    case SelectView: break;

    case TopView:
        eye = QVector3D(0.0f, zoomMag, 0.0f);
        up = QVector3D(0.0f, 0.0f, -1.0f);
        break;

    case BottomView:
        eye = QVector3D(0.0f, -zoomMag, 0.0f);
        up = QVector3D(0.0f, 0.0f, 1.0f);
        break;

    case FrontView:
        eye = QVector3D(0.0f, 0.0f, zoomMag);
        up = QVector3D(0.0f, 1.0f, 0.0f);
        break;

    case BackView:
        eye = QVector3D(0.0f, 0.0f, -zoomMag);
        up = QVector3D(0.0f, 1.0f, 0.0f);
        break;

    case LeftView:
        eye = QVector3D(zoomMag, 0.0f, 0.0f);
        up = QVector3D(0.0f, 1.0f, 0.0f);
        break;

    case RightView:
        eye = QVector3D(-zoomMag, 0.0f, 0.0f);
        up = QVector3D(0.0f, 1.0f, 0.0f);
        break;

    case FrontRightView:
        eye = zoomMag * QVector3D(-1, 1, 1).normalized();
        up = QVector3D(0.0f, 1.0f, 0.0f);
        break;

    case BackLeftView:
        eye = zoomMag * QVector3D(1, 1, -1).normalized();
        up = QVector3D(0.0f, 1.0f, 0.0f);
        break;
    }

    // Perform a short animation from the current camera position
    // to the new position.
    m_cameraAnimation->stop();
    m_cameraAnimation->setStartEye(camera()->eye());
    m_cameraAnimation->setStartUpVector(camera()->upVector());
    m_cameraAnimation->setStartCenter(camera()->center());
    m_cameraAnimation->setEndEye(eye);
    m_cameraAnimation->setEndUpVector(up);
    m_cameraAnimation->setEndCenter(QVector3D());
    m_cameraAnimation->setDuration(500);
    m_cameraAnimation->setEasingCurve(QEasingCurve::OutQuad);
    m_cameraAnimation->start();
}

void Viewer::resetWarnings()
{
    m_warningDisplayed = false;
}

void Viewer::registerPicking()
{
    m_warningDisplayed = false;
    QGLAbstractScene *manager = m_model->manager();
    QList<QGLPickNode *>nodes = manager->pickNodes();
    QList<QGLPickNode *>::const_iterator it = nodes.constBegin();
    for ( ; it != nodes.constEnd(); ++it)
    {
        QGLPickNode *node = *it;
        registerObject(node->id(), node);
        connect(node, SIGNAL(clicked()),
                this, SLOT(objectPicked()));
    }
}

void Viewer::unregisterPicking()
{
    QGLAbstractScene *manager = m_model->manager();
    if (manager) {
        QList<QGLPickNode *>nodes = manager->pickNodes();
        QList<QGLPickNode *>::const_iterator it = nodes.constBegin();
        for ( ; it != nodes.constEnd(); ++it)
        {
            QGLPickNode *node = *it;
            deregisterObject(node->id());
        }
    }
}

void Viewer::objectPicked()
{
    Q_ASSERT(m_treeView);
    QGLPickNode *node = qobject_cast<QGLPickNode*>(sender());
    Q_ASSERT(node);
    QGLSceneNode *target = qobject_cast<QGLSceneNode*>(node->target());
    QModelIndex ix = m_model->selectNode(target);
    m_treeView->expand(ix);
    m_treeView->selectionModel()->clearSelection();
    m_treeView->selectionModel()->select(ix, QItemSelectionModel::Select);
    qDebug() << "Picked:" << node->target();
}
