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

#include "qmouse3dxwaredevice.h"
#include "qglnamespace.h"
#include <QtGui/qapplication.h>
#include <QtGui/qwidget.h>
#include <QtGui/qx11info_x11.h>
#include <X11/keysym.h>

QT_BEGIN_NAMESPACE

static QMouse3DxWareDevice *currentDevice = 0;

QMouse3DxWareDevice::QMouse3DxWareDevice(QObject *parent)
    : QMouse3DDevice(parent)
    , initialized(false)
    , available(false)
    , appWindow(0)
    , commandWindow(0)
    , commandEventAtom(0)
    , motionEventAtom(0)
    , buttonPressEventAtom(0)
    , buttonReleaseEventAtom(0)
    , f1Keycode(0)
    , f2Keycode(0)
    , f3Keycode(0)
    , f4Keycode(0)
    , f5Keycode(0)
    , f6Keycode(0)
{
    currentDevice = this;
    init();
}

QMouse3DxWareDevice::~QMouse3DxWareDevice()
{
    currentDevice = 0;
}

bool QMouse3DxWareDevice::isAvailable() const
{
    return available;
}

QStringList QMouse3DxWareDevice::deviceNames() const
{
    QStringList names;
    if (available)
        names += QLatin1String("3DxWare");
    return names;
}

void QMouse3DxWareDevice::setWidget(QWidget *widget)
{
    QMouse3DDevice::setWidget(widget);

    // If we don't have a 3D mouse available, then nothing we can do.
    if (!available)
        return;

    // Get the X window id of the top-level window for this widget.
    Window window;
    if (widget)
        window = (Window)(widget->window()->winId());
    else
        window = 0;
    if (appWindow == window)
        return;
    appWindow = window;

    // Inform the Magellan server which window should receive the events.
    XEvent event;
    memset(&event, 0, sizeof(event));
    event.type = ClientMessage;
    event.xclient.format = 16;
    event.xclient.window = commandWindow;
    event.xclient.message_type = commandEventAtom;
    event.xclient.data.s[0] = (short)((window >> 16) & 0xFFFF);
    event.xclient.data.s[1] = (short)(window & 0xFFFF);
    event.xclient.data.s[2] = 27695;    // Register window event type.
    XSendEvent(QX11Info::display(), commandWindow, False, 0, &event);
}

// Ignore all errors while we are calling XGetWindowAttributes()
// because it is possible a BadWindow will occur if the Magellan
// command window no longer exists.
static int magellan_error_handler(Display *, XErrorEvent *)
{
    return 0;
}

// Qt event filter for getting the low-level X11 events from Magellan.
static QApplication::EventFilter prevFilter = 0;
static bool filterInstalled = false;
static bool magellan_event_filter(void *message, long *result)
{
    XEvent *event = reinterpret_cast<XEvent *>(message);
    if (currentDevice) {
        if (event->type == ClientMessage) {
            if (event->xclient.message_type == currentDevice->motionEventAtom) {
                currentDevice->motionEvent(event);
                return true;
            } else if (event->xclient.message_type ==
                            currentDevice->buttonPressEventAtom) {
                currentDevice->buttonPressEvent(event->xclient.data.s[2]);
                return true;
            } else if (event->xclient.message_type ==
                            currentDevice->buttonReleaseEventAtom) {
                currentDevice->buttonReleaseEvent(event->xclient.data.s[2]);
                return true;
            }
        } else if (event->type == 2 /*KeyPress*/ && event->xkey.state == 0x000D) {
            // Probably a Custom key press event from "3dxsrv", which we
            // translate using the mappings from the README file.
            if (currentDevice->customKeyEvent(event->xkey.keycode, true))
                return true;
        } else if (event->type == 3 /*KeyRelease*/ && event->xkey.state == 0x000D) {
            if (currentDevice->customKeyEvent(event->xkey.keycode, false))
                return true;
        }
    }
    if (prevFilter)
        return prevFilter(message, result);
    else
        return false;
}

