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

#include "qmouse3dwin32info.h"

typedef WINUSERAPI BOOL (WINAPI *pRegisterRawInputDevices)(IN PCRAWINPUTDEVICE pRawInputDevices, IN UINT uiNumDevices, IN UINT cbSize);
typedef WINUSERAPI INT (WINAPI *pGetRawInputData)(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData,  PINT pcbSize, UINT cbSizeHeader);
typedef WINUSERAPI INT (WINAPI *pGetRawInputDeviceInfoA)(IN HANDLE hDevice, IN UINT uiCommand, OUT LPVOID pData, IN OUT PINT pcbSize);
typedef WINUSERAPI INT (WINAPI *pGetRawInputDeviceList)(OUT PRAWINPUTDEVICELIST pRawInputDeviceList, IN OUT PINT puiNumDevices, IN UINT cbSize);

pRegisterRawInputDevices _RegisterRawInputDevices;
pGetRawInputData _GetRawInputData;
pGetRawInputDeviceInfoA _GetRawInputDeviceInfo;
pGetRawInputDeviceList _GetRawInputDeviceList;

bool InitialiseMouse3dRawInputFunctionsUsingUser32DynamicLinkLibrary() 
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

bool Mouse3dEventFilterFunction(void *newMessage, long *result)
{
    MSG* message=(MSG*)newMessage;
    //the "message" component is a MSG (it tagMSG as defined in windows.h)
    switch(message->message){
    case WM_INPUT:
        {   
            qDebug() << "Raw Input Recieved!\n";
            //OnRawInput(newMessage);
            // This feature requires Windows XP or greater.
            // The symbol _WIN32_WINNT must be >= 0x0501.

            HRAWINPUT hRawInput = (HRAWINPUT)message->lParam;

            int dwSize;

            (*_GetRawInputData)(hRawInput, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

            LPBYTE lpb = new BYTE[dwSize];
            if (lpb == NULL)
            {
                qDebug()<< "Error allocating memory for raw input data!\n";
                return true;
            }

            if ((*_GetRawInputData)(hRawInput, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
                qDebug()<<"GetRawInputData doesn't return correct size !\n";
            }

            //RAWINPUT* raw = (RAWINPUT*)lpb;
            RAWINPUT* pRawInput = (RAWINPUT*)lpb;		
			
			HANDLE thisDevice = pRawInput->header.hDevice;

            if (pRawInput->header.dwType!= RIM_TYPEHID) 
            {
                delete[] lpb; 
                qDebug()<<"Mouse/Keyboard...";
                return true;  //the data is from either keyboard or mouse, so ignore it.
            }
            else
                qDebug()<<"Other...";


            RID_DEVICE_INFO sRidDeviceInfo;

            sRidDeviceInfo.cbSize = sizeof(RID_DEVICE_INFO);
            dwSize = sizeof(RID_DEVICE_INFO);
            
            if ((*_GetRawInputDeviceInfo)(pRawInput->header.hDevice, RIDI_DEVICEINFO, &sRidDeviceInfo, &dwSize)!=0)
            //if ((*_GetRawInputDeviceInfo)(pRawInput->header.hDevice, RIDI_DEVICENAME, &sRidDeviceInfo, &dwSize) == dwSize ) 
            {

                if (sRidDeviceInfo.hid.dwVendorId == LOGITECH_VENDOR_ID)
                {
                    //motion data comes in 2 packages
                    //orientation is right handed coord system with z down
                    //this is the standard HID orientation
                    if (pRawInput->data.hid.bRawData[0]==0x01)
                    {
                        //translation vector
                        short *pnData = reinterpret_cast <short*>(&pRawInput->data.hid.bRawData[1]);
                        short X = pnData[0];
                        short Y = pnData[1];
                        short Z = pnData[2];
                        qDebug() << "PanZoom RI Data = " << X << "," << Y << "," << Z;
                    }
                    else if (pRawInput->data.hid.bRawData[0]==0x02)
                    {
                        //directed rotation vector
                        short *pnData = reinterpret_cast <short*>(&pRawInput->data.hid.bRawData[1]);
                        short rX = pnData[0];
                        short rY = pnData[1];
                        short rZ = pnData[2];
                        qDebug() << "Rotation RI Data = " << rX << "," << rY << "," << rZ;                
                    }
                    else if (pRawInput->data.hid.bRawData[0]==0x03)
                    {
                        //state of the keys
                        unsigned long dwKeystate= *reinterpret_cast<unsigned long *>(&pRawInput->data.hid.bRawData[1]);                                 
                        int nVirtualKeyCode = HidToVirtualKey(sRidDeviceInfo.hid.dwProductId, dwKeystate);                                                         
                    }
                }
            }

            delete[] lpb; 
        }
        return true;
    }
    return false;
}