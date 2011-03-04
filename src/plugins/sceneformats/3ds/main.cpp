/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qglsceneformatplugin.h"
#include "qgl3dsscenehandler.h"

QT_BEGIN_NAMESPACE

class QGL3dsScenePlugin : public QGLSceneFormatPlugin
{
public:
    QStringList keys() const;
    virtual QGLSceneFormatHandler *create(QIODevice *device, const QUrl& url, const QString &format) const;
};

QStringList QGL3dsScenePlugin::keys() const
{
    return QStringList() << QLatin1String("3ds") << QLatin1String("application/x-3ds") << QLatin1String("image/x-3ds");
}

QGLSceneFormatHandler *QGL3dsScenePlugin::create(QIODevice *device, const QUrl& url, const QString &format) const
{
    Q_UNUSED(device);
    Q_UNUSED(url);
    Q_UNUSED(format);
    return new QGL3dsSceneHandler;
}

Q_EXPORT_STATIC_PLUGIN(QGL3dsScenePlugin)
Q_EXPORT_PLUGIN2(qscene3ds, QGL3dsScenePlugin)

QT_END_NAMESPACE
