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


#include "launcher.h"
#include "imageloader.h"
#include "imagemanager.h"

#include <QSemaphore>
#include <QTime>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QSet>
#include <QDebug>

#ifndef Q_MAX_CONCURRENT_LOADERS
#define Q_MAX_CONCURRENT_LOADERS 8
#endif

Launcher::Launcher()
    : m_sem(0)
    , m_threadPoolSize(0)
    , m_freeWorkers(0)
    , m_allWorkers(0)
{
    m_threadPoolSize = QThread::idealThreadCount();
    if (m_threadPoolSize < 3)
        m_threadPoolSize = 3;
    if (m_threadPoolSize > Q_MAX_CONCURRENT_LOADERS)
        m_threadPoolSize = Q_MAX_CONCURRENT_LOADERS;

    // take a thread out of the pool for myself
    m_sem = new QSemaphore(m_threadPoolSize - 2);
}

void Launcher::acquire()
{
    // block if threads running > m_threadPoolSize
    qDebug() << ">>>>>> @@@@@@ ----- acquiring..." << QThread::currentThread();
    QTime time;
    time.start();
    m_sem->acquire();
    qDebug() << "<<<<<< @@@@@@ ----- acquired!  In" << time.elapsed() << "ms" << QThread::currentThread();
}

void Launcher::release()
{
    ImageLoader *loader = qobject_cast<ImageLoader*>(sender());
    if (loader)
    {
        Q_ASSERT(loader->isFinished());
        m_freeWorkers->append(loader);
        qDebug() << "ImageManager::release" << loader << QThread::currentThread();
    }
    m_sem->release();
}

ImageLoader *Launcher::getLoader()
{
    ImageLoader *loader = m_freeWorkers->takeFirst();
    if (loader == 0)
    {
        loader = new ImageLoader;
        m_allWorkers->append(loader);
        qDebug() << "ImageManager::getLoader() - created new" << loader << "in thread:"
                 << QThread::currentThread();
    }
    return loader;
}

void Launcher::createLoader(const QUrl &url)
{
    ImageManager *manager = qobject_cast<ImageManager*>(sender());
    Q_ASSERT(manager);

    qDebug() << ">>>>>> ImageManager::createLoader(" << url.toString() << ")" << QThread::currentThread();
    acquire();
    ImageLoader *loader = getLoader();
    loader->setUrl(url);
    connect(loader, SIGNAL(finished()), this, SLOT(release()));
    connect(loader, SIGNAL(imageLoaded(ThumbnailableImage)), manager, SIGNAL(imageReady(ThumbnailableImage)));
    connect(loader, SIGNAL(imageLoaded(ThumbnailableImage)), this, SLOT(incrementCounter()));
    loader->start(QThread::IdlePriority);
    qDebug() << "<<<<<< ImageManager::createLoader(" << url.toString() << ")" << QThread::currentThread();
}

void Launcher::incrementCounter()
{
    m_count++;
}

void Launcher::run()
{
    qDebug() << ">>>>>>> Launcher::run" << m_url << QThread::currentThread();

    if (m_url.scheme() != "file")
    {
        qWarning("URL scheme %s not yet supported", qPrintable(m_url.scheme()));
        return;
    }

    SynchronizedList<ImageLoader> freeWorkers;
    SynchronizedList<ImageLoader> allWorkers;

    m_freeWorkers = &freeWorkers;
    m_allWorkers = &allWorkers;

    exec();

    freeWorkers.clear();  // otherwise destructor will try double delete

    qDebug() << "<<<<<< Launcher::run" << m_url << QThread::currentThread();
}
