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

#ifndef QMOUSE3DWIN32INFO_H
#define QMOUSE3DWIN32INFO_H

#include <windows.h>
#include <math.h>
#include <QDebug>
#include <QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#ifndef HRAWINPUT
#define HRAWINPUT DWORD
#endif

#define LOGITECH_VENDOR_ID 0x46d  //Vendor identifier for Logitech - manufacturers of 3dconnexion human input devices.

// Function pointers for Windows Raw Input functions.
typedef WINUSERAPI BOOL (WINAPI *pRegisterRawInputDevices)(IN PCRAWINPUTDEVICE pRawInputDevices, IN UINT uiNumDevices, IN UINT cbSize);
typedef WINUSERAPI INT (WINAPI *pGetRawInputData)(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData,  PINT pcbSize, UINT cbSizeHeader);
typedef WINUSERAPI INT (WINAPI *pGetRawInputDeviceInfoA)(IN HANDLE hDevice, IN UINT uiCommand, OUT LPVOID pData, IN OUT PINT pcbSize);
typedef WINUSERAPI INT (WINAPI *pGetRawInputDeviceList)(OUT PRAWINPUTDEVICELIST pRawInputDeviceList, IN OUT PINT puiNumDevices, IN UINT cbSize);

bool initialiseMouse3dRawInputFunctionsUsingUser32DynamicLinkLibrary();
bool mouse3dEventFilterFunction(void *newMessage, long *result);

// A small class whose only purpose is the generation of signals on Raw Input message receipt.
class QMouseEventTransmitter : public QObject
{
    Q_OBJECT
public:
    QMouseEventTransmitter(QObject *parent = 0);
    ~QMouseEventTransmitter();
	
	void sendDetectedSignal(HRAWINPUT hRawInput);

Q_SIGNALS:
     void rawInputDetected(HRAWINPUT hRawInput);
};

//These product identification (pid) codes are encapsulated in the
//windows event messages and identify individual 3d connexion mouse
//products.
enum connextion3d_product_id  {
	eSpacePilot					= 0xc625,
	eSpaceNavigator				= 0xc626,
	eSpaceExplorer				= 0xc627,
	eSpaceNavigatorForNotebooks	= 0xc628,
	eSpacePilotPRO				= 0xc629
};

// Virtual key-codes for 3dConnexion mouse buttons.
enum connexion3dKeyCode
{
	V3DK_INVALID=0,		// Left button on SpaceNavigator
	V3DK_MENU=1,		// Right button on SpaceNavigator
	V3DK_FIT,			// Fit to viewport
	V3DK_TOP,			// Switch to Top View
	V3DK_LEFT,			// Switch to Left View
	V3DK_RIGHT,			// Switch to Right View
	V3DK_FRONT,			// Switch to Front View
	V3DK_BOTTOM,		// Switch to Bottom View
	V3DK_BACK,			// Switch to Back View
	V3DK_CW,			// Rotate 90 degrees clockwise
	V3DK_CCW,			// Rotate 90 degrees counter-clockwise
	V3DK_ISO1,			// Isometric top/front/right view
	V3DK_ISO2,			// Isometric top/back/left view
	V3DK_1,				// Custom 3d mouse function key 1
	V3DK_2,				// .
	V3DK_3,				// .
	V3DK_4,				// .
	V3DK_5,				// .
	V3DK_6,				// .
	V3DK_7,				// .
	V3DK_8,				// .
	V3DK_9,				// .
	V3DK_10,			// Custom 3d mouse function key 10
	V3DK_ESC,			// Escape key
	V3DK_ALT,			// Alt key
	V3DK_SHIFT,			// Shit key
	V3DK_CTRL,			// Control key
	V3DK_ROTATE,		// Toggle rotation on/off
	V3DK_PANZOOM,		// Toggle pan on/off
	V3DK_DOMINANT,		// Toggle main axis being dominant
	V3DK_PLUS,			// Increase sensitivity of movements
	V3DK_MINUS			// Decrease sensitivity of movements
};

static const char* connexion3d_key_names[] =
{
  "INVALID", "MENU", "FIT", "TOP", "LEFT", "RIGHT", "FRONT", "BOTTOM", "BACK", "CW", 
  "CCW", "ISO1", "ISO2", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "ESC", 
  "ALT", "SHIFT", "CTRL", "ROTATE", "PANZOOM", "DOMINANT", "PLUS", "MINUS"
};

static const connexion3dKeyCode SpaceExplorerKeys [] = 
{
  V3DK_INVALID, V3DK_1, V3DK_2, V3DK_TOP, V3DK_LEFT, V3DK_RIGHT, V3DK_FRONT, V3DK_ESC, 
  V3DK_ALT, V3DK_SHIFT, V3DK_CTRL, V3DK_FIT, V3DK_MENU, V3DK_PLUS, V3DK_MINUS, V3DK_ROTATE
};

static const connexion3dKeyCode SpacePilotKeys [] = 
{
  V3DK_INVALID, V3DK_1, V3DK_2, V3DK_3, V3DK_4, V3DK_5, V3DK_6, V3DK_TOP, V3DK_LEFT, 
  V3DK_RIGHT, V3DK_FRONT, V3DK_ESC, V3DK_ALT, V3DK_SHIFT, V3DK_CTRL, V3DK_FIT, V3DK_MENU, 
  V3DK_PLUS, V3DK_MINUS, V3DK_DOMINANT, V3DK_ROTATE
};

__inline QString ProductName(unsigned long pid)
{   
	switch (pid)
	{
	case 0xc625:
		return "Space Pilot";
	case 0xc626:
		return "Space Navigator";            
	case 0xc627:
		return "Space Explorer";            
	case 0xc628:
		return "Space Navigator - Notebook";            
	case 0xc629:
		return "Space Pilot Pro";            
	default:
		return "Unknown!";            
	}
}

__inline unsigned short rawInputToVirtualKeyCode(unsigned long pid, int hidKeyCode)
{                          
    unsigned short virtualkey = V3DK_INVALID;	
	
	//Sanity checks on input.
	if (hidKeyCode<=0) 
		virtualkey = V3DK_INVALID;	
	else 
		//get the appropriate bit masking for key codes
		virtualkey = 1 + (int)(log( (double) hidKeyCode ) / log( 2.0 ));

	int numKeys = 0;
	
	//Special product cases.
	switch (pid)
	{
	case eSpacePilot:
		numKeys = sizeof(SpacePilotKeys)/sizeof(SpacePilotKeys[0]);
		virtualkey < numKeys ? virtualkey = SpacePilotKeys[virtualkey] : virtualkey = V3DK_INVALID;
		break;
	case eSpaceExplorer:
		numKeys = sizeof(SpaceExplorerKeys)/SpaceExplorerKeys[0];
		virtualkey < numKeys ? virtualkey = SpaceExplorerKeys[virtualkey] : virtualkey = V3DK_INVALID;
		break;	
	case eSpaceNavigator:
	case eSpaceNavigatorForNotebooks:
	case eSpacePilotPRO:
		//no change
		if (virtualkey>V3DK_MINUS) virtualkey = V3DK_INVALID;
		break;
	default:
		//no change but unknown hardware.  Show a warning, but carry on.
		qWarning() << "Unknown product id [" <<pid<<"] defaulting to standard keys.";
		break;
	}	

	return virtualkey;  
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMOUSE3DWIN32INFO_H
