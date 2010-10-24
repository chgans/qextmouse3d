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

#ifndef QGLTEXTURE2D_P_H
#define QGLTEXTURE2D_P_H

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

#include "qgltexture2d.h"
#include "qgltextureutils_p.h"
#include "qurl.h"

#include <QtCore/qatomic.h>

QT_BEGIN_NAMESPACE

class QGLTexture2DTextureInfo
{
public:
    QGLTexture2DTextureInfo
            (const QGLContext *context, GLuint textureId, uint imageGeneration,
             uint parameterGeneration, bool isLiteral = false)
        : tex(context)
    {
        if (textureId)
            tex.setTextureId(textureId);
        this->imageGeneration = imageGeneration;
        this->parameterGeneration = parameterGeneration;
        this->isLiteral = isLiteral;
        this->next = 0;
    }

    QGLBoundTexture tex;
    uint imageGeneration;
    uint parameterGeneration;
    bool isLiteral;
    QGLTexture2DTextureInfo *next;
};

class DDSFormat;

class QGLTexture2DPrivate : public QObject
{
    Q_OBJECT
public:
    QGLTexture2DPrivate();
    ~QGLTexture2DPrivate();

    QSize size;
    QSize requestedSize;
    QImage image;
    QUrl url;
    QByteArray compressedData;
    QGLContext::BindOptions bindOptions;
    QGL::TextureWrap horizontalWrap;
    QGL::TextureWrap verticalWrap;
#if !defined(QT_OPENGL_ES)
    bool mipmapSupported;
    bool mipmapSupportedKnown;
#endif
    uint imageGeneration;
    uint parameterGeneration;
    QGLTexture2DTextureInfo *infos;

    bool bind(GLenum target);
    virtual void bindImages(QGLTexture2DTextureInfo *info);

private Q_SLOTS:
    void destroyContext(const QGLContext *context);
};

QT_END_NAMESPACE

#endif
