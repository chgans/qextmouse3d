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

#include "qmouse3dlinuxinputdevice.h"
#include "qmouse3dlcdscreen.h"
#include "qglnamespace.h"
#include <QtGui/qwidget.h>
#include <QtGui/qicon.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

QT_BEGIN_NAMESPACE

QMouse3DLinuxInputDevice::QMouse3DLinuxInputDevice
        (const QString &dName, const QString &realName, QObject *parent)
    : QMouse3DDevice(parent)
    , isOpen(false)
    , devName(dName)
    , name(realName)
    , fd(-1)
    , notifier(0)
    , flatMiddle(15)
    , mscKey(-1)
    , sawTranslate(false)
    , sawRotate(false)
    , prevWasFlat(false)
    , lcdScreen(0)
    , mouseType(QMouse3DLinuxInputDevice::MouseUnknown)
{
    memset(values, 0, sizeof(values));
    memset(tempValues, 0, sizeof(tempValues));
}

QMouse3DLinuxInputDevice::~QMouse3DLinuxInputDevice()
{
    delete notifier;
    if (fd != -1)
        ::close(fd);
}

bool QMouse3DLinuxInputDevice::isAvailable() const
{
    // Not used - QMouse3DHalDevice reports the availability state.
    // If the QMouse3DLinuxInputDevice object exists, the device is available.
    return true;
}

QStringList QMouse3DLinuxInputDevice::deviceNames() const
{
    // Not used - QMouse3DHalDevice reports the name.
    return QStringList();
}

void QMouse3DLinuxInputDevice::setWidget(QWidget *widget)
{
    QMouse3DDevice::setWidget(widget);
    if (isOpen && !widget) {
        // Close the device - we don't need it any more.
        delete notifier;
        ::close(fd);
        notifier = 0;
        fd = -1;
        isOpen = false;
    } else if (!isOpen && widget) {
        // Attempt to open the device.
        int fd = ::open(devName.toLatin1().constData(), O_RDONLY | O_NONBLOCK, 0);
        if (fd >= 0) {
            isOpen = true;
            initDevice(fd);
        }
    }
    if (lcdScreen) {
        lcdScreen->setActive(widget != 0);
        if (widget) {
            QWidget *window = widget->window();
            QString title = window->windowIconText();
            if (title.isEmpty())
                title = window->windowTitle();
            lcdScreen->setTitle(title);
            QIcon icon = window->windowIcon();
            if (icon.isNull())
                lcdScreen->setImage(QImage());
            else
                lcdScreen->setImage(icon.pixmap(128, 128).toImage());
            lcdScreen->update();
        } else {
            lcdScreen->setTitle(QString());
        }
    }
}

void QMouse3DLinuxInputDevice::initDevice(int fd)
{
    // Remember the fd for later.
    this->fd = fd;

    // Grab all events from this device so that other applications
    // in the system (particularly the X server) don't get the events.
    ::ioctl(fd, EVIOCGRAB, 1);

    // Determine the size of the "flat middle", where we clamp values to
    // zero to filter out noise when the mouse is in the center position.
    int flat = 0;
    for (int index = 0; index < 6; ++index) {
        struct input_absinfo info;
        if (::ioctl(fd, EVIOCGABS(ABS_X + index), &info) >= 0) {
            int range = qMax(qAbs(info.minimum), qAbs(info.maximum));
            if (range < 10)
                range = 500; // Zero protection.
            if (info.flat != 0)
                flat = qMax(flat, info.flat);
            else
                flat = qMax(flat, range / 10);
        } else {
            flat = qMax(flat, 16);
        }
    }
    flatMiddle = flat;

    // Create a socket notifier to receive notification of new events.
    notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(notifier, SIGNAL(activated(int)), this, SLOT(readyRead()));

    // Clear the current mouse state.
    memset(values, 0, sizeof(values));
    memset(tempValues, 0, sizeof(tempValues));
    mscKey = -1;
    sawTranslate = false;
    sawRotate = false;
    prevWasFlat = false;

    // What type of 3D mouse do we have?
    mouseType = QMouse3DLinuxInputDevice::MouseUnknown;
    if (name.contains(QLatin1String("3Dconnexion"))) {
        mouseType |= QMouse3DLinuxInputDevice::Mouse3Dconnexion;
        if (name.contains(QLatin1String("SpaceNavigator")))
            mouseType |= QMouse3DLinuxInputDevice::MouseSpaceNavigator;
        else if (name.contains(QLatin1String("SpacePilot PRO")))
            mouseType |= QMouse3DLinuxInputDevice::MouseSpacePilotPRO;
    }

    // Create a LCD screen handler if we have a SpacePilot PRO.
    if (!lcdScreen && (mouseType & MouseSpacePilotPRO) != 0)
        lcdScreen = new QMouse3DSpacePilotPROScreen(this);
}

