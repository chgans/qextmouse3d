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

#include "qglext.h"
#include <QtOpenGL/private/qgl_p.h>

QT_BEGIN_NAMESPACE

#if !defined(QT_OPENGL_ES)

typedef void (APIENTRY *q_PFNGLACTIVETEXTUREPROC) (GLenum);
typedef void (APIENTRY *q_PFNGLCLIENTACTIVETEXTUREPROC) (GLenum);

class QGLMultiTextureExtensions
{
public:
    QGLMultiTextureExtensions(const QGLContext * = 0)
    {
        activeTexture = 0;
        clientActiveTexture = 0;
        multiTextureResolved = false;
    }

    q_PFNGLACTIVETEXTUREPROC activeTexture;
    q_PFNGLCLIENTACTIVETEXTUREPROC clientActiveTexture;
    bool multiTextureResolved;
};

#if QT_VERSION >= 0x040800
Q_GLOBAL_STATIC(QGLContextGroupResource<QGLMultiTextureExtensions>, qt_multitexture_funcs)
#else
static void qt_multitexture_funcs_free(void *data)
{
    delete reinterpret_cast<QGLMultiTextureExtensions *>(data);
}

Q_GLOBAL_STATIC_WITH_ARGS(QGLContextResource, qt_multitexture_funcs, (qt_multitexture_funcs_free))
#endif

static QGLMultiTextureExtensions *resolveMultiTextureExtensions
    (const QGLContext *ctx)
{
    QGLMultiTextureExtensions *extn =
        reinterpret_cast<QGLMultiTextureExtensions *>
            (qt_multitexture_funcs()->value(ctx));
    if (!extn) {
        extn = new QGLMultiTextureExtensions();
        qt_multitexture_funcs()->insert(ctx, extn);
    }
    if (!(extn->multiTextureResolved)) {
        extn->multiTextureResolved = true;
        if (!extn->activeTexture) {
            extn->activeTexture = (q_PFNGLACTIVETEXTUREPROC)
                ctx->getProcAddress(QLatin1String("glActiveTexture"));
        }
        if (!extn->activeTexture) {
            extn->activeTexture = (q_PFNGLACTIVETEXTUREPROC)
                ctx->getProcAddress(QLatin1String("glActiveTextureARB"));
        }
        if (!extn->clientActiveTexture) {
            extn->clientActiveTexture = (q_PFNGLCLIENTACTIVETEXTUREPROC)
                ctx->getProcAddress(QLatin1String("glClientActiveTexture"));
        }
        if (!extn->clientActiveTexture) {
            extn->clientActiveTexture = (q_PFNGLCLIENTACTIVETEXTUREPROC)
                ctx->getProcAddress(QLatin1String("glClientActiveTextureARB"));
        }
    }
    return extn;
}

void qt_gl_ClientActiveTexture(GLenum texture)
{
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return;
    QGLMultiTextureExtensions *extn = resolveMultiTextureExtensions(ctx);
    if (extn->clientActiveTexture)
        extn->clientActiveTexture(texture);
}

void qt_gl_ActiveTexture(GLenum texture)
{
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return;
    QGLMultiTextureExtensions *extn = resolveMultiTextureExtensions(ctx);
    if (extn->activeTexture)
        extn->activeTexture(texture);
}

#endif

#if !defined(QT_OPENGL_ES_2)

