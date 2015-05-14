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

#ifndef QGLNAMESPACE_H
#define QGLNAMESPACE_H

#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

namespace QGL
{
    enum Mouse3DKeys
    {
        Key_Fit                 = 0x01200002,
        Key_TopView             = 0x01200003,
        Key_LeftView            = 0x01200004,
        Key_RightView           = 0x01200005,
        Key_FrontView           = 0x01200006,
        Key_BottomView          = 0x01200007,
        Key_BackView            = 0x01200008,
        Key_RotateCW90          = 0x01200009,
        Key_RotateCCW90         = 0x0120000a,
        Key_ISO1                = 0x0120000b,
        Key_ISO2                = 0x0120000c,
        Key_Button1             = 0x0120000d,
        Key_Button2             = 0x0120000e,
        Key_Button3             = 0x0120000f,
        Key_Button4             = 0x01200010,
        Key_Button5             = 0x01200011,
        Key_Button6             = 0x01200012,
        Key_Button7             = 0x01200013,
        Key_Button8             = 0x01200014,
        Key_Button9             = 0x01200015,
        Key_Button10            = 0x01200016,
        Key_Rotations           = 0x0120001b,
        Key_Translations        = 0x0120001c,
        Key_DominantAxis        = 0x0120001d,
        Key_IncreaseSensitivity = 0x0120001e,
        Key_DecreaseSensitivity = 0x0120001f
    };
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