static inline int clampRange(int value)
{
    return qMin(qMax(value, -32768), 32767);
}

void QMouse3DLinuxInputDevice::readyRead()
{
    // Read as many events as we can in case the event queue has gotten
    // backed up due to an application delay.  We only care about the
    // most recent mouse position.
    struct input_event event;
    bool deliverMotion = false;
    while (read(fd, &event, sizeof(event)) == int(sizeof(event))) {
        if (event.type == EV_ABS || event.type == EV_REL) {
            if (event.code <= ABS_RZ) {
                tempValues[event.code] = clampRange(event.value);
                if (event.code >= ABS_RX)
                    sawRotate = true;
                else
                    sawTranslate = true;
            }
        } else if (event.type == EV_MSC && event.code == MSC_SCAN &&
                   (mouseType & QMouse3DLinuxInputDevice::Mouse3Dconnexion) != 0) {
            // If the value is between 0x90001 and 0x9001f then we
            // assume that it is a 3Dconnexion special key and then
            // wait for the EV_KEY to tell us the press/release state.
            if (event.value >= 0x90001 && event.value <= 0x9001f)
                mscKey = event.value;
        } else if (event.type == EV_KEY && mscKey != -1) {
            translateMscKey(mscKey, event.value != 0);
            mscKey = -1;
        } else if (event.type == EV_SYN) {
            mscKey = -1;
            if (sawTranslate) {
                deliverMotion = true;
                sawTranslate = false;
                values[0] = tempValues[0];
                values[1] = tempValues[1];
                values[2] = tempValues[2];
                tempValues[0] = tempValues[1] = tempValues[2] = 0;
                if (qAbs(values[0]) < flatMiddle &&
                        qAbs(values[1]) < flatMiddle &&
                        qAbs(values[2]) < flatMiddle) {
                    values[0] = 0;
                    values[1] = 0;
                    values[2] = 0;
                }
            }
            if (sawRotate) {
                deliverMotion = true;
                sawRotate = false;
                values[3] = tempValues[3];
                values[4] = tempValues[4];
                values[5] = tempValues[5];
                tempValues[3] = tempValues[4] = tempValues[5] = 0;
                if (qAbs(values[3]) < flatMiddle &&
                        qAbs(values[4]) < flatMiddle &&
                        qAbs(values[5]) < flatMiddle) {
                    values[3] = 0;
                    values[4] = 0;
                    values[5] = 0;
                }
            }
        }
    }
    if (deliverMotion) {
        // Filter out multiple "flat" events so we don't get too much noise
        // being delivered up to the widget layer.
        bool isFlat = (values[0] == 0 && values[1] == 0 && values[2] == 0 &&
                       values[3] == 0 && values[4] == 0 && values[5] == 0);
        if (prevWasFlat && isFlat)
            deliverMotion = false;
        prevWasFlat = isFlat;
    }
    if (deliverMotion) {
        // Deliver the motion event and ask QMouse3DDevice to filter it.
        QMouse3DEvent mevent
            ((short)(values[0]), (short)(values[1]), (short)(values[2]),
             (short)(values[3]), (short)(values[4]), (short)(values[5]));
        motion(&mevent);
    }
}

// These are the keycodes that are reported by the 3Dconnection
// SpacePilot PRO via the Linux input event interface as EV_MSC events.
// The SpaceNavigator reports similar events, but only has 2 buttons,
// which we map to the same behavior as SPKey_Pan and SPKey_Rotation.
//
// http://www.3dconnexion.com/products/spacepilot-pro.html
// http://www.3dconnexion.com/products/spacenavigator.html
enum SpacePilotKeys
{
    SPKey_Menu              = 0x90001,  // Left button on SpaceNavigator
    SPKey_Fit               = 0x90002,  // Right button on SpaceNavigator

    SPKey_TopView           = 0x90003,
    SPKey_LeftView          = 0x90004,
    SPKey_RightView         = 0x90005,
    SPKey_FrontView         = 0x90006,
    SPKey_BottomView        = 0x90007,
    SPKey_BackView          = 0x90008,

    SPKey_RotateCW90        = 0x90009,  // Rotate 90 degrees clockwise
    SPKey_RotateCCW90       = 0x9000a,  // Rotate 90 degrees counter-clockwise

    SPKey_ISO1              = 0x9000b,  // Isometric top/front/right view
    SPKey_ISO2              = 0x9000c,  // Isometric top/back/left view

