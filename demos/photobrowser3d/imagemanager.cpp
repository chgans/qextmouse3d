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
#include "qatlas.h"

#include <QSemaphore>
#include <QDir>
#include <QTime>
#include <QTimer>
#include <QDebug>

#ifndef Q_MAX_CONCURRENT_LOADERS
#define Q_MAX_CONCURRENT_LOADERS 8
#endif

ImageManager::ImageManager(QObject *parent)
    : QThread(parent)
    , m_sem(0)
    , m_threadPoolSize(0)
    , m_launcher(0)
    , m_atlas(new QAtlas)
{
    m_threadPoolSize = QThread::idealThreadCount();
    if (m_threadPoolSize == -1)
        m_threadPoolSize = Q_MAX_CONCURRENT_LOADERS + 2;
    // need at least 3 threads - manager, launcher & one loader
    Q_ASSERT(m_threadPoolSize > 2);
    // debug ---- remove me
    m_threadPoolSize = 3;
    --m_sem;  // take one thread out for this manager objects run()
    m_sem = new QSemaphore(m_threadPoolSize);
}

void ImageManager::acquire()
{
    // block if threads running > m_threadPoolSize
    QTime time;
    time.start();
    m_sem->acquire();
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
    if (m_sem->available() == m_threadPoolSize)
    //    quit();
}

void ImageManager::createLoader(const QUrl &url)
{
    ImageLoader *loader = new ImageLoader(this);
    loader->setUrl(url);
    connect(loader, SIGNAL(finished()), this, SLOT(release()));
    connect(loader, SIGNAL(imageLoaded(ThumbnailableImage)), this,
            SIGNAL(imageReady(ThumbnailableImage)));
    connect(loader, SIGNAL(imageLoaded(ThumbnailableImage)), this, SLOT(incrementCounter()));
    loader->start(QThread::IdlePriority);
}

void ImageManager::incrementCounter()
{
    m_count++;
}

void ImageManager::run()
{
    m_count = 0;
    m_launcher = new Launcher(this);
    m_launcher->setUrl(m_url);
    connect(m_launcher, SIGNAL(imageUrl(QUrl)),
            this, SIGNAL(imageUrl(QUrl)));
    connect(m_launcher, SIGNAL(finished()),
            this, SLOT(release()));
    acquire();   // grab a thread for the launcher
    m_launcher->start(QThread::IdlePriority);
    //fprintf(stderr, "ImageManager::run - start %p\n", this);
    QTime timer;
    timer.start();
    exec();
    fprintf(stderr, "ImageManager::run - %d images loaded from %s in %d ms\n",
            m_count, qPrintable(m_url.path()), timer.elapsed());
    delete m_launcher;
    m_launcher = 0;
}
