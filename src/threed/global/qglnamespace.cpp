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

#include "qglnamespace.h"
#include <QtCore/qsize.h>

QT_BEGIN_NAMESPACE

/*!
    \namespace QGL

    \brief The QGL namespace contains miscellaneous identifiers
    used throughout the Qt/3D module.
*/

/*!
    \enum QGL::Mouse3DKeys
    \since 4.8
    This enum defines extra key codes for QKeyEvent related to 3D mice
    devices.  See QMouse3DEventProvider for further details.

    \value Key_Fit Reset the transform so the model is centered to
           fit within the view.
    \value Key_TopView Switch to the top view.
    \value Key_LeftView Switch to the left view.
    \value Key_RightView Switch to the right view.
    \value Key_FrontView Switch to the front view.
    \value Key_BottomView Switch to the bottom view.
    \value Key_BackView Switch to the back view.
    \value Key_RotateCW90 Rotate clockwise by 90 degrees.
    \value Key_RotateCCW90 Rotate counter-clockwise by 90 degrees.
    \value Key_ISO1 Show an isometric view of the model with top, front,
           and right sides.
    \value Key_ISO2 Show an isometric view of the model with top, back
           and left sides.
    \value Key_Button1 Special function button 1.
    \value Key_Button2 Special function button 2.
    \value Key_Button3 Special function button 3.
    \value Key_Button4 Special function button 4.
    \value Key_Button5 Special function button 5.
    \value Key_Button6 Special function button 6.
    \value Key_Button7 Special function button 7.
    \value Key_Button8 Special function button 8.
    \value Key_Button9 Special function button 9.
    \value Key_Button10 Special function button 10.
    \value Key_Rotations Toggles whether or not the rotation axes on a
           3D mouse will be filtered.
    \value Key_Translations Toggles whether or not the translation axes
           on a 3D mouse will be filtered.
    \value Key_DominantAxis Toggles whether or not 3D mouse movements
           should be constrained to the dominant axis.
    \value Key_IncreaseSensitivity Increase the sensitivity of the mouse
           to wrist movements.
    \value Key_DecreaseSensitivity Decrease the sensitivity of the mouse
           to wrist movements.
*/

QT_END_NAMESPACE
