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

#ifndef QGLDISPLAYLIST_P_H
#define QGLDISPLAYLIST_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qglgeometry_p.h"
#include "qglscenenode_p.h"
#include "qgldisplaylist.h"

#include <QtCore/qmap.h>

QT_BEGIN_NAMESPACE

class QGLDisplayList;
class QGLSection;
class QGLPrimitive;

class QGLDisplayListPrivate : public QGLSceneNodePrivate
{
    Q_DECLARE_PUBLIC(QGLDisplayList);
public:
    QGLDisplayListPrivate(int version = QObjectPrivateVersion);
    ~QGLDisplayListPrivate();
    inline void setDirty(bool dirty = true);
    void addTriangle(int a, int b, int c, QGLPrimitive &p);
    void adjustSectionNodes(QGLSection *sec, int offset, QGeometryData *geom);
    int adjustNodeTree(QGLSceneNode *top, int offset, QGeometryData *geom,
                       QList<QGLSceneNode*> &deleted);

    bool finalizeNeeded;
    QList<QGLSection*> sections;
    QGLSection *currentSection;
    QList<QGLSceneNode*> nodeStack;
    QPointer<QGLSceneNode> currentNode;
    QGLPrimitive *currentOperation;
    QGL::Operation operation;
    QList<QGLPrimitive *> primitiveQueue;
};

inline void QGLDisplayListPrivate::setDirty(bool dirty)
{
    finalizeNeeded = dirty;
}

QT_END_NAMESPACE

#endif // QGLDISPLAYLIST_P_H