    SPKey_F1                = 0x9000d,
    SPKey_F2                = 0x9000e,
    SPKey_F3                = 0x9000f,
    SPKey_F4                = 0x90010,
    SPKey_F5                = 0x90011,
    SPKey_F6                = 0x90012,
    SPKey_F7                = 0x90013,
    SPKey_F8                = 0x90014,
    SPKey_F9                = 0x90015,
    SPKey_F10               = 0x90016,

    SPKey_Escape            = 0x90017,
    SPKey_Alt               = 0x90018,
    SPKey_Shift             = 0x90019,
    SPKey_Control           = 0x9001a,

    SPKey_Rotation          = 0x9001b,  // Toggle rotation on/off
    SPKey_Pan               = 0x9001c,  // Toggle pan on/off
    SPKey_Dominant          = 0x9001d,  // Toggle main axis being dominant
    SPKey_IncSensitivity    = 0x9001e,  // Increase sensitivity of movements
    SPKey_DecSensitivity    = 0x9001f   // Decrease sensitivity of movements
};

void QMouse3DLinuxInputDevice::translateMscKey(int code, bool press)
{
    int qtcode = -1;
    switch (code) {
    case SPKey_Menu:
        // On the SpaceNavigator, map this key to "translation lock".
        if (mouseType & QMouse3DLinuxInputDevice::MouseSpaceNavigator) {
            if (press)
                toggleFilter(QMouse3DEventProvider::Translations);
            qtcode = QGL::Key_Translations;
        } else {
            qtcode = Qt::Key_Menu;
        }
        break;

    case SPKey_Fit:
        // On the SpaceNavigator, map this key to "rotation lock".
        if (mouseType & QMouse3DLinuxInputDevice::MouseSpaceNavigator) {
            if (press)
                toggleFilter(QMouse3DEventProvider::Rotations);
            qtcode = QGL::Key_Rotations;
        } else {
            qtcode = QGL::Key_Fit;
        }
        break;

    case SPKey_TopView:         qtcode = QGL::Key_TopView; break;
    case SPKey_LeftView:        qtcode = QGL::Key_LeftView; break;
    case SPKey_RightView:       qtcode = QGL::Key_RightView; break;
    case SPKey_FrontView:       qtcode = QGL::Key_FrontView; break;
    case SPKey_BottomView:      qtcode = QGL::Key_BottomView; break;
    case SPKey_BackView:        qtcode = QGL::Key_BackView; break;
    case SPKey_RotateCW90:      qtcode = QGL::Key_RotateCW90; break;
    case SPKey_RotateCCW90:     qtcode = QGL::Key_RotateCCW90; break;
    case SPKey_ISO1:            qtcode = QGL::Key_ISO1; break;
    case SPKey_ISO2:            qtcode = QGL::Key_ISO2; break;
    case SPKey_F1:              qtcode = QGL::Key_Button1; break;
    case SPKey_F2:              qtcode = QGL::Key_Button2; break;
    case SPKey_F3:              qtcode = QGL::Key_Button3; break;
    case SPKey_F4:              qtcode = QGL::Key_Button4; break;
    case SPKey_F5:              qtcode = QGL::Key_Button5; break;
    case SPKey_F6:              qtcode = QGL::Key_Button6; break;
    case SPKey_F7:              qtcode = QGL::Key_Button7; break;
    case SPKey_F8:              qtcode = QGL::Key_Button8; break;
    case SPKey_F9:              qtcode = QGL::Key_Button9; break;
    case SPKey_F10:             qtcode = QGL::Key_Button10; break;
    case SPKey_Escape:          qtcode = Qt::Key_Escape; break;
    case SPKey_Alt:             qtcode = Qt::Key_Alt; break;
    case SPKey_Shift:           qtcode = Qt::Key_Shift; break;
    case SPKey_Control:         qtcode = Qt::Key_Control; break;

    case SPKey_Rotation:
        if (press)
            toggleFilter(QMouse3DEventProvider::Rotations);
        qtcode = QGL::Key_Rotations;
        break;
    case SPKey_Pan:
        if (press)
            toggleFilter(QMouse3DEventProvider::Translations);
        qtcode = QGL::Key_Translations;
        break;
    case SPKey_Dominant:
        if (press)
            toggleFilter(QMouse3DEventProvider::DominantAxis);
        qtcode = QGL::Key_DominantAxis;
        break;
    case SPKey_IncSensitivity:
        if (press)
            adjustSensitivity(2.0f);
        qtcode = QGL::Key_IncreaseSensitivity;
        break;
    case SPKey_DecSensitivity:
        if (press)
            adjustSensitivity(0.5f);
        qtcode = QGL::Key_DecreaseSensitivity;
        break;
    default: break;
    }
    if (qtcode != -1) {
        if (press)
            keyPress(qtcode);
        else
            keyRelease(qtcode);
    }
}

QT_END_NAMESPACE
