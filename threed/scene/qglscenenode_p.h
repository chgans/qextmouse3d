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

#ifndef QGLSCENENODE_P_H
#define QGLSCENENODE_P_H

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

#include "qglnamespace.h"
#include "qglscenenode.h"

#include <QtGui/qmatrix4x4.h>
#include <QtCore/qlist.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qset.h>
#include <QtCore/private/qobject_p.h>

#if QT_VERSION >= 0x040700 && !defined(QT_NO_DECLARATIVE)
#include <QtDeclarative/qdeclarativelist.h>
#endif

class QGLAbstractEffect;
class QGLPickNode;

class QGLSceneNodePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGLSceneNode)
public:
    QGLSceneNodePrivate(QGLSceneNode::Type t, int version = QObjectPrivateVersion)
        : QObjectPrivate(version)
        , type(t)
        , geometry(0)
        , palette(0)
        , localEffect(QGL::FlatColor)   // 0 - zero
        , customEffect(0)
        , hasEffect(false)
        , material(-1)
        , start(0)
        , count(0)
        , viewNormals(false)
        , pickNode(0)
        , boxValid(false)
        , transformValid(false)
        , drawingMode(QGLSceneNode::Triangles)
    {
    }

#if 0
    static void appendFunc(QDeclarativeListProperty<QGLSceneNode> *list, QGLSceneNode*node);
    static int countFunc(QDeclarativeListProperty<QGLSceneNode> *list);
    static QGLSceneNode *atFunc(QDeclarativeListProperty<QGLSceneNode> *list, int index);
    static void clearFunc(QDeclarativeListProperty<QGLSceneNode> *list);
#endif
    void invalidateParentBoundingBox() const
    {
        QList<QGLSceneNode*>::const_iterator it = parentNodes.constBegin();
        for ( ; it != parentNodes.constEnd(); ++it)
            (*it)->invalidateBoundingBox();
    }

    QGLSceneNode::Type type;
    QGeometryData geometry;
    QGLMaterialCollection *palette;
    QMatrix4x4 localTransform;
    QVector3D translate;
    QVector3D rotate;
    QVector3D scale;
    QGL::StandardEffect localEffect;
    QGLAbstractEffect *customEffect;
    QList<QGLSceneNode*> childNodes;
    QList<QGLSceneNode*> parentNodes;
    bool hasEffect;
    int material;
    int start;
    int count;
    bool viewNormals;
    QGLPickNode *pickNode;
    mutable QBox3D bb;
    mutable bool boxValid;
    mutable QMatrix4x4 transform;
    mutable bool transformValid;
    QGLSceneNode::DrawingMode drawingMode;
};

#endif // QGLSCENENODE_P_H
