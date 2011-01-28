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

#ifndef VIEWPORTFBONODE_SG_H
#define VIEWPORTFBONODE_SG_H

#include <QtDeclarative/qdeclarative.h>

#if defined(QML_VERSION) && QML_VERSION >= 0x020000

#include <QtDeclarative/node.h>
#include <QtDeclarative/texturematerial.h>
#include <QtOpenGL/qglframebufferobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class ViewportSG;
class QGLFramebufferObject;
class QSGContext;
class QSGStereoContext;

class ViewportFboNodeSG : public GeometryNode
{
public:
    ViewportFboNodeSG(ViewportSG *viewport, QSGContext *context);
    ~ViewportFboNodeSG();

    void setSize(const QSize &size);
    QSize size() const { return m_size; }

    void setOpacity(qreal opacity);
    qreal opacity() const { return m_opacity; }

    bool useAlpha() const { return m_useAlpha; }
    void setUseAlpha(bool useAlpha);

    void preprocess();

private:
    ViewportSG *m_viewport;
    QSGStereoContext *m_context;
    QSize m_size;
    qreal m_opacity;

    struct EyeBuffer
    {
        EyeBuffer() : fbo(0) {}
        ~EyeBuffer() { delete fbo; }

        QGLFramebufferObject *fbo;
        TextureMaterial material;
        TextureMaterialWithOpacity materialO;
        QSGTextureRef texture;

        void reset() { delete fbo; fbo = 0; }
    };

    EyeBuffer m_left;
    EyeBuffer m_right;

    bool m_useAlpha;
    bool m_linearFiltering;
    bool m_leftWasDirty;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QML_VERSION >= 0x020000

#endif