/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt scene graph research project.
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

#include "photobrowser3dview.h"
#include "imagemanager.h"
#include "imagedisplay.h"
#include "skybox.h"
#include "qglpicknode.h"
#include "qfocusadaptor.h"
#include "thumbnailableimage.h"
#include "qatlas.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QWheelEvent>
#include <QDir>
#include <QTimer>
#include <QTime>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QSignalTransition>
#include <QPropertyAnimation>

PhotoBrowser3DView::PhotoBrowser3DView()
    : QGLView()
    , m_scene(0)
    , m_images(new ImageManager)
    , m_skybox(0)
    , m_palette(new QGLMaterialCollection(this))
    , m_velocity(0.0f)
    , m_keyTimer(new QTimer(this))
    , m_panTime(new QTime())
    , m_state(0)
    , m_app(0)
    , m_zoomed(0)
    , m_browse(0)
    , m_pan(0)
    , m_framesDirty(true)
    , m_done(false)
    , m_closing(false)
{
    setOption(QGLView::ObjectPicking, true);

    qRegisterMetaType<ThumbnailableImage>("ThumbnailableImage");

    QString path = ":/res";
    int ix = qApp->arguments().indexOf("--skybox");
    if (ix != -1)
    {
        if (qApp->arguments().size() > ix+1)
            path = qApp->arguments().at(ix+1);
        else
            qWarning("Expected path/to/skybox/files after \"--skybox\" switch\n");
    }

    m_displaySize = 4.0;
    m_skybox = new SkyBox(this, path);
    m_scene = new ImageDisplay(this, m_palette, m_displaySize);

    setupStates();

    QTimer::singleShot(0, this, SLOT(initialise()));
}

PhotoBrowser3DView::~PhotoBrowser3DView()
{
    qDebug() << "PhotoBrowser3DView::~PhotoBrowser3DView";
    delete m_panTime;
    delete m_atlas;
}

void PhotoBrowser3DView::setupStates()
{
    m_state = new QStateMachine(this);
    m_app = new QState;
    m_zoomed = new QState(m_app);
    m_browse = new QState(m_app);
    m_pan = new QState(m_app);
    m_app->setInitialState(m_browse);
    m_state->addState(m_app);
    QFinalState *end_state = new QFinalState;
    m_app->addTransition(this, SIGNAL(done()), end_state);
    m_state->addState(end_state);
    connect(m_state, SIGNAL(finished()), this, SLOT(close()));

    m_fa = new QFocusAdaptor(this);
    m_browse->assignProperty(m_fa, "progress", 0.0);
    m_pan->assignProperty(m_fa, "progress", 0.0);
    m_zoomed->assignProperty(m_fa, "progress", 1.0);

    m_state->setObjectName("StateMachine");
    m_app->setObjectName("Application");
    m_zoomed->setObjectName("Zoomed");
    m_browse->setObjectName("Browse");
    m_pan->setObjectName("Pan");
    end_state->setObjectName("EndState");
    QObject::connect(m_state, SIGNAL(entered()), this, SLOT(stateEnter()));
    QObject::connect(m_state, SIGNAL(exited()), this, SLOT(stateExit()));
    QObject::connect(m_app, SIGNAL(entered()), this, SLOT(stateEnter()));
    QObject::connect(m_app, SIGNAL(exited()), this, SLOT(stateExit()));
    QObject::connect(m_browse, SIGNAL(entered()), this, SLOT(stateEnter()));
    QObject::connect(m_browse, SIGNAL(exited()), this, SLOT(stateExit()));
    QObject::connect(m_pan, SIGNAL(entered()), this, SLOT(stateEnter()));
    QObject::connect(m_pan, SIGNAL(exited()), this, SLOT(stateExit()));
    QObject::connect(m_zoomed, SIGNAL(entered()), this, SLOT(stateEnter()));
    QObject::connect(m_zoomed, SIGNAL(exited()), this, SLOT(stateExit()));
    QObject::connect(end_state, SIGNAL(entered()), this, SLOT(stateEnter()));
    QObject::connect(end_state, SIGNAL(exited()), this, SLOT(stateExit()));

    QSignalTransition *transition = m_browse->addTransition(this, SIGNAL(zoom()), m_zoomed);
    QPropertyAnimation *a = new QPropertyAnimation(m_fa, "progress");
    a->setDuration(500);
    a->setEasingCurve(QEasingCurve::OutQuad);
    transition->addAnimation(a);

    transition = m_pan->addTransition(this, SIGNAL(zoom()), m_zoomed);
    a = new QPropertyAnimation(m_fa, "progress");
    a->setDuration(500);
    a->setEasingCurve(QEasingCurve::InQuad);
    transition->addAnimation(a);

    transition = m_zoomed->addTransition(this, SIGNAL(zoom()), m_browse);
    a = new QPropertyAnimation(m_fa, "progress");
    a->setDuration(500);
    a->setEasingCurve(QEasingCurve::InQuad);
    transition->addAnimation(a);

    m_state->setInitialState(m_app);
    m_state->start();
}