typedef void (APIENTRY *q_PFNGLBLENDCOLORPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef void (APIENTRY *q_PFNGLBLENDEQUATIONPROC) (GLenum mode);
typedef void (APIENTRY *q_PFNGLBLENDFUNCSEPARATEPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void (APIENTRY *q_PFNGLBLENDEQUATIONSEPARATEPROC) (GLenum modeRGB, GLenum modeAlpha);

class QGLBlendExtensions
{
public:
    QGLBlendExtensions(const QGLContext * = 0)
    {
        blendColor = 0;
        blendFuncSeparate = 0;
        blendEquation = 0;
        blendEquationSeparate = 0;
        blendResolved = false;
    }

    q_PFNGLBLENDCOLORPROC blendColor;
    q_PFNGLBLENDFUNCSEPARATEPROC blendFuncSeparate;
    q_PFNGLBLENDEQUATIONPROC blendEquation;
    q_PFNGLBLENDEQUATIONSEPARATEPROC blendEquationSeparate;
    bool blendResolved;
};

#if QT_VERSION >= 0x040800
Q_GLOBAL_STATIC(QGLContextGroupResource<QGLBlendExtensions>, qt_blend_funcs)
#else
static void qt_blend_funcs_free(void *data)
{
    delete reinterpret_cast<QGLBlendExtensions *>(data);
}

Q_GLOBAL_STATIC_WITH_ARGS(QGLContextResource, qt_blend_funcs, (qt_blend_funcs_free))
#endif

static QGLBlendExtensions *resolveBlendExtensions(const QGLContext *ctx)
{
    QGLBlendExtensions *extn =
        reinterpret_cast<QGLBlendExtensions *>(qt_blend_funcs()->value(ctx));
    if (!extn) {
        extn = new QGLBlendExtensions();
        qt_blend_funcs()->insert(ctx, extn);
    }
    if (!(extn->blendResolved)) {
        extn->blendResolved = true;
        if (!extn->blendColor) {
            extn->blendColor = (q_PFNGLBLENDCOLORPROC)
                ctx->getProcAddress(QLatin1String("glBlendColorEXT"));
        }
        if (!extn->blendColor) {
            extn->blendColor = (q_PFNGLBLENDCOLORPROC)
                ctx->getProcAddress(QLatin1String("glBlendColorOES"));
        }
        if (!extn->blendColor) {
            extn->blendColor = (q_PFNGLBLENDCOLORPROC)
                ctx->getProcAddress(QLatin1String("glBlendColor"));
        }
        if (!extn->blendFuncSeparate) {
            extn->blendFuncSeparate = (q_PFNGLBLENDFUNCSEPARATEPROC)
                ctx->getProcAddress(QLatin1String("glBlendFuncSeparateEXT"));
        }
        if (!extn->blendFuncSeparate) {
            extn->blendFuncSeparate = (q_PFNGLBLENDFUNCSEPARATEPROC)
                ctx->getProcAddress(QLatin1String("glBlendFuncSeparateOES"));
        }
        if (!extn->blendFuncSeparate) {
            extn->blendFuncSeparate = (q_PFNGLBLENDFUNCSEPARATEPROC)
                ctx->getProcAddress(QLatin1String("glBlendFuncSeparate"));
        }
        if (!extn->blendEquation) {
            extn->blendEquation = (q_PFNGLBLENDEQUATIONPROC)
                ctx->getProcAddress(QLatin1String("glBlendEquationEXT"));
        }
        if (!extn->blendEquation) {
            extn->blendEquation = (q_PFNGLBLENDEQUATIONPROC)
                ctx->getProcAddress(QLatin1String("glBlendEquationOES"));
        }
        if (!extn->blendEquation) {
            extn->blendEquation = (q_PFNGLBLENDEQUATIONPROC)
                ctx->getProcAddress(QLatin1String("glBlendEquation"));
        }
        if (!extn->blendEquationSeparate) {
            extn->blendEquationSeparate = (q_PFNGLBLENDEQUATIONSEPARATEPROC)
                ctx->getProcAddress(QLatin1String("glBlendEquationSeparateEXT"));
        }
        if (!extn->blendEquationSeparate) {
            extn->blendEquationSeparate = (q_PFNGLBLENDEQUATIONSEPARATEPROC)
                ctx->getProcAddress(QLatin1String("glBlendEquationSeparateOES"));
        }
        if (!extn->blendEquationSeparate) {
            extn->blendEquationSeparate = (q_PFNGLBLENDEQUATIONSEPARATEPROC)
                ctx->getProcAddress(QLatin1String("glBlendEquationSeparate"));
        }
    }
    return extn;
}

void qt_gl_BlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return;
    QGLBlendExtensions *extn = resolveBlendExtensions(ctx);
    if (extn->blendColor)
        extn->blendColor(red, green, blue, alpha);
}

void qt_gl_BlendEquation(GLenum mode)
{
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return;
    QGLBlendExtensions *extn = resolveBlendExtensions(ctx);
    if (extn->blendEquation)
        extn->blendEquation(mode);
}

void qt_gl_BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return;
    QGLBlendExtensions *extn = resolveBlendExtensions(ctx);
    if (extn->blendEquationSeparate)
        extn->blendEquationSeparate(modeRGB, modeAlpha);
    else if (extn->blendEquation)
        extn->blendEquation(modeRGB);   // Do the best we can.
}

void qt_gl_BlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return;
    QGLBlendExtensions *extn = resolveBlendExtensions(ctx);
    if (extn->blendFuncSeparate)
        extn->blendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    else
        glBlendFunc(srcRGB, dstRGB);    // Do the best we can.
}

#endif

QT_END_NAMESPACE
