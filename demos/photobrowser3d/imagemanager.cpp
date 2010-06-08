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


#include "imagemanager.h"
#include "imageloader.h"
#include "launcher.h"

#include <QSemaphore>
#include <QDir>
#include <QTime>
#include <QDebug>

#include <QTimer>

#ifndef Q_MAX_CONCURRENT_LOADERS
#define Q_MAX_CONCURRENT_LOADERS 8
#endif

ImageManager::ImageManager(QObject *parent)
    : QThread(parent)
    , m_sem(0)
    , m_threadPoolSize(0)
{
    m_threadPoolSize = QThread::idealThreadCount();
    qDebug() << "System reported ideal thread count:" << m_threadPoolSize;
    if (m_threadPoolSize == -1)
        m_threadPoolSize = Q_MAX_CONCURRENT_LOADERS + 2;
    // need at least 3 threads - manager, launcher & one loader
    Q_ASSERT(m_threadPoolSize > 2);
    --m_sem;  // take one thread out for this manager objects run()
    m_sem = new QSemaphore(m_threadPoolSize);
    connect(this, SIGNAL(done()),
            this, SLOT(quit()), Qt::QueuedConnection);
}

void ImageManager::acquire()
{
    // block if threads running > m_maxThreads
    QTime time;
    time.start();
    m_sem->acquire();
    qDebug() << "ImageManager::acquire took:" << time.elapsed() << " - now:"
            << (m_threadPoolSize - m_sem->available()) << "threads running";
}

void ImageManager::release()
{
    ImageLoader *loader = qobject_cast<ImageLoader*>(sender());
    if (loader)
    {
        Q_ASSERT(loader->isFinished());
        loader->deleteLater();  // delete on return to event loop
    }
    m_sem->release();
    qDebug() << "ImageManager::release - now:"
            << (m_threadPoolSize - m_sem->available()) << "threads running";
    if (m_sem->available() == m_threadPoolSize)
    {
        qDebug() << "Calling quit - in thread:" << QThread::currentThreadId();
        emit done();
    }
}

void ImageManager::createLoader(const QUrl &url)
{
    ImageLoader *loader = new ImageLoader(this);
    loader->setUrl(url);
    connect(loader, SIGNAL(finished()),
            this, SLOT(release()), Qt::QueuedConnection);
    connect(loader, SIGNAL(imageLoaded(QImage)),
            this, SIGNAL(imageReady(QImage)), Qt::QueuedConnection);
    loader->start();
}

void ImageManager::run()
{
    qDebug() << "starting ImageManager" << m_url << "in thread:" << QThread::currentThreadId();
    Launcher launcher(this);
    launcher.setUrl(m_url);
    connect(&launcher, SIGNAL(imageUrl(QUrl)),
            this, SLOT(createLoader(QUrl)));
    connect(&launcher, SIGNAL(finished()),
            this, SLOT(release()));
    acquire();   // grab a thread for the launcher
    launcher.start();
    exec();
    qDebug() << "exiting ImageManager" << m_url;
}