void PhotoBrowser3DView::initialise()
{
    qDebug() << ">>>>>> PhotoBrowser3DView::initialise()" << QThread::currentThread();
    QString path = QDir::home().absoluteFilePath("Pictures");
    int ix = qApp->arguments().indexOf("--pictures");
    if (ix != -1)
    {
        if (qApp->arguments().size() > ix+1)
            path = qApp->arguments().at(ix+1);
        else
            qWarning("Expected /path/to/image/files after \"--pictures\" switch\n");
    }
    connect(m_images, SIGNAL(imageUrl(QUrl)), m_scene, SLOT(addThumbnailNode(QUrl)));
    connect(m_images, SIGNAL(finished()), this, SLOT(waitForExit()));

    connect(m_scene, SIGNAL(framesChanged()), this, SLOT(framesDirty()));

    m_atlas = new QAtlas;
    QUrl url;
    url.setScheme("file");
    url.setPath(path);
    m_images->setImageBaseUrl(url);
    m_images->setAtlas(m_atlas);
    QThread::Priority p = QThread::idealThreadCount() < 2 ?
                QThread::IdlePriority : QThread::NormalPriority;
    m_images->start(p);

    m_keyTimer->setInterval(100);
    connect(m_keyTimer, SIGNAL(timeout()),
            this, SLOT(keyTimeOut()));
    qDebug() << "<<<<<<< PhotoBrowser3DView::initialise()" << QThread::currentThread();
}

void PhotoBrowser3DView::wheelEvent(QWheelEvent *e)
{
    e->accept();
    QVector3D viewVec = camera()->eye() - camera()->center();
    qreal zoomMag = viewVec.length();
    qreal inc = float(e->delta()) / 50.0f;
    if (!qFuzzyIsNull(inc))
    {
        zoomMag += inc;
        if (zoomMag < 2.0f)
            zoomMag = 2.0f;
        QLine3D viewLine(camera()->center(), viewVec);
        camera()->setEye(viewLine.point(zoomMag));
        update();
    }
}

void PhotoBrowser3DView::keyTimeOut()
{
    m_velocity = (m_velocity < 0.0f) ? (m_velocity + 1.0f) : (m_velocity - 1.0f);
    if (qFuzzyIsNull(m_velocity))
    {
        m_velocity = 0.0f;
        m_keyTimer->stop();
    }
}

void PhotoBrowser3DView::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Space)
    {
        //emit manualControlEngaged();
    }
    else if (e->key() == Qt::Key_Q)
    {
        m_done = true;
        emit done();
    }
    else if (e->key() == Qt::Key_Left || e->key() == Qt::Key_Right)
    {
        m_velocity += (e->key() == Qt::Key_Left) ? -1.0f : 1.0f;
        if (m_velocity >= 5.0f)
            m_velocity = 5.0f;
        if (m_velocity <= -5.0f)
            m_velocity = -5.0f;
        if (qFuzzyIsNull(m_velocity))
        {
            m_velocity = 0.0f;
        }
        else
        {
            m_panTime->start();
            m_keyTimer->start();
            update();
        }
    }
    else if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)
    {
        if (e->modifiers() & Qt::ControlModifier)
        {
            QVector3D viewVec = camera()->eye() - camera()->center();
            qreal zoomMag = viewVec.length();
            zoomMag += (e->key() == Qt::Key_Up) ? -0.5f : 0.5f;
            if (zoomMag < 5.0f)
                zoomMag = 5.0f;
            QLine3D viewLine(camera()->center(), viewVec);
            camera()->setEye(viewLine.point(zoomMag));
            update();
        }
        else
        {
            QGLView::keyPressEvent(e);
        }
    }
    else if (e->key() == Qt::Key_Escape)
    {
        qDumpScene(m_scene);
        QGLView::keyPressEvent(e);
        //resetView();
        //emit manualControlEngaged();
    }
    else
    {
        QGLView::keyPressEvent(e);
    }
}

