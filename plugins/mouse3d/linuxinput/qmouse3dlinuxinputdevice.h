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

#ifndef QMOUSE3DLINUXINPUTDEVICE_H
#define QMOUSE3DLINUXINPUTDEVICE_H

#include "qmouse3ddevice_p.h"
#include <QtCore/qsocketnotifier.h>
#include <QtCore/qtimer.h>
#include <linux/input.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QMouse3DLinuxInputDevice : public QMouse3DDevice
{
    Q_OBJECT
public:
    QMouse3DLinuxInputDevice(QObject *parent = 0);
    ~QMouse3DLinuxInputDevice();

    bool isAvailable() const;
    QStringList deviceNames() const;

    void setWidget(QWidget *widget);

private Q_SLOTS:
    void readyRead();

private:
    bool available;
    bool isOpen;
    QString name;
    int fd;
    QSocketNotifier *notifier;
    int values[6];
    int tempValues[6];
    int range[6];
    int flatMiddle;
    int mscKey;
    bool sawTranslate;
    bool sawRotate;
    bool prevWasFlat;

    enum
    {
        // Flag bits for the type of mouse - can be more than one.
        MouseUnknown            = 0x0000,
        Mouse3Dconnexion        = 0x0001,
        MouseSpacePilotPRO      = 0x0002,
        MouseSpaceNavigator     = 0x0004
    };
    int mouseType;

    void findDevice(bool leaveOpen);
    void initDevice(int fd);
    void translateMscKey(int code, bool press);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
