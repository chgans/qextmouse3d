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

#ifndef QGRAPHICSSCALE3D_H
#define QGRAPHICSSCALE3D_H

#include <QtGui/QGraphicsTransform>
#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Q_QT3D_EXPORT QGraphicsScale3D : public QGraphicsTransform
{
    Q_OBJECT
    Q_PROPERTY(QVector3D origin READ origin WRITE setOrigin NOTIFY originChanged)
    Q_PROPERTY(qreal xScale READ xScale WRITE setXScale NOTIFY scaleChanged)
    Q_PROPERTY(qreal yScale READ yScale WRITE setYScale NOTIFY scaleChanged)
    Q_PROPERTY(qreal zScale READ zScale WRITE setZScale NOTIFY scaleChanged)
    Q_PROPERTY(QVector3D scaleVector READ scaleVector WRITE setScaleVector NOTIFY scaleChanged)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
public:
    QGraphicsScale3D(QObject *parent = 0);
    ~QGraphicsScale3D();

    QVector3D origin() const { return m_origin; }
    void setOrigin(const QVector3D &value);

    qreal xScale() const { return m_scale.x(); }
    void setXScale(qreal value);

    qreal yScale() const { return m_scale.y(); }
    void setYScale(qreal value);

    qreal zScale() const { return m_scale.z(); }
    void setZScale(qreal value);

    QVector3D scaleVector() const { return m_scale; }
    void setScaleVector(const QVector3D &value);

    qreal scale() const;
    void setScale(qreal value);

    void applyTo(QMatrix4x4 *matrix) const;

Q_SIGNALS:
    void originChanged();
    void scaleChanged();

private:
    QVector3D m_origin;
    QVector3D m_scale;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
