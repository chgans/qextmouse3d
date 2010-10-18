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


#include <windows.h>
#include <winuser.h>
#include <QWidget>
#include "qmouse3dwin32inputdevice.h"
#include "qmouse3dwin32info.h"
#include "qglnamespace.h"

QT_BEGIN_NAMESPACE

//
//  TODO
//
//  emit availableChanged(); when new devices added etc.
//  devices (list of devices to be done)
//  mousetype enumeration
//  keypress events
//  keyup events
//
//
//
//
//

extern pRegisterRawInputDevices _RegisterRawInputDevices;
extern pGetRawInputData _GetRawInputData;
extern pGetRawInputDeviceInfoA _GetRawInputDeviceInfo;
extern pGetRawInputDeviceList _GetRawInputDeviceList;

QMouse3DWin32InputDevice::QMouse3DWin32InputDevice
        (const QString &dName , const QString &realName , QObject *parent)
    : QMouse3DDevice(parent)
    , isOpen(false)
    , devName(dName)
    , name(realName)  
     , flatMiddle(15)
    , mscKey(-1)
    , sawTranslate(false)
    , sawRotate(false)
    , prevWasFlat(false)
    , mouseType(QMouse3DWin32InputDevice::MouseUnknown)
{	
    //Initialise raw input functions from the user32.dll.
	//
	// TODO
	//

    //Initialise mouse data containers
    memset(values, 0, sizeof(values));
    memset(tempValues, 0, sizeof(tempValues));
}

QMouse3DWin32InputDevice::~QMouse3DWin32InputDevice()
{
    //delete notifier;
    //if (fd != -1)
    //    ::close(fd);
}

//Check if there are 3d mice available and save their details to the 'devices' list.
bool QMouse3DWin32InputDevice::isAvailable() const
{
    // Not used - QMouse3DHalDevice reports the availability state.
    // If the QMouse3DLinuxInputDevice object exists, the device is available.
    return true;
}

//Get the names of devices attached - in Win32 this will usually be the type of 3dConnexion mouse attached.
QStringList QMouse3DWin32InputDevice::deviceNames() const
{
    // Not used - QMouse3DHalDevice reports the name.
    return QStringList();
}


//Set the widget to which raw input processing will be applied - this should generally be the main window for the widget.
void QMouse3DWin32InputDevice::setWidget(QWidget *widget)
{
	
	HWND windowHandle = 0;
	if (widget) windowHandle = widget->winId();
	
	RAWINPUTDEVICE Rid[3];

    Rid[0].usUsagePage = 0x01;
    Rid[0].usUsage = 0x08;              //this is the usage identifier for 3d mice.
    Rid[0].dwFlags = 0;   
    Rid[0].hwndTarget = windowHandle;

    if ((*_RegisterRawInputDevices)(Rid, 1, sizeof (Rid[0]))==FALSE) {
        qWarning()<< "Raw Input Registration failed with error: " << GetLastError();
        return;
    }
    qDebug() << "Raw input registration successful!";    
}

void QMouse3DWin32InputDevice::initDevice(int fd)
{
   
}

static inline int clampRange(int value)
{
//    return qMin(qMax(value, -32768), 32767);
}


// These are the keycodes that are reported by the 3Dconnection
// SpacePilot PRO 
//
// The SpaceNavigator reports keypress events, but only has 2 buttons,
// which we map to the same behavior as pan and rotation keys
//
// http://www.3dconnexion.com/products/spacepilot-pro.html
// http://www.3dconnexion.com/products/spacenavigator.html

void QMouse3DWin32InputDevice::translateMscKey(int code, bool press)
{
    //int qtcode = -1;
    //switch (code) {
    //case SPKey_Menu:
    //    // On the SpaceNavigator, map this key to "translation lock".
    //    if (mouseType & QMouse3DWin32InputDevice::MouseSpaceNavigator) {
    //        if (press)
    //            toggleFilter(QMouse3DEventProvider::Translations);
    //        qtcode = QGL::Key_Translations;
    //    } else {
    //        qtcode = Qt::Key_Menu;
    //    }
    //    break;

    //case SPKey_Fit:
    //    // On the SpaceNavigator, map this key to "rotation lock".
    //    if (mouseType & QMouse3DWin32InputDevice::MouseSpaceNavigator) {
    //        if (press)
    //            toggleFilter(QMouse3DEventProvider::Rotations);
    //        qtcode = QGL::Key_Rotations;
    //    } else {
    //        qtcode = QGL::Key_Fit;
    //    }
    //    break;

    //case SPKey_TopView:         qtcode = QGL::Key_TopView; break;
    //case SPKey_LeftView:        qtcode = QGL::Key_LeftView; break;
    //case SPKey_RightView:       qtcode = QGL::Key_RightView; break;
    //case SPKey_FrontView:       qtcode = QGL::Key_FrontView; break;
    //case SPKey_BottomView:      qtcode = QGL::Key_BottomView; break;
    //case SPKey_BackView:        qtcode = QGL::Key_BackView; break;
    //case SPKey_RotateCW90:      qtcode = QGL::Key_RotateCW90; break;
    //case SPKey_RotateCCW90:     qtcode = QGL::Key_RotateCCW90; break;
    //case SPKey_ISO1:            qtcode = QGL::Key_ISO1; break;
    //case SPKey_ISO2:            qtcode = QGL::Key_ISO2; break;
    //case SPKey_F1:              qtcode = QGL::Key_Button1; break;
    //case SPKey_F2:              qtcode = QGL::Key_Button2; break;
    //case SPKey_F3:              qtcode = QGL::Key_Button3; break;
    //case SPKey_F4:              qtcode = QGL::Key_Button4; break;
    //case SPKey_F5:              qtcode = QGL::Key_Button5; break;
    //case SPKey_F6:              qtcode = QGL::Key_Button6; break;
    //case SPKey_F7:              qtcode = QGL::Key_Button7; break;
    //case SPKey_F8:              qtcode = QGL::Key_Button8; break;
    //case SPKey_F9:              qtcode = QGL::Key_Button9; break;
    //case SPKey_F10:             qtcode = QGL::Key_Button10; break;
    //case SPKey_Escape:          qtcode = Qt::Key_Escape; break;
    //case SPKey_Alt:             qtcode = Qt::Key_Alt; break;
    //case SPKey_Shift:           qtcode = Qt::Key_Shift; break;
    //case SPKey_Control:         qtcode = Qt::Key_Control; break;

    //case SPKey_Rotation:
    //    if (press)
    //        toggleFilter(QMouse3DEventProvider::Rotations);
    //    qtcode = QGL::Key_Rotations;
    //    break;
    //case SPKey_Pan:
    //    if (press)
    //        toggleFilter(QMouse3DEventProvider::Translations);
    //    qtcode = QGL::Key_Translations;
    //    break;
    //case SPKey_Dominant:
    //    if (press)
    //        toggleFilter(QMouse3DEventProvider::DominantAxis);
    //    qtcode = QGL::Key_DominantAxis;
    //    break;
    //case SPKey_IncSensitivity:
    //    if (press)
    //        adjustSensitivity(2.0f);
    //    qtcode = QGL::Key_IncreaseSensitivity;
    //    break;
    //case SPKey_DecSensitivity:
    //    if (press)
    //        adjustSensitivity(0.5f);
    //    qtcode = QGL::Key_DecreaseSensitivity;
    //    break;
    //default: break;
    //}
    //if (qtcode != -1) {
    //    if (press)
    //        keyPress(qtcode);
    //    else
    //        keyRelease(qtcode);
    //}
}

QT_END_NAMESPACE
