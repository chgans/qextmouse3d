/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QGLTEXTURECUBEMAP_H
#define QGLTEXTURECUBEMAP_H

#include "qglnamespace.h"
#include <QtOpenGL/qgl.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLTextureCubePrivate;

class Q_QT3D_EXPORT QGLTextureCube
{
public:
    QGLTextureCube();
    ~QGLTextureCube();

    enum Face
    {
        PositiveX,
        NegativeX,
        PositiveY,
        NegativeY,
        PositiveZ,
        NegativeZ
    };

    bool isNull() const;
    bool hasAlphaChannel() const;

    QSize size() const;
    void setSize(const QSize& value);
    QSize requestedSize() const;

    QImage image(QGLTextureCube::Face face) const;
    void setImage(QGLTextureCube::Face face, const QImage& image);
    void clearImage(QGLTextureCube::Face face);

    void copyImage(QGLTextureCube::Face face, const QImage& image, const QPoint& offset = QPoint(0, 0));

    QGLContext::BindOptions bindOptions() const;
    void setBindOptions(QGLContext::BindOptions options);

    QGL::TextureWrap horizontalWrap() const;
    void setHorizontalWrap(QGL::TextureWrap value);

    QGL::TextureWrap verticalWrap() const;
    void setVerticalWrap(QGL::TextureWrap value);

    bool bind() const;
    static void release();

    GLuint textureId() const;

    static QGLTextureCube *fromTextureId(GLuint id, const QSize& size);

private:
    QScopedPointer<QGLTextureCubePrivate> d_ptr;

    Q_DISABLE_COPY(QGLTextureCube)
    Q_DECLARE_PRIVATE(QGLTextureCube)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
