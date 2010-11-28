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

#ifndef CAPSULE_H
#define CAPSULE_H

#include "qdeclarativeitem3d.h"
#include "qglscenenode.h"
#include <QtCore/qmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Capsule : public QDeclarativeItem3D
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(int levelOfDetail READ levelOfDetail WRITE setLevelOfDetail NOTIFY levelOfDetailChanged)

public:
    explicit Capsule(QObject *parent = 0);
    ~Capsule() {}

    qreal radius() const { return m_radius; }
    void setRadius(qreal radius);

    qreal height() const {return m_height;}
    void setHeight(qreal height);

    int levelOfDetail() const {return m_lod;}
    void setLevelOfDetail(int lod);

Q_SIGNALS:
    void radiusChanged();
    void heightChanged();
    void levelOfDetailChanged();

protected:
    void drawItem(QGLPainter *painter);

private:
    qreal m_radius;
    qreal m_height;
    int m_lod;
    QMap<int, QGLSceneNode *> m_lodGeometry;
};

QML_DECLARE_TYPE(Capsule)

QT_END_NAMESPACE

QT_END_HEADER

#endif // CAPSULE_H
