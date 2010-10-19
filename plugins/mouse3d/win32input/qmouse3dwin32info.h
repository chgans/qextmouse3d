#ifndef QMOUSE3DWIN32INFO_H
#define QMOUSE3DWIN32INFO_H

#include <windows.h>
#include <math.h>
#include <QDebug>
#include <QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

//#define _WIN32_WINNT 0x501   // This says that we are using WinXP or later - raw input is only available on such system.
#define LOGITECH_VENDOR_ID 0x46d
#define WM_INPUT 0x00FF      // This is the code for WM_INPUT messages.

typedef WINUSERAPI BOOL (WINAPI *pRegisterRawInputDevices)(IN PCRAWINPUTDEVICE pRawInputDevices, IN UINT uiNumDevices, IN UINT cbSize);
typedef WINUSERAPI INT (WINAPI *pGetRawInputData)(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData,  PINT pcbSize, UINT cbSizeHeader);
typedef WINUSERAPI INT (WINAPI *pGetRawInputDeviceInfoA)(IN HANDLE hDevice, IN UINT uiCommand, OUT LPVOID pData, IN OUT PINT pcbSize);
typedef WINUSERAPI INT (WINAPI *pGetRawInputDeviceList)(OUT PRAWINPUTDEVICELIST pRawInputDeviceList, IN OUT PINT puiNumDevices, IN UINT cbSize);

bool InitialiseMouse3dRawInputFunctionsUsingUser32DynamicLinkLibrary();

bool mouse3dEventFilterFunction(void *newMessage, long *result);

class QMouseData : public QObject
{
    Q_OBJECT
public:
    QMouseData(QObject *parent = 0);
    ~QMouseData();
	
	void sendDetectedSignal(HRAWINPUT hRawInput);

Q_SIGNALS:
     void rawInputDetected(HRAWINPUT hRawInput);
};

enum e3dconnexion_pid {
  eSpacePilot = 0xc625,
  eSpaceNavigator = 0xc626,
  eSpaceExplorer = 0xc627,
  eSpaceNavigatorForNotebooks = 0xc628,
  eSpacePilotPRO = 0xc629
};

enum e3dmouse_virtual_key 
{
  V3DK_INVALID=0
  , V3DK_MENU=1, V3DK_FIT
  , V3DK_TOP, V3DK_LEFT, V3DK_RIGHT, V3DK_FRONT, V3DK_BOTTOM, V3DK_BACK
  , V3DK_CW, V3DK_CCW
  , V3DK_ISO1, V3DK_ISO2
  , V3DK_1, V3DK_2, V3DK_3, V3DK_4, V3DK_5, V3DK_6, V3DK_7, V3DK_8, V3DK_9, V3DK_10
  , V3DK_ESC, V3DK_ALT, V3DK_SHIFT, V3DK_CTRL
  , V3DK_ROTATE, V3DK_PANZOOM, V3DK_DOMINANT
  , V3DK_PLUS, V3DK_MINUS
};

static const char* _3dmouse_virtual_key[] =
{
  "V3DK_INVALID"
  , "V3DK_MENU", "V3DK_FIT"
  , "V3DK_TOP", "V3DK_LEFT", "V3DK_RIGHT", "V3DK_FRONT", "V3DK_BOTTOM", "V3DK_BACK"
  , "V3DK_CW", "V3DK_CCW"
  , "V3DK_ISO1", "V3DK_ISO2"
  , "V3DK_1", "V3DK_2", "V3DK_3", "V3DK_4", "V3DK_5", "V3DK_6", "V3DK_7", "V3DK_8", "V3DK_9", "V3DK_10"
  , "V3DK_ESC", "V3DK_ALT", "V3DK_SHIFT", "V3DK_CTRL"
  , "V3DK_ROTATE", "V3DK_PANZOOM", "V3DK_DOMINANT"
  , "V3DK_PLUS", "V3DK_MINUS"
};


struct tag_VirtualKeys
{
  e3dconnexion_pid pid;
  size_t nKeys;
  e3dmouse_virtual_key *vkeys;
};

static const e3dmouse_virtual_key SpaceExplorerKeys [] = 
{
  V3DK_INVALID     // there is no button 0
  , V3DK_1, V3DK_2
  , V3DK_TOP, V3DK_LEFT, V3DK_RIGHT, V3DK_FRONT
  , V3DK_ESC, V3DK_ALT, V3DK_SHIFT, V3DK_CTRL
  , V3DK_FIT, V3DK_MENU
  , V3DK_PLUS, V3DK_MINUS
  , V3DK_ROTATE
};

static const e3dmouse_virtual_key SpacePilotKeys [] = 
{
  V3DK_INVALID 
  , V3DK_1, V3DK_2, V3DK_3, V3DK_4, V3DK_5, V3DK_6
  , V3DK_TOP, V3DK_LEFT, V3DK_RIGHT, V3DK_FRONT
  , V3DK_ESC, V3DK_ALT, V3DK_SHIFT, V3DK_CTRL
  , V3DK_FIT, V3DK_MENU
  , V3DK_PLUS, V3DK_MINUS
  , V3DK_DOMINANT, V3DK_ROTATE
};

static const struct tag_VirtualKeys _3dmouseVirtualKeys[]= 
{
  eSpacePilot
  , sizeof(SpacePilotKeys)/sizeof(SpacePilotKeys[0])
  , const_cast<e3dmouse_virtual_key *>(SpacePilotKeys),
  eSpaceExplorer
  , sizeof(SpaceExplorerKeys)/sizeof(SpaceExplorerKeys[0])
  , const_cast<e3dmouse_virtual_key *>(SpaceExplorerKeys)
};  
   
__inline double Log2( double n )  
{  
    // log(n)/log(2) is log2.  
    return log( n ) / log( 2.0 );  
}

__inline QString ProductName(unsigned long pid)
{   
	   switch (pid)
	   {
		case 0xc625:
			return "Space Pilot";
			break;
		case 0xc626:
			return "Space Navigator";            
			break;
		case 0xc627:
			return "Space Explorer";            
			break;
		case 0xc628:
			return "Space Navigator - Notebook";            
			break;
		case 0xc629:
			return "Space Pilot Pro";            
			break;
		default:
			return "Unknown!";            
			break;
	   }
}

__inline unsigned short HidToVirtualKey(unsigned long pid, int hidKeyCode)
{                          
	  unsigned short virtualkey = V3DK_INVALID;

	  if (hidKeyCode<=0) virtualkey = V3DK_INVALID;
	  else virtualkey = 1 + (int)Log2(hidKeyCode);

	  for (size_t i=0; i<sizeof(_3dmouseVirtualKeys)/sizeof(_3dmouseVirtualKeys[0]); ++i)
	  {                                
		  if (pid == _3dmouseVirtualKeys[i].pid)
		  {                    
			  qDebug() << "Special case: " << ProductName(pid);          
			  if (virtualkey < _3dmouseVirtualKeys[i].nKeys)
				virtualkey = _3dmouseVirtualKeys[i].vkeys[virtualkey];
			  else
				virtualkey = V3DK_INVALID;
		  }               
	  }

	  qDebug()<<"Converted " << hidKeyCode << " to key " << virtualkey << "(" << _3dmouse_virtual_key[virtualkey] << " for pid " << pid;      
	  return virtualkey;  

}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMOUSE3DWIN32INFO_H
