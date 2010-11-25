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

#include "qglsceneformatplugin.h"
#include "qaiscenehandler.h"

#include "assimp.hpp"

#include <QtCore/qmap.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

class QAiScenePlugin : public QGLSceneFormatPlugin
{
public:
    QStringList keys() const;
    virtual QGLSceneFormatHandler *create(QIODevice *device, const QUrl& url, const QString &format) const;
};

QStringList QAiScenePlugin::keys() const
{
    static QMultiMap<QString, QString> mimetypes;
    if (mimetypes.size() == 0)
    {
        mimetypes.insertMulti("3ds", "application/x-3ds");
        mimetypes.insertMulti("3ds", "image/x-3ds");
        mimetypes.insertMulti("dae", "model/x3d+binary");
        mimetypes.insertMulti("dxf", "application/x-dxf");
    }
    QStringList result;
    Assimp::Importer importer;
    aiString extns;
    importer.GetExtensionList(extns);
    QString qextns = QString::fromUtf8(extns.data, extns.length);
    QStringList extnList = qextns.split(';');
    for (int i = 0; i < extnList.size(); ++i)
    {
        QString xt = extnList.at(i);
        xt = xt.simplified();
        if (xt.startsWith(QLatin1String("*.")))
            xt = xt.mid(2);
        result << xt;
        QMap<QString, QString>::const_iterator it = mimetypes.constFind(xt);
        for ( ; it != mimetypes.constEnd(); ++it)
            result << it.value();
    }
    qDebug() << "Asset Importer reports support for:" << result;
    return result;
}

QGLSceneFormatHandler *QAiScenePlugin::create(QIODevice *device, const QUrl& url, const QString &format) const
{
    Q_UNUSED(device);
    Q_UNUSED(url);
    Q_UNUSED(format);
    return new QAiSceneHandler;
}

Q_EXPORT_STATIC_PLUGIN(QAiScenePlugin)
Q_EXPORT_PLUGIN2(qsceneai, QAiScenePlugin)

QT_END_NAMESPACE
