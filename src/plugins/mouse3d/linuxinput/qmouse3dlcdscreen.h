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

#ifndef QMOUSE3DLCDSCREEN_H
#define QMOUSE3DLCDSCREEN_H

#include "qmouse3deventprovider.h"
#include <QtCore/qobject.h>
#include <QtGui/qimage.h>

#ifdef QT_HAVE_LIBUSB
#include <usb.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QExtMouse3DLcdScreen : public QObject
{
    Q_OBJECT
public:
    QExtMouse3DLcdScreen(QObject *parent = 0);
    ~QExtMouse3DLcdScreen();

    void setImage(const QImage &image) { m_image = image; }
    void setTitle(const QString &title) { m_title = title; }
    void setFilters(QExtMouse3DEventProvider::Filters filters) { m_filters = filters; }

    virtual void setActive(bool enable) = 0;
    void update();

protected:
    virtual QImage::Format screenFormat() const = 0;
    virtual QSize screenSize() const = 0;
    virtual void setScreen(const QImage &screen) = 0;

private:
    QImage m_defaultImage;
    QImage m_image;
    QString m_title;
    QExtMouse3DEventProvider::Filters m_filters;
};

class QExtMouse3DSpacePilotPROScreen : public QExtMouse3DLcdScreen
{
    Q_OBJECT
public:
    QExtMouse3DSpacePilotPROScreen(QObject *parent = 0);
    ~QExtMouse3DSpacePilotPROScreen();

    void setActive(bool enable);

protected:
    QImage::Format screenFormat() const;
    QSize screenSize() const;
    void setScreen(const QImage &screen);

private:
#ifdef QT_HAVE_LIBUSB
    usb_dev_handle *m_handle;
    int m_interface;
#endif

    void clearScreen();
    void writeImage(const QImage &image);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
