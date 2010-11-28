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

#ifndef LINE_H
#define LINE_H

#include "qdeclarativeitem3d.h"
#include "qglscenenode.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Line : public QDeclarativeItem3D
{
    Q_OBJECT
    Q_PROPERTY(QVector3D startPoint READ startPoint WRITE setStartPoint NOTIFY startPointChanged)
    Q_PROPERTY(QVector3D endPoint READ endPoint WRITE setEndPoint NOTIFY endPointChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)

public:
    explicit Line(QObject *parent = 0);
    ~Line() {}

    QVector3D startPoint() const {return m_startPos;}
    void setStartPoint(QVector3D m_startPos);

    QVector3D endPoint() const {return m_endPos;}
    void setEndPoint(QVector3D m_endPos);

    qreal width() const {return m_width;}
    void setWidth(qreal width);

Q_SIGNALS:
    void startPointChanged();
    void endPointChanged();
    void widthChanged();

protected:
    void drawItem(QGLPainter *painter);

private:
    QVector3D m_startPos;
    QVector3D m_endPos;
    qreal m_width;
};

QML_DECLARE_TYPE(Line)

QT_END_NAMESPACE

QT_END_HEADER

#endif // LINE_H
