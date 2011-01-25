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

#ifndef STEREOVIEW_SG_H
#define STEREOVIEW_SG_H

#include <QtDeclarative/qdeclarative.h>

#if defined(QML_VERSION) && QML_VERSION >= 0x020000

#include "qsgitem.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class StereoViewSG : public QSGItem
{
    Q_OBJECT
    Q_ENUMS(Layout)
    Q_PROPERTY(Layout layout READ layout WRITE setLayout NOTIFY layoutChanged)
    Q_PROPERTY(QSGItem *viewport READ viewport NOTIFY viewportChanged)
public:
    StereoViewSG(QSGItem *parent = 0);
    ~StereoViewSG();

    enum Layout
    {
        Default,
        Hardware,
        RedCyan,
        LeftRight,
        RightLeft,
        TopBottom,
        BottomTop,
        Disabled
    };

    // Scenegraph viewer handles stereo - cannot alter the layout.
    StereoViewSG::Layout layout() const { return Default; }
    void setLayout(StereoViewSG::Layout layout) { Q_UNUSED(layout); }

    QSGItem *viewport() { return this; }

Q_SIGNALS:
    void layoutChanged();
    void viewportChanged();
};

QML_DECLARE_TYPE(StereoViewSG)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QML_VERSION >= 0x020000

#endif
