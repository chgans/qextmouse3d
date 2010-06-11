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

#include <QApplication>
#include <QDesktopWidget>
#include <QWheelEvent>
#include <QDir>
#include <QTimer>
#include <QTime>

PhotoBrowser3DView::PhotoBrowser3DView()
    : QGLView()
    , m_scene(0)
    , m_images(new ImageManager(this))
    , m_skybox(0)
    , m_palette(new QGLMaterialCollection(this))
    , m_velocity(0.0f)
    , m_keyTimer(new QTimer(this))
    , m_panTime(new QTime())
{
    QString path = ":/res";
    int ix = qApp->arguments().indexOf("--skybox");
    if (ix != -1)
    {
        if (qApp->arguments().size() > ix+1)
            path = qApp->arguments().at(ix+1);
        else
            qWarning("Expected path/to/skybox/files after \"--skybox\" switch\n");
    }

    m_skybox = new SkyBox(this, path);
    m_scene = new ImageDisplay(this, m_palette);

    QTimer::singleShot(0, this, SLOT(initialise()));

    m_keyTimer->setInterval(100);
    connect(m_keyTimer, SIGNAL(timeout()),
            this, SLOT(keyTimeOut()));
}

PhotoBrowser3DView::~PhotoBrowser3DView()
{
    delete m_panTime;
}

void PhotoBrowser3DView::initialise()
{
    QString path = QDir::home().absoluteFilePath("Pictures");
    int ix = qApp->arguments().indexOf("--pictures");
    if (ix != -1)
    {
        if (qApp->arguments().size() > ix+1)
            path = qApp->arguments().at(ix+1);
        else
            qWarning("Expected path/to/image/files after \"--pictures\" switch\n");
    }
    connect(m_images, SIGNAL(imageReady(QImage)),
            m_scene, SLOT(addImage(QImage)));
    QUrl url;
    url.setScheme("file");
    url.setPath(path);
    m_images->setImageUrl(url);
    m_images->start();
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
        if (zoomMag < 5.0f)
            zoomMag = 5.0f;
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
        close();
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

void PhotoBrowser3DView::closeEvent(QCloseEvent *e)
{
    m_images->quit();
    m_images->wait();
    QWidget::closeEvent(e);
}

void PhotoBrowser3DView::initializeGL(QGLPainter *painter)
{
    Q_UNUSED(painter);
    //camera()->setEye(QVector3D(0.0f, 0.0f, -10.0f));
}

void PhotoBrowser3DView::earlyPaintGL(QGLPainter *)
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

void PhotoBrowser3DView::paintGL(QGLPainter *painter)
{    
    painter->setClearColor(Qt::blue);
    painter->setBlendingEnabled(true);
    painter->clear();
    m_skybox->draw(painter);
    m_scene->draw(painter);
}

/*
void PhotoBrowser3DView::paintEvent(QPaintEvent *e)
{
    // TODO: paint 2D controls on top
    //m_painter->updateContent();
    View::paintEvent(e);
}
*/
