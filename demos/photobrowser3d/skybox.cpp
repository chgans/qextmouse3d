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

#include "skybox.h"
#include "qgldisplaylist.h"
#include "qglcube.h"
#include "qglpainter.h"
#include "qglview.h"
#include "qgloperation.h"

#include <QDir>
#include <QFileInfo>

SkyBox::SkyBox(QGLView *view, const QString &imagePath)
    : m_list(new QGLDisplayList)
    , m_view(view)
{
    m_list->newSection(QGL::Faceted);
    m_list->setEffect(QGL::FlatReplaceTexture2D);
    m_list->setEffectEnabled(true);
    QVector3D blb(-1.0, -1.0, -1.0);
    QVector3D blf(-1.0, -1.0, 1.0);
    QVector3D tlf(-1.0, 1.0, 1.0);
    QVector3D tlb(-1.0, 1.0, -1.0);
    QVector3D brb(1.0, -1.0, -1.0);
    QVector3D brf(1.0, -1.0, 1.0);
    QVector3D trf(1.0, 1.0, 1.0);
    QVector3D trb(1.0, 1.0, -1.0);
    QVector2D bl(0.0f, 0.0f);
    QVector2D br(1.0f, 0.0f);
    QVector2D tr(1.0f, 1.0f);
    QVector2D tl(0.0f, 1.0f);
    {
        QGLPrimitive q;   // left
        q.appendVertex(blf, blb, tlb, tlf);
        q.appendTexCoord(bl, br, tr, tl);
        m_list->addQuad(q);
        m_faces[0] = m_list->currentNode();
        m_faces[0]->setMaterial(new QGLMaterial);
    }
    {
        m_list->newNode();   // top
        QGLPrimitive q;
        q.appendVertex(trf, tlf, tlb, trb);
        q.appendTexCoord(bl, br, tr, tl);
        m_list->addQuad(q);
        m_faces[1] = m_list->currentNode();
        m_faces[1]->setMaterial(new QGLMaterial);
    }
    {
        m_list->newNode();    // right
        QGLPrimitive q;
        q.appendVertex(brb, brf, trf, trb);
        q.appendTexCoord(bl, br, tr, tl);
        m_list->addQuad(q);
        m_faces[2] = m_list->currentNode();
        m_faces[2]->setMaterial(new QGLMaterial);
    }
    {
        m_list->newNode();    // bottom
        QGLPrimitive q;
        q.appendVertex(blf, brf, brb, blb);
        q.appendTexCoord(bl, br, tr, tl);
        m_list->addQuad(q);
        m_faces[3] = m_list->currentNode();
        m_faces[3]->setMaterial(new QGLMaterial);
    }
    {
        m_list->newNode();    // front
        QGLPrimitive q;
        q.appendVertex(brf, blf, tlf, trf);
        q.appendTexCoord(bl, br, tr, tl);
        m_list->addQuad(q);
        m_faces[4] = m_list->currentNode();
        m_faces[4]->setMaterial(new QGLMaterial);
    }
    {
        m_list->newNode();    // back
        QGLPrimitive q;
        q.appendVertex(blb, brb, trb, tlb);
        q.appendTexCoord(bl, br, tr, tl);
        m_list->addQuad(q);
        m_faces[5] = m_list->currentNode();
        m_faces[5]->setMaterial(new QGLMaterial);
    }
    m_list->finalize();

    setImagePath(imagePath.isEmpty() ? QString(":/") : imagePath);
}


void SkyBox::setImagePath(const QString &imagePath)
{
    static QStringList expected2;
    static QStringList expected;

    if (expected.isEmpty())
        expected << "east" << "up" << "west" << "down" << "south" << "north";
    if (expected2.isEmpty())
        expected2 << "right" << "top" << "left" << "bottom" << "front" << "back";
    if (imagePath != m_imagePath)
    {
        m_imagePath = imagePath;
        QStringList notFound = expected;
        QDir imDir(m_imagePath);
        QFileInfoList files = imDir.entryInfoList(QDir::Files);
        QFileInfoList::const_iterator it = files.constBegin();
        for ( ; it != files.constEnd() && notFound.size() > 0; ++it)
        {
            QFileInfo ent = *it;
            QString tok = ent.baseName().toLower();
            int ix = 0;
            for ( ; ix < 6; ++ix)
                if (tok.contains(expected.at(ix)))
                    break;
            if (ix == 6)
            {
                ix = 0;
                for ( ; ix < 6; ++ix)
                    if (tok.contains(expected2.at(ix)))
                        break;
            }
            if (ix != 6)
            {
                notFound.removeOne(expected.at(ix));
                QUrl url;
                url.setScheme("file");
                url.setPath(ent.absoluteFilePath());
                m_faces[ix]->material()->setTextureUrl(url);
                m_faces[ix]->material()->texture()->setHorizontalWrap(QGL::Clamp);
                m_faces[ix]->material()->texture()->setVerticalWrap(QGL::Clamp);
            }
        }
        if (notFound.size() > 2)
        {
            qWarning("Could not load textures for");
            for (int i = 0; i < notFound.size(); ++i)
                qWarning("\t%s", qPrintable(notFound.at(i)));
        }
    }
}

void SkyBox::draw(QGLPainter *painter) const
{
    painter->modelViewMatrix().push();
    painter->modelViewMatrix().setToIdentity();

    QGLCamera *cam = m_view->camera();
    QVector3D eye = cam->eye();
    QVector3D center = cam->center();
    qreal nearPlane = cam->nearPlane();
    QSizeF size = cam->viewSize();
    cam->setCenter(-eye);
    cam->setEye(QVector3D());
    cam->setNearPlane(0.3f);
    cam->setViewSize(QSizeF(0.3f, 0.3f));
    painter->setCamera(cam);

    painter->setDepthTestingEnabled(false);

    m_list->draw(painter);

    painter->setDepthTestingEnabled(true);

    cam->setCenter(center);
    cam->setEye(eye);
    cam->setNearPlane(nearPlane);
    cam->setViewSize(size);
    painter->setCamera(cam);

#ifndef QT_OPENGL_ES_2
    glPopAttrib();
#endif

    painter->modelViewMatrix().pop();
}