void QMouse3DxWareDevice::init()
{
    if (initialized || !qApp)
        return;
    initialized = true;

    Display *dpy = QX11Info::display();

    // Create the atoms we will need for the 3DxWare "Magellan" protocol.
    // If the atoms aren't on the server, the "3dxsrv" daemon is not running.
    commandEventAtom = XInternAtom(dpy, "CommandEvent", True);
    motionEventAtom = XInternAtom(dpy, "MotionEvent", True);
    buttonPressEventAtom = XInternAtom(dpy, "ButtonPressEvent", True);
    buttonReleaseEventAtom = XInternAtom(dpy, "ButtonReleaseEvent", True);
    if (!commandEventAtom || !motionEventAtom || !buttonPressEventAtom ||
            !buttonReleaseEventAtom)
        return;

    // Find the Magellan command window on the root and check to see
    // if it actually exists.  If it doesn't exist, we assume the "3dxsrv"
    // daemon is no longer running and that there is no Magellan device.
    Window root = QX11Info::appRootWindow();
    Atom actualType;
    int actualFormat;
    unsigned long numItems;
    unsigned long bytesAfter;
    unsigned char *propValue = 0;
    if (XGetWindowProperty(dpy, root, commandEventAtom, 0, 1, False,
                           AnyPropertyType, &actualType, &actualFormat,
                           &numItems, &bytesAfter, &propValue) == 0)
        commandWindow = *((Window *)propValue);
    if (propValue)
        XFree(propValue);
    if (!commandWindow)
        return;
    XSync(dpy, False);  // Flush pending requests and process their errors.
    XErrorHandler prevHandler = XSetErrorHandler(magellan_error_handler);
    XWindowAttributes attrs;
    bool ok = (XGetWindowAttributes(dpy, commandWindow, &attrs) != 0);
    XSetErrorHandler(prevHandler);
    if (!ok)
        return;

    // Install an event filter so we can get messages back from the server.
    if (!filterInstalled) {
        prevFilter = qApp->setEventFilter(magellan_event_filter);
        filterInstalled = true;
    }

    // Fetch the keycodes for F1 ... F6, for mapping Custom keys.
    f1Keycode = XKeysymToKeycode(dpy, XK_F1);
    f2Keycode = XKeysymToKeycode(dpy, XK_F2);
    f3Keycode = XKeysymToKeycode(dpy, XK_F3);
    f4Keycode = XKeysymToKeycode(dpy, XK_F4);
    f5Keycode = XKeysymToKeycode(dpy, XK_F5);
    f6Keycode = XKeysymToKeycode(dpy, XK_F6);

    // The "3dxsrv" server is available and running.
    available = true;
}

void QMouse3DxWareDevice::motionEvent(XEvent *event)
{
    QMouse3DEvent mevent(event->xclient.data.s[2],
                         event->xclient.data.s[3],
                         event->xclient.data.s[4],
                         event->xclient.data.s[5],
                         event->xclient.data.s[6],
                         event->xclient.data.s[7]);
    motion(&mevent, false); // The events are already filtered by "3dxsrv".
}

static int magellan_map_key(int code)
{
    // See the README for more information about the button mappings.
    switch (code) {
    case 1:     return QGL::Key_Button1;
    case 2:     return QGL::Key_Button2;
    case 3:     return QGL::Key_Button3;
    case 4:     return QGL::Key_Button4;
    case 5:     return QGL::Key_Button5;
    case 6:     return QGL::Key_Button6;
    case 7:     return QGL::Key_Button7;
    case 8:     return QGL::Key_Button8;
    case 9:     return QGL::Key_Button9;
    case 10:    return QGL::Key_Button10;
    case 11:    return QGL::Key_TopView;
    case 12:    return QGL::Key_LeftView;
    case 13:    return QGL::Key_RightView;
    case 14:    return QGL::Key_FrontView;
    case 15:    return QGL::Key_BottomView;
    case 16:    return QGL::Key_BackView;
    default:    return -1;
    }
}

void QMouse3DxWareDevice::buttonPressEvent(int code)
{
    code = magellan_map_key(code);
    if (code != -1)
        keyPress(code);
}

void QMouse3DxWareDevice::buttonReleaseEvent(int code)
{
    code = magellan_map_key(code);
    if (code != -1)
        keyRelease(code);
}

bool QMouse3DxWareDevice::customKeyEvent(unsigned int keycode, bool press)
{
    // See the README for more information about the custom button mappings.
    int code = -1;
    if (keycode == f1Keycode)
        code = Qt::Key_Menu;
    else if (keycode == f2Keycode)
        code = QGL::Key_Fit;
    else if (keycode == f3Keycode)
        code = QGL::Key_RotateCW90;
    else if (keycode == f4Keycode)
        code = QGL::Key_RotateCCW90;
    else if (keycode == f5Keycode)
        code = QGL::Key_ISO1;
    else if (keycode == f6Keycode)
        code = QGL::Key_ISO2;
    if (code != -1) {
        if (press)
            keyPress(code);
        else
            keyRelease(code);
        return true;
    }
    return false;
}

QT_END_NAMESPACE
