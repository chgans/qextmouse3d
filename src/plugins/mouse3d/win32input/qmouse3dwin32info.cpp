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

#include "qmouse3dwin32info.h"


pRegisterRawInputDevices _RegisterRawInputDevices;
pGetRawInputData _GetRawInputData;
pGetRawInputDeviceInfoA _GetRawInputDeviceInfo;
pGetRawInputDeviceList _GetRawInputDeviceList;

QMouseEventTransmitter mouseSignaller;

bool initialiseMouse3dRawInputFunctionsUsingUser32DynamicLinkLibrary()
{
    //Initialise raw input functions from the user32.dll.
    HMODULE user32DLL = LoadLibrary(L"user32.dll");
    if (!user32DLL){
        qWarning()<<"Load user32.dll failed.";
        return false;
    }


    _RegisterRawInputDevices = (pRegisterRawInputDevices)GetProcAddress(user32DLL,"RegisterRawInputDevices");
    if (!_RegisterRawInputDevices)  {
        qWarning()<< "Failed to locate RegisterRawInputDevice method in user32.dll.";
        return false;
    }

    _GetRawInputData = (pGetRawInputData)GetProcAddress(user32DLL,"GetRawInputData");
    if (!_GetRawInputData)  {
        qWarning()<< "Failed to locate GetRawInputData method in user32.dll.";
        return false;
    }

    _GetRawInputDeviceList = (pGetRawInputDeviceList)GetProcAddress(user32DLL,"GetRawInputDeviceList");
    if (!_GetRawInputDeviceList) {
        qWarning()<< "Failed to locate GetRawInputDeviceList method in user32.dll.";
        return false;
    }

    _GetRawInputDeviceInfo = (pGetRawInputDeviceInfoA)GetProcAddress(user32DLL,"GetRawInputDeviceInfoW");
    if (!_GetRawInputDeviceInfo) {
        qWarning()<< "Failed to locate GetRawInputDeviceInfoW method in user32.dll.";
        return false;
    }
    return true;
}

bool mouse3dEventFilterFunction(void *newMessage, long *result)
{
    Q_UNUSED(result);
    MSG* message=(MSG*)newMessage;
    //the "message" component is a MSG (it tagMSG as defined in windows.h)
    switch(message->message){
    case WM_INPUT:
        {
            //OnRawInput(newMessage);
            // This feature requires Windows XP or greater.
            // The symbol _WIN32_WINNT must be >= 0x0501.
            mouseSignaller.sendDetectedSignal((HRAWINPUT)message->lParam);
        }
        return true;
    }
    return false;
}

QMouseEventTransmitter::QMouseEventTransmitter(QObject *parent) : QObject(parent) {
}

QMouseEventTransmitter::~QMouseEventTransmitter() {
}

void QMouseEventTransmitter::sendDetectedSignal(HRAWINPUT hRawInput) {
    emit rawInputDetected(hRawInput);
}
