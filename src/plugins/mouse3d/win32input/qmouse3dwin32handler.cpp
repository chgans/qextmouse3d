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

#include <QtGui/QApplication>
#include "qmouse3dwin32handler.h"
#include "qmouse3dwin32info.h"
#include <QtCore/qdebug.h>

//Raw input function pointers
extern pRegisterRawInputDevices _RegisterRawInputDevices;
extern pGetRawInputData _GetRawInputData;
extern pGetRawInputDeviceInfoA _GetRawInputDeviceInfo;
extern pGetRawInputDeviceList _GetRawInputDeviceList;

QT_BEGIN_NAMESPACE
QMouse3DWin32Handler::QMouse3DWin32Handler(QObject *parent)
    : QMouse3DDevice(parent)    
{
    // Find all mouse devices that are currently connected.
    int inputDeviceCount;
    
    PRAWINPUTDEVICELIST inputDeviceList;    
    	
	if ((*_GetRawInputDeviceList)(NULL, &inputDeviceCount, sizeof(RAWINPUTDEVICELIST)) != 0) {
		qWarning() << "Unable to get count of attached devices.";
		return;
	}

	// Allocate the array to hold the DeviceList
    inputDeviceList = new RAWINPUTDEVICELIST[inputDeviceCount];
    if (!inputDeviceList) {
        qWarning() << "Unable to create list for raw input devices.";
        return;
    }
		
	if ((*_GetRawInputDeviceList)(inputDeviceList, &inputDeviceCount, sizeof(RAWINPUTDEVICELIST)) == -1)  {
        qWarning() << "Could not get list of raw input devices.";
        return;
	}
    
    devices.clear();
    	
	for (int i = 0; i < inputDeviceCount; i++) {
        //if the device is a generid HID (ie. not mouse or keyboard)		
		if (inputDeviceList[i].dwType == RIM_TYPEHID) {				
			mouseAdded(inputDeviceList[i].hDevice);			
        }
    }	
}

QMouse3DWin32Handler::~QMouse3DWin32Handler()
{    
    qDeleteAll(devices);
}

bool QMouse3DWin32Handler::isAvailable() const
{
    return !devices.isEmpty();
}

QStringList QMouse3DWin32Handler::deviceNames() const
{
    QStringList names;
    for (int index = 0; index < devices.size(); ++index)
        names += devices[index]->devName;
    return names;
}

void QMouse3DWin32Handler::setProvider(QMouse3DEventProvider *provider)
{
    QMouse3DDevice::setProvider(provider);
    for (int index = 0; index < devices.size(); ++index)
        devices[index]->device->setProvider(provider);
}

void QMouse3DWin32Handler::setWidget(QWidget *widget)
{
    QMouse3DDevice::setWidget(widget);
    for (int index = 0; index < devices.size(); ++index)
        devices[index]->device->setWidget(widget);
}

void QMouse3DWin32Handler::updateFilters(QMouse3DEventProvider::Filters filters)
{
    QMouse3DDevice::updateFilters(filters);
    for (int index = 0; index < devices.size(); ++index)
        devices[index]->device->updateFilters(filters);
}

void QMouse3DWin32Handler::updateSensitivity(qreal sensitivity)
{
    QMouse3DDevice::updateSensitivity(sensitivity);
    for (int index = 0; index < devices.size(); ++index)
        devices[index]->device->updateSensitivity(sensitivity);
}

void QMouse3DWin32Handler::deviceAdded(HANDLE deviceHandle)
{
    Q_UNUSED(deviceHandle);
	//
	//
	//	TODO: No equivalent for this in Win32 right now.
	//
	//
}

void QMouse3DWin32Handler::deviceRemoved(HANDLE deviceHandle)
{
    for (int index = 0; index < devices.size(); ++index) {
        MouseInfo *info = devices[index];
        if (info->deviceHandle == deviceHandle) {
            devices.removeAt(index);
            delete info;
            emit availableChanged();
            break;
        }
    }
}

void QMouse3DWin32Handler::mouseAdded (HANDLE deviceHandle)
{
	//Check if we already have this device in our list and bail out if we do
	for(int i=0; i< devices.size(); i++) {
		if (devices[i]->deviceHandle == deviceHandle) {
			return;
		}
	}

	//Get additional device information about the device with the given handle.
	RID_DEVICE_INFO deviceInfo;
    deviceInfo.cbSize = sizeof(RID_DEVICE_INFO);
    int dwSize = sizeof(RID_DEVICE_INFO);

    //Get the device information
    if ((*_GetRawInputDeviceInfo)(deviceHandle, RIDI_DEVICEINFO, &deviceInfo, &dwSize)!=0)    
    {
		//Add the mouse device
		//Determine if the device is a 3dMouse
		if (deviceInfo.hid.usUsage==0x08)                
		{
			//Set 3d device information and add it to the list of available devices.
			QString devName = "Unknown";                     
			QString devBrand = "Unknown";
			devName = ProductName(deviceInfo.hid.dwProductId);   
			if (deviceInfo.hid.dwVendorId == LOGITECH_VENDOR_ID)  {
				devBrand = "Logitech";
			}
		   
			QMouse3DWin32InputDevice *device = new QMouse3DWin32InputDevice(devName, devBrand, deviceHandle, this);    	
			MouseInfo *info = new MouseInfo(deviceHandle, devName, devBrand, device);

			devices.append(info);
		}		
    }
    else {
        qWarning() << "Failed to get valid device information.";
    }			

	// if there is a valid mouse, set the message filter
	if (devices.size()>0)
		QApplication::instance()->setEventFilter(mouse3dEventFilterFunction);	

	//QCoreApplication::EventFilter(
    // Tell the application that there is a new mouse attached.
    emit availableChanged();
}

QT_END_NAMESPACE
