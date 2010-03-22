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

#ifndef QGLTEXTUREUTILS_P_H
#define QGLTEXTUREUTILS_P_H

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

#include <QtOpenGL/qgl.h>
#include <QtOpenGL/private/qgl_p.h>
#include <QtCore/qdatetime.h>
#include "qglnamespace.h"

QT_BEGIN_NAMESPACE

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif
#ifndef GL_UNSIGNED_SHORT_5_6_5
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#endif
#ifndef GL_UNSIGNED_INT_8_8_8_8_REV
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#endif
#ifndef GL_TEXTURE_CUBE_MAP
#define GL_TEXTURE_CUBE_MAP 0x8513
#endif
#ifndef GL_TEXTURE_CUBE_MAP_POSITIVE_X
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#endif
#ifndef GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#endif

#ifndef GL_GENERATE_MIPMAP_SGIS
#define GL_GENERATE_MIPMAP_SGIS       0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS  0x8192
#endif

#if !defined(QT_OPENGL_ES)
#define q_glTexParameteri(target,name,value) \
        glTexParameteri((target), (name), int(value))
#else
#define q_glTexParameteri(target,name,value) \
        glTexParameterf((target), (name), GLfloat(int(value)))
#endif

// returns the highest number closest to v, which is a power of 2
// NB! assumes 32 bit ints
int qt_gl_next_power_of_two(int v);

// Modify a wrapping mode to account for platform differences.
QGL::TextureWrap qt_gl_modify_texture_wrap(QGL::TextureWrap value);

typedef void (APIENTRY *q_glCompressedTexImage2DARB)
    (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);

class QGLTextureExtensions
{
public:
    QGLTextureExtensions(const QGLContext *ctx);
    ~QGLTextureExtensions();

    int npotTextures : 1;
    int generateMipmap : 1;
    int bgraTextureFormat : 1;
    int ddsTextureCompression : 1;
    int etc1TextureCompression : 1;
    int pvrtcTextureCompression : 1;
    q_glCompressedTexImage2DARB compressedTexImage2D;

    static QGLTextureExtensions *extensions();
};

class QGLBoundTexture
{
public:
    QGLBoundTexture(const QGLContext *ctx);
    ~QGLBoundTexture();

    const QGLContext *context() const { return m_guard.context(); }

    GLuint textureId() const { return m_guard.id(); }
    void setTextureId(GLuint id) { m_guard.setId(id); }

    QGLContext::BindOptions options() const { return m_options; }
    void setOptions(QGLContext::BindOptions options) { m_options = options; }

    QSize size() const { return m_size; }
    bool hasAlpha() const { return m_hasAlpha; }

    void startUpload(GLenum target, const QSize &imageSize);
    void uploadFace(GLenum target, const QImage &image, const QSize &scaleSize,
                    GLenum format = GL_RGBA);
    void createFace(GLenum target, const QSize &size, GLenum format = GL_RGBA);
    void finishUpload(GLenum target);

    static bool canBindCompressedTexture
        (const char *buf, int len, const char *format, bool *hasAlpha,
         bool *isFlipped);
    bool bindCompressedTexture
        (const QString& fileName, const char *format = 0);
    bool bindCompressedTexture
        (const char *buf, int len, const char *format = 0);
    bool bindCompressedTextureDDS(const char *buf, int len);
    bool bindCompressedTexturePVR(const char *buf, int len);

private:
    QGLSharedResourceGuard m_guard;
    QGLContext::BindOptions m_options;
    QSize m_size;
    bool m_hasAlpha;
    QTime time;
};

QT_END_NAMESPACE

#endif
