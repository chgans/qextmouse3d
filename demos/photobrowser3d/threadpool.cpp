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

#include "threadpool.h"
#include "imageloader.h"
#include "imagemanager.h"

#include <QSemaphore>

ThreadPool::ThreadPool()
{
    m_threadPoolSize = QThread::idealThreadCount();
    if (m_threadPoolSize < 1)
        m_threadPoolSize = 2;
    m_stop = 0;
}

ThreadPool::~ThreadPool()
{
    Q_ASSERT(m_allWorkers.size() == 0);
}

void ThreadPool::deployLoader(const QUrl &url)
{
    // INVARIANT: this critical section is only ever executed from its
    // own thread - thus access to it is serialized
    Q_ASSERT(QThread::currentThread() == thread());

    qDebug() << ">>>>>> ThreadPool::deployLoader(" << url.toString() << ")"
             << QThread::currentThread();

    ImageManager *manager = qobject_cast<ImageManager*>(sender());
    Q_ASSERT(manager);

    ImageLoader *loader = 0;
    if (m_freeWorkers.size() > 0)
        loader = m_freeWorkers.takeFirst();

    if (!loader && m_allWorkers.size() < m_threadPoolSize)
    {
        loader = new ImageLoader;
        m_allWorkers.append(loader);
        qDebug() << "ThreadPool::deployLoader - created new" << loader <<
                    "in thread:" << QThread::currentThread();
        connect(loader, SIGNAL(imageLoaded(ThumbnailableImage)), manager,
                SIGNAL(imageReady(ThumbnailableImage)));
        connect(loader, SIGNAL(imageLoaded(ThumbnailableImage)), this,
                SIGNAL(retrieveLoader()));
        connect(loader, SIGNAL(finished()), this, SLOT(closeLoader()));
        connect(this, SIGNAL(stopAll()), loader, SLOT(stop()));
        loader->start(QThread::IdlePriority);
    }
    if (loader)
        loader->setUrl(url);
    else
        m_workList.append(url);

    qDebug() << "<<<<<< ThreadPool::deployLoader(" << url.toString() << ")" << QThread::currentThread();
}

void ThreadPool::retrieveLoader()
{
    ImageLoader *loader = qobject_cast<ImageLoader*>(sender());
    qDebug() << "ThreadPool::retrieveLoader()" << loader;
    Q_ASSERT(loader);
    if (!m_stop)
    {
        if (!m_workList.isEmpty())
            loader->setUrl(m_workList.takeFirst());
        else
            m_freeWorkers.append(loader);
    }
}

void ThreadPool::stop()
{
    m_stop.ref();
    emit stopAll();
}

void ThreadPool::closeLoader()
{
    ImageLoader *loader = qobject_cast<ImageLoader*>(sender());
    qDebug() << "ThreadPool::closeLoader()" << loader;
    Q_ASSERT(loader);
    m_allWorkers.removeOne(loader);
    loader->deleteLater();
    if (m_allWorkers.isEmpty() && m_stop)
        emit stopped();
}