void PhotoBrowser3DView::waitForExit()
{
    qDebug() << "PhotoBrowser3DView::waitForExit";
    QThread::yieldCurrentThread();
    qDebug() << "    waiting for ImageManager to exit";
    m_images->wait();
    m_images->deleteLater();
    m_images = 0;
    if (m_closing)
    {
        if (!m_done)
        {
            emit done();
            m_done = true;
        }
    }
    qDebug() << "    done with wait - exiting";
}

void PhotoBrowser3DView::closeEvent(QCloseEvent *e)
{
    qDebug() << ">>> PhotoBrowser3DView::closeEvent";
    if (m_images)
    {
        e->ignore();
        qDebug() << "     closeEvent() - signalling stop";
        m_images->stop();
        qDebug() << "     closeEvent() - signalling waitForExit of ImageManager";

        // this was a request to close the main window, so we are closing up shop
        // set this flag to indicate that when the image manager stops done event
        // should be signalled to the state machine, resulting in close
        m_closing = true;
    }
    else
    {
        qDebug() << "ImageManager cleaned up - accepting close";
        e->accept();
    }
    qDebug() << "<<< PhotoBrowser3DView::closeEvent";
}

void PhotoBrowser3DView::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    registerFrames();
    QGLView::mousePressEvent(e);
}

void PhotoBrowser3DView::initializeGL(QGLPainter *painter)
{
    Q_UNUSED(painter);
    camera()->setEye(QVector3D(0.0f, 0.0f, 4.0f * m_displaySize));
    registerFrames();
}

void PhotoBrowser3DView::earlyPaintGL(QGLPainter *)
{
    if (!m_done)
    {
        qreal t = m_panTime->restart();
        if (!qIsNull(m_velocity))
        {
            qreal distance = t * m_velocity / 1000.0f;
            QVector3D tx = camera()->translation(distance, 0.0f, 0.0f);
            camera()->setEye(camera()->eye() + tx);
            camera()->setCenter(camera()->center() + tx);
        }
    }
}

void PhotoBrowser3DView::paintGL(QGLPainter *painter)
{
    if (!m_done)
    {
        //qDebug() << "paintGL -- eye:" << camera()->eye()
        //            << "center:" << camera()->center();
        painter->setClearColor(Qt::blue);
        glEnable(GL_BLEND);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_skybox->draw(painter);
        m_scene->draw(painter);
    }
}

/*
void PhotoBrowser3DView::paintEvent(QPaintEvent *e)
{
    // TODO: paint 2D controls on top
    //m_painter->updateContent();
    View::paintEvent(e);
}
*/

void PhotoBrowser3DView::zoomImage()
{
    QGLPickNode *pn = qobject_cast<QGLPickNode*>(sender());
    Q_ASSERT(pn);
    QGLSceneNode *n = pn->target();
    m_fa->setTarget(n);
    qDebug() << "emitting zoom";
    emit zoom();
    qDebug() << "    done";
}

void PhotoBrowser3DView::framesDirty()
{
    m_framesDirty = true;
}

void PhotoBrowser3DView::stateEnter()
{
    qDebug() << "Entered state:" << sender()->objectName();
}

void PhotoBrowser3DView::stateExit()
{
    qDebug() << "Exited state:" << sender()->objectName();
}

void PhotoBrowser3DView::registerFrames()
{
    if (m_framesDirty)
    {
        QList<QGLPickNode*> pickList = m_scene->pickNodes();
        QList<QGLPickNode*>::const_iterator it = pickList.constBegin();
        for ( ; it != pickList.constEnd(); ++it)
        {
            QGLPickNode *pn = *it;
            pn->disconnect(this);
            QObject::connect(pn, SIGNAL(clicked()),
                             this, SLOT(zoomImage()));
            registerObject(pn->id(), pn);
        }
        m_framesDirty = false;
    }
}
