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

#include "modelview.h"
#include "qglabstractscene.h"
#include "qglscenenode.h"

ModelView::ModelView(QWidget *parent)
    : QGLView(parent)
    , m_scene(0)
    , m_main(0)
{
}

//! [initialize]
void ModelView::initializeGL(QGLPainter *painter)
{
    Q_UNUSED(painter);
    camera()->setEye(QVector3D(0.0f, 2.0f, 25.0f));

    m_scene = QGLAbstractScene::loadScene(QLatin1String(":/penguin.3ds"));
    if (m_scene)
        m_main = m_scene->defaultObject(QGLSceneNode::Main);

    QQuaternion xt = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -90.0f);
    QQuaternion yt = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 30.0f);
    m_pose = yt * xt;
}
//! [initialize]

//! [paint]
void ModelView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().rotate(m_pose);

    if (m_main)
        m_main->draw(painter);
}
//! [paint]
