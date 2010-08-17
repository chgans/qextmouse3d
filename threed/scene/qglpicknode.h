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

#ifndef QGLPICKNODE_H
#define QGLPICKNODE_H

#include <QtCore/qobject.h>
#include "qt3dglobal.h"

class QGLAbstractScene;
class QGLSceneNode;
class QEvent;

class Q_QT3D_EXPORT QGLPickNode : public QObject
{
    Q_OBJECT
public:
    explicit QGLPickNode(QGLAbstractScene *parent = 0);
    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    QGLSceneNode *target() const { return m_target; }
    void setTarget(QGLSceneNode *target) { m_target = target; }

signals:
    void pressed();
    void released();
    void clicked();
    void doubleClicked();
    void hoverChanged();

public slots:

protected:
    bool event(QEvent *e);
    int m_id;
    QGLSceneNode *m_target;
};

#endif // QGLPICKNODE_H