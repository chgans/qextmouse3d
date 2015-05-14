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

#ifndef QMOUSE3DEVENTPROVIDER_H
#define QMOUSE3DEVENTPROVIDER_H

#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qnamespace.h>
#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QExtMouse3DEventProviderPrivate;

class QWidget;

class Q_QT3D_EXPORT QExtMouse3DEventProvider : public QObject
{
    Q_OBJECT
public:
    QExtMouse3DEventProvider(QObject *parent = 0);
    ~QExtMouse3DEventProvider();

    bool isAvailable() const;
    QStringList deviceNames() const;

    QWidget *widget() const;
    void setWidget(QWidget *widget);

    enum Filter
    {
        NoFilters       = 0x0000,
        Translations    = 0x0001,
        Rotations       = 0x0002,
        DominantAxis    = 0x0004,
        Sensitivity     = 0x0008,
        AllFilters      = 0xFFFF
    };
    Q_DECLARE_FLAGS(Filters, Filter)

    QExtMouse3DEventProvider::Filters filters() const;
    void setFilters(QExtMouse3DEventProvider::Filters filters);
    void toggleFilter(QExtMouse3DEventProvider::Filter filter);

    QExtMouse3DEventProvider::Filters keyFilters() const;
    void setKeyFilters(QExtMouse3DEventProvider::Filters filters);

    qreal sensitivity() const;
    void setSensitivity(qreal value);

Q_SIGNALS:
    void availableChanged();
    void filtersChanged();
    void sensitivityChanged();

private:
    QScopedPointer<QExtMouse3DEventProviderPrivate> d_ptr;

    Q_DISABLE_COPY(QExtMouse3DEventProvider)
    Q_DECLARE_PRIVATE(QExtMouse3DEventProvider)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QExtMouse3DEventProvider::Filters)

QT_END_NAMESPACE

QT_END_HEADER

#endif
