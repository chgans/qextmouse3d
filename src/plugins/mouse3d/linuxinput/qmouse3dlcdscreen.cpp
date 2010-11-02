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

#include "qmouse3dlcdscreen.h"
#include <QtGui/qpainter.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QMouse3DLcdScreen::QMouse3DLcdScreen(QObject *parent)
    : QObject(parent)
    , m_defaultImage(QLatin1String(":/Qt3D/Icons/qt3dlogo.png"))
    , m_filters(0)
{
}

QMouse3DLcdScreen::~QMouse3DLcdScreen()
{
}

void QMouse3DLcdScreen::update()
{
    // Create a blank screen image.
    QSize screenSz = screenSize();
    QImage screen(screenSz, screenFormat());
    screen.fill(0);
    QPainter painter;
    painter.begin(&screen);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // Draw the window title at the top of the screen.
    QRect screenRect(QPoint(0, 0), screenSz);
    if (!m_title.isEmpty()) {
        QFont font(painter.font());
        font.setPointSize(font.pointSize() + 6);
        font.setWeight(QFont::Bold);
        painter.setFont(font);
        painter.setPen(Qt::white);
        QString title = painter.fontMetrics().elidedText
            (m_title, Qt::ElideRight, screenSz.width());
        QSize sz = painter.fontMetrics().size(0, title);
        QRectF r((screenRect.width() - sz.width()) / 2, 0,
                 sz.width(), sz.height());
        painter.drawText(r, title);
        screenRect.setY(screenRect.y() + sz.height());
    }

    // Draw the window icon image in the middle of the screen remainder.
    QImage image(m_image);
    if (image.isNull())
        image = m_defaultImage;
    QSize size = image.size();
    size.scale(screenRect.size(), Qt::KeepAspectRatio);
    QRect rect(screenRect.x() + (screenRect.width() - size.width()) / 2,
               screenRect.y() + (screenRect.height() - size.height()) / 2,
               size.width(), size.height());
    painter.drawImage(rect, image);

    // Send the screen image to the device.
    painter.end();
    setScreen(screen);
}

QMouse3DSpacePilotPROScreen::QMouse3DSpacePilotPROScreen(QObject *parent)
    : QMouse3DLcdScreen(parent)
#ifdef QT_HAVE_LIBUSB
    , m_handle(0)
    , m_interface(0)
#endif
{
#ifdef QT_HAVE_LIBUSB
    usb_init();
#endif
}

QMouse3DSpacePilotPROScreen::~QMouse3DSpacePilotPROScreen()
{
#ifdef QT_HAVE_LIBUSB
    if (m_handle) {
        clearScreen();
        usb_release_interface(m_handle, m_interface);
        usb_close(m_handle);
    }
#endif
}

void QMouse3DSpacePilotPROScreen::setActive(bool enable)
{
#ifdef QT_HAVE_LIBUSB
    if (enable && !m_handle) {
        // Find the device on the USB bus and open it.
        usb_find_busses();
        usb_find_devices();
        struct usb_bus *bus = usb_get_busses();
        struct usb_device *dev = 0;
        while (bus != 0 && !dev) {
            dev = bus->devices;
            while (dev != 0) {
                if (dev->descriptor.idVendor == 0x046d &&
                        dev->descriptor.idProduct == 0xc629)
                    break;
                dev = dev->next;
            }
            bus = bus->next;
        }
        if (!dev)
            return;
        usb_dev_handle *handle = usb_open(dev);
        if (!handle)
            return;
        int iface = -1;
        for (int index = 0; index < dev->config->bNumInterfaces; ++index) {
            struct usb_interface_descriptor *desc;
            desc = dev->config->interface[index].altsetting;
            if (desc->bInterfaceClass == 255) {
                iface = desc->bInterfaceNumber;
                break;
            }
        }
        if (iface == -1) {
            usb_close(handle);
            return;
        }
        if (usb_claim_interface(handle, iface) < 0) {
            usb_close(handle);
            return;
        }
        m_handle = handle;
        m_interface = iface;
        clearScreen();
    } else if (!enable && m_handle) {
        // Release and close the device.
        clearScreen();
        usb_release_interface(m_handle, m_interface);
        usb_close(m_handle);
        m_handle = 0;
        m_interface = 0;
    }
#else
    Q_UNUSED(enable);
#endif
}

QImage::Format QMouse3DSpacePilotPROScreen::screenFormat() const
{
    return QImage::Format_RGB16;
}

QSize QMouse3DSpacePilotPROScreen::screenSize() const
{
    return QSize(320, 240);
}

void QMouse3DSpacePilotPROScreen::setScreen(const QImage &screen)
{
    Q_UNUSED(screen);

#ifdef QT_HAVE_LIBUSB
    // Bail out if the device is not currently active.
    if (!m_handle)
        return;

    // The image needs to be rotated and flipped for the SpacePilot PRO.
    QTransform transform;
    transform.rotate(-90);
    QImage tempImage = screen.transformed(transform).mirrored();

    // Write the new image to the device.
    writeImage(tempImage);
#endif
}

void QMouse3DSpacePilotPROScreen::clearScreen()
{
    QImage tempImage(320, 240, QImage::Format_RGB16);
    tempImage.fill(0);
    writeImage(tempImage);
}

void QMouse3DSpacePilotPROScreen::writeImage(const QImage &image)
{
#ifdef QT_HAVE_LIBUSB
    char header[512];
    int count = 320 * 240 * 2;
    memset(header, 0, sizeof(header));
    header[0]  = 0x10;
    header[1]  = 0x0F;
    header[2]  = (char)count;
    header[3]  = (char)(count >> 8);
    header[4]  = (char)(count >> 16);
    header[5]  = (char)(count >> 24);
    header[6]  = 0x00;
    header[7]  = 0x00;
    header[8]  = 0x00;
    header[9]  = 0x00;
    header[10] = 0x00;
    header[11] = (char)319;
    header[12] = (char)(319 >> 8);
    header[13] = (char)239;
    header[14] = (char)(239 >> 8);
    usb_bulk_write(m_handle, 0x02, header, 512, 100);
    usb_bulk_write(m_handle, 0x02, (char *)image.constBits(), count, 100);
#endif
}

QT_END_NAMESPACE
