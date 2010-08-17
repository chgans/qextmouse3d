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

#ifndef PANE_H
#define PANE_H

#include "item3d.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Pane : public Item3d
{
    Q_OBJECT
    Q_ENUMS(Orientation)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(bool twoSided READ twoSided WRITE setTwoSided NOTIFY twoSidedChanged)
    Q_PROPERTY(Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
public:
    Pane(QObject *parent = 0);
    ~Pane();

    qreal width() const;
    void setWidth(qreal value);

    qreal height() const;
    void setHeight(qreal value);

    bool twoSided() const;
    void setTwoSided(bool value);

    enum Orientation
    {
        Normal,
        Rot90,
        Rot180,
        Rot270
    };

    Orientation orientation() const;
    void setOrientation(Orientation value);

protected:
    void drawItem(QGLPainter *painter);

Q_SIGNALS:
    void widthChanged();
    void heightChanged();
    void twoSidedChanged();
    void orientationChanged();

private:
    qreal m_width;
    qreal m_height;
    bool m_twoSided;
    Orientation m_orientation;
};

QML_DECLARE_TYPE(Pane)

QT_END_NAMESPACE

QT_END_HEADER

#endif