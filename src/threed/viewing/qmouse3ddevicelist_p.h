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

class QMouse3DDeviceList : public QObject
{
    Q_OBJECT
public:
    QMouse3DDeviceList(QObject *parent = 0);
    ~QMouse3DDeviceList();

    QList<QMouse3DDevice *> devices;

    static QMouse3DDeviceList *attach();
    static void detach(QMouse3DDeviceList *list);

    void attachWidget(QMouse3DEventProvider *provider, QWidget *widget);
    void detachWidget(QMouse3DEventProvider *provider, QWidget *widget);

    void updateFilters(QMouse3DEventProvider *provider,
                       QMouse3DEventProvider::Filters value);
    void updateSensitivity
        (QMouse3DEventProvider *provider, qreal value);

private Q_SLOTS:
    void availableDeviceChanged();

Q_SIGNALS:
    void availableChanged();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void setWidget(QMouse3DEventProvider *provider, QWidget *widget);

    QBasicAtomicInt ref;
    QWidget *currentWidget;
    QMouse3DEventProvider *currentProvider;
    QMap<QWidget *, QMouse3DEventProvider *> widgets;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
