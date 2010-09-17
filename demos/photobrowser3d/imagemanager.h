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


#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QThread>
#include <QUrl>
#include <QImage>
#include <QMutex>

#include "thumbnailableimage.h"

class Launcher;
class QSemaphore;
class QAtlas;

class ImageManager : public QThread
{
    Q_OBJECT
public:
    explicit ImageManager(QObject *parent = 0);
    void setImageBaseUrl(const QUrl &url) { m_url = url; }
    QUrl imageBaseUrl() const { return m_url; }
    QAtlas *atlas() const { return m_atlas; }
signals:
    void imageUrl(const QUrl &);
    void imageReady(const ThumbnailableImage &);
    void errorOccurred(const QString &);
public slots:
    void acquire();
    void release();
    void createLoader(const QUrl &);
protected:
    void run();
private slots:
    void incrementCounter();
private:
    QUrl m_url;
    QSemaphore *m_sem;
    int m_threadPoolSize;
    Launcher *m_launcher;
    int m_count;
    QAtlas *m_atlas;
};

#endif // IMAGEMANAGER_H
