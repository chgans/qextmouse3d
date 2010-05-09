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

#ifndef QGRAPHICSTRANSLATION3D_H
#define QGRAPHICSTRANSLATION3D_H

#include <QtGui/QGraphicsTransform>
#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Q_QT3D_EXPORT QGraphicsTranslation3D : public QGraphicsTransform
{
    Q_OBJECT
    Q_PROPERTY(QVector3D translate READ translate WRITE setTranslate NOTIFY translateChanged)
    Q_PROPERTY(qreal xTranslate READ xTranslate WRITE setXTranslate NOTIFY translateChanged)
    Q_PROPERTY(qreal yTranslate READ yTranslate WRITE setYTranslate NOTIFY translateChanged)
    Q_PROPERTY(qreal zTranslate READ zTranslate WRITE setZTranslate NOTIFY translateChanged)
public:
    QGraphicsTranslation3D(QObject *parent = 0);
    ~QGraphicsTranslation3D();

    QVector3D translate() const { return m_translate; }
    void setTranslate(const QVector3D &value);

    qreal xTranslate() const { return m_translate.x(); }
    void setXTranslate(qreal value);

    qreal yTranslate() const { return m_translate.y(); }
    void setYTranslate(qreal value);

    qreal zTranslate() const { return m_translate.z(); }
    void setZTranslate(qreal value);

    void applyTo(QMatrix4x4 *matrix) const;

Q_SIGNALS:
    void translateChanged();

private:
    QVector3D m_translate;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
