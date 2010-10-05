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

#include "buttons.h"
#include "qglpainter.h"

#include <QDebug>

Buttons::Buttons(QObject *parent, QGLMaterialCollection *palette)
    : QGLSceneNode(parent)
{
    setObjectName("Buttons");
    setPalette(palette);
    setBoundingBoxTestEnabled(false);

    m_left = new QGLSceneNode(this);
    m_left->setObjectName("Left Button");
    m_right = new QGLSceneNode(this);
    m_right->setObjectName("Right Button");

    QGLMaterial *mat = new QGLMaterial;
    QImage im(":/controls/arrows-left.png");
    m_size = im.size();
    QGLTexture2D *tex = new QGLTexture2D(mat);
    tex->setImage(im);
    mat->setTexture(tex);

    setMaterial(mat);
    setEffect(QGL::FlatReplaceTexture2D);

    QGeometryData data;
    QSize f = im.size() / 2;
    QVector2D a(-f.width(), -f.height());
    QVector2D b(f.width(), -f.height());
    QVector2D c(f.width(), f.height());
    QVector2D d(-f.width(), f.height());
    QVector2D ta(0, 1);
    QVector2D tb(1, 1);
    QVector2D tc(1, 0);
    QVector2D td(0, 0);
    data.appendVertex(a, b, c, d);
    data.appendTexCoord(ta, tb, tc, td);
    data.appendIndices(0, 1, 2);
    data.appendIndices(0, 2, 3);

    // the right hand arrow geometry is same as above, flipped X <-> -X
    data.appendGeometry(data);
    data.texCoord(4).setX(1);
    data.texCoord(5).setX(0);
    data.texCoord(6).setX(0);
    data.texCoord(7).setX(1);
    data.appendIndices(4, 5, 6);
    data.appendIndices(4, 6, 7);

    m_left->setGeometry(data);
    m_left->setCount(6);
    m_left->setBoundingBoxTestEnabled(false);

    m_right->setGeometry(data);
    m_right->setStart(6);
    m_right->setCount(6);
    m_right->setBoundingBoxTestEnabled(false);
}

void Buttons::draw(QGLPainter *painter)
{
    painter->projectionMatrix().push();
    painter->modelViewMatrix().push();

    QRect rect = painter->currentSurface()->viewportRect();
    QMatrix4x4 projm;
    projm.ortho(rect);
    painter->projectionMatrix() = projm;
    painter->modelViewMatrix().setToIdentity();

    if (m_left->position().isNull())
    {
        QVector2D pos(m_size.width() / 2, rect.height() - m_size.height() / 2);
        m_left->setPosition(pos);
        pos.setX(rect.width() - (m_size.width() / 2));
        m_right->setPosition(pos);
    }

    glDisable(GL_DEPTH_TEST);

    QGLSceneNode::draw(painter);

    glEnable(GL_DEPTH_TEST);

    painter->projectionMatrix().pop();
    painter->modelViewMatrix().pop();
}
