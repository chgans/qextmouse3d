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

#ifndef QMOUSE3DDEVICE_P_H
#define QMOUSE3DDEVICE_P_H

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

#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>
#include "qmouse3devent.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QMouse3DDevicePrivate;
class QWidget;

class Q_QT3D_EXPORT QMouse3DDevice : public QObject
{
    Q_OBJECT
public:
    QMouse3DDevice(QObject *parent = 0);
    ~QMouse3DDevice();

    virtual bool isAvailable() const = 0;
    virtual QStringList deviceNames() const = 0;

    QWidget *widget() const;
    virtual void setWidget(QWidget *widget);

    // Special modes that change the filtering to be applied to the
    // input motions by this class when "filter" is true.
    enum
    {
        Mode_Rotation               = 0x0001,
        Mode_Translation            = 0x0002,
        Mode_Dominant               = 0x0004,
        Mode_IncreaseSensitivity    = 0x0008,
        Mode_DecreaseSensitivity    = 0x0010
    };

Q_SIGNALS:
    void availableChanged();

protected:
    void keyPress(int key);
    void keyRelease(int key);
    void changeMode(int mode);
    void motion(QMouse3DEvent *event, bool filter);

private:
    QScopedPointer<QMouse3DDevicePrivate> d_ptr;

    Q_DISABLE_COPY(QMouse3DDevice)
    Q_DECLARE_PRIVATE(QMouse3DDevice)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
