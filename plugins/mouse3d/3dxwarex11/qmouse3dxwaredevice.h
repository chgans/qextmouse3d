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

#ifndef QMOUSE3DXWAREDEVICE_H
#define QMOUSE3DXWAREDEVICE_H

#include "qmouse3ddevice_p.h"
#include <QtGui/private/qt_x11_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QMouse3DxWareDevice : public QMouse3DDevice
{
    Q_OBJECT
public:
    QMouse3DxWareDevice(QObject *parent = 0);
    ~QMouse3DxWareDevice();

    bool isAvailable() const;
    QStringList deviceNames() const;

    void setWidget(QWidget *widget);

    bool initialized;
    bool available;
    Window appWindow;
    Window commandWindow;
    Atom commandEventAtom;
    Atom motionEventAtom;
    Atom buttonPressEventAtom;
    Atom buttonReleaseEventAtom;
    unsigned int f1Keycode;
    unsigned int f2Keycode;
    unsigned int f3Keycode;
    unsigned int f4Keycode;
    unsigned int f5Keycode;
    unsigned int f6Keycode;

    void init();
    void motionEvent(XEvent *event);
    void buttonPressEvent(int code);
    void buttonReleaseEvent(int code);
    bool customKeyEvent(unsigned int keycode, bool press);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
