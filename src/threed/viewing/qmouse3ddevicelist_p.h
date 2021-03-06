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

#ifndef QMOUSE3DDEVICELIST_P_H
#define QMOUSE3DDEVICELIST_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmouse3ddevice_p.h"
#include "qmouse3deventprovider.h"
#include <QtCore/qatomic.h>
#include <QtCore/qmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QExtMouse3DDeviceList : public QObject
{
    Q_OBJECT
public:
    QExtMouse3DDeviceList(QObject *parent = 0);
    ~QExtMouse3DDeviceList();

    QList<QExtMouse3DDevice *> devices;

    static QExtMouse3DDeviceList *attach();
    static void detach(QExtMouse3DDeviceList *list);

    void attachWidget(QExtMouse3DEventProvider *provider, QWidget *widget);
    void detachWidget(QExtMouse3DEventProvider *provider, QWidget *widget);

    void updateFilters(QExtMouse3DEventProvider *provider,
                       QExtMouse3DEventProvider::Filters value);
    void updateSensitivity
        (QExtMouse3DEventProvider *provider, qreal value);

private Q_SLOTS:
    void availableDeviceChanged();

Q_SIGNALS:
    void availableChanged();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void setWidget(QExtMouse3DEventProvider *provider, QWidget *widget);

    QBasicAtomicInt ref;
    QWidget *currentWidget;
    QExtMouse3DEventProvider *currentProvider;
    QMap<QWidget *, QExtMouse3DEventProvider *> widgets;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
