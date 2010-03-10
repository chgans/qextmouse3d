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

#ifndef QGLPAINTER_P_H
#define QGLPAINTER_P_H

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

#include "qglpainter.h"
#include <QtCore/qatomic.h>
#include <QtCore/qbitarray.h>
#include <QtCore/qmap.h>
#include <QtGui/private/qpaintengineex_p.h>
#include <QtOpenGL/private/qglextensions_p.h>

QT_BEGIN_NAMESPACE

#ifndef Q_WS_MAC
# ifndef APIENTRYP
#   ifdef APIENTRY
#     define APIENTRYP APIENTRY *
#   else
#     define APIENTRY
#     define APIENTRYP *
#   endif
# endif
#else
# define APIENTRY
# define APIENTRYP *
#endif

typedef ptrdiff_t qGLsizeiptr;
typedef ptrdiff_t qGLintptr;

typedef void (APIENTRYP q_PFNGLSTENCILFUNCSEPARATEPROC) (GLenum face, GLenum func, GLint ref, GLuint mask);
typedef void (APIENTRYP q_PFNGLSTENCILMASKSEPARATEPROC) (GLenum face, GLuint mask);
typedef void (APIENTRYP q_PFNGLSTENCILOPSEPARATEPROC) (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);

typedef void (APIENTRYP q_PFNGLBLENDCOLORPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef void (APIENTRYP q_PFNGLBLENDEQUATIONPROC) (GLenum mode);
typedef void (APIENTRYP q_PFNGLBLENDFUNCSEPARATEPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void (APIENTRYP q_PFNGLBLENDEQUATIONSEPARATEPROC) (GLenum modeRGB, GLenum modeAlpha);

typedef void (APIENTRYP q_PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRYP q_PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRYP q_PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRYP q_PFNGLBUFFERDATAPROC) (GLenum target, qGLsizeiptr size, const GLvoid *data, GLenum usage);
typedef void (APIENTRYP q_PFNGLBUFFERSUBDATAPROC) (GLenum target, qGLintptr offset, qGLsizeiptr size, const GLvoid *data);
typedef void (APIENTRYP q_PFNGLGETBUFFERSUBDATAPROC) (GLenum target, qGLintptr offset, qGLsizeiptr size, GLvoid *data);
typedef void (APIENTRYP q_PFNGLGETBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef GLvoid* (APIENTRYP q_PFNGLMAPBUFFERPROC) (GLenum target, GLenum access);
typedef GLboolean (APIENTRYP q_PFNGLUNMAPBUFFERPROC) (GLenum target);

typedef void (APIENTRY *_glActiveTexture) (GLenum);
typedef void (APIENTRY *_glClientActiveTexture) (GLenum);

typedef void (APIENTRY *q_glVertexAttribPointer) (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);

// We can call the buffer functions directly in OpenGL/ES 1.1 or higher,
// but all other platforms need to resolve the extensions.
#if defined(QT_OPENGL_ES)
#if defined(GL_OES_VERSION_1_0) && !defined(GL_OES_VERSION_1_1)
#define QGL_RESOLVE_BUFFER_FUNCS 1
#endif
#else
#define QGL_RESOLVE_BUFFER_FUNCS 1
#endif

class QGLPainterExtensions
{
public:
    QGLPainterExtensions()
    {
#if !defined(QT_OPENGL_ES)
        stencilFuncSeparate = 0;
        stencilMaskSeparate = 0;
        stencilOpSeparate = 0;
        stencilResolved = false;

        blendColor = 0;
        blendFuncSeparate = 0;
        blendEquation = 0;
        blendEquationSeparate = 0;
        blendResolved = false;
#endif

#if defined(QGL_RESOLVE_BUFFER_FUNCS)
        bindBuffer = 0;
        deleteBuffers = 0;
        genBuffers = 0;
        bufferData = 0;
        bufferSubData = 0;
        getBufferSubData = 0;
        getBufferParameteriv = 0;
#endif
        mapBuffer = 0;
        unmapBuffer = 0;
        qt_glActiveTexture = 0;
        qt_glClientActiveTexture = 0;
        multiTextureResolved = false;

        vertexAttribPointer = 0;
    }

#if !defined(QT_OPENGL_ES)
    q_PFNGLSTENCILFUNCSEPARATEPROC stencilFuncSeparate;
    q_PFNGLSTENCILMASKSEPARATEPROC stencilMaskSeparate;
    q_PFNGLSTENCILOPSEPARATEPROC stencilOpSeparate;
    bool stencilResolved;

    q_PFNGLBLENDCOLORPROC blendColor;
    q_PFNGLBLENDFUNCSEPARATEPROC blendFuncSeparate;
    q_PFNGLBLENDEQUATIONPROC blendEquation;
    q_PFNGLBLENDEQUATIONSEPARATEPROC blendEquationSeparate;
    bool blendResolved;
#endif

#if defined(QGL_RESOLVE_BUFFER_FUNCS)
    q_PFNGLBINDBUFFERPROC bindBuffer;
    q_PFNGLDELETEBUFFERSPROC deleteBuffers;
    q_PFNGLGENBUFFERSPROC genBuffers;
    q_PFNGLBUFFERDATAPROC bufferData;
    q_PFNGLBUFFERSUBDATAPROC bufferSubData;
    q_PFNGLGETBUFFERSUBDATAPROC getBufferSubData;
    q_PFNGLGETBUFFERPARAMETERIVPROC getBufferParameteriv;
#endif
    q_PFNGLMAPBUFFERPROC mapBuffer;
    q_PFNGLUNMAPBUFFERPROC unmapBuffer;
    _glActiveTexture qt_glActiveTexture;
    _glClientActiveTexture qt_glClientActiveTexture;
    bool multiTextureResolved;

    q_glVertexAttribPointer vertexAttribPointer;
};


#define QGL_MAX_LIGHTS      32
#define QGL_MAX_STD_EFFECTS 16

class QGLPainterPickPrivate
{
public:
    QGLPainterPickPrivate();
    ~QGLPainterPickPrivate();

    bool isPicking;
    int objectPickId;
    int pickColorIndex;
    QRgb pickColor;
    QMap<int, QRgb> pickObjectToColor;
    QMap<QRgb, int> pickColorToObject;
    QGLAbstractEffect *defaultPickEffect;
};

struct QGLShaderExtensionFuncs;

class QGLPainterPrivate
{
public:
    QGLPainterPrivate();
    ~QGLPainterPrivate();

    QAtomicInt ref;
    const QGLContext *context;
    QPaintEngineEx *activePaintEngine;
    QGLMatrixStack projectionMatrix;
    QGLMatrixStack modelViewMatrix;
    QGLAbstractEffect *effect;
    QGLAbstractEffect *userEffect;
    QGL::StandardEffect standardEffect;
    QGLAbstractEffect *stdeffects[QGL_MAX_STD_EFFECTS];
#ifdef Q_WS_WIN
    QGLPainterExtensions *extensionFuncs;
    QGLShaderExtensionFuncs *shaderExtensionFuncs;
#endif
    int textureUnitCount;
    QBitArray texturesInUse;
    const QGLLightModel *lightModel;
    QGLLightModel *defaultLightModel;
    const QGLLightParameters *lights[QGL_MAX_LIGHTS];
    QMatrix4x4 lightTransforms[QGL_MAX_LIGHTS];
    QGLLightParameters *defaultLight0;
    QGLLightParameters *defaultLight1;
    int enabledLights;
    int maxLights;
    const QGLMaterialParameters *frontMaterial;
    const QGLMaterialParameters *backMaterial;
    QGLMaterialParameters *defaultMaterial;
    QGLMaterialParameters *frontColorMaterial;
    QGLMaterialParameters *backColorMaterial;
    const QGLFogParameters *fogParameters;
    QBox3D viewingCube;
    QRect scissorRect;
    QColor color;
    QGLPainter::Updates updates;
    GLuint currentBufferId;
    QGLPainterPickPrivate *pick;
    QMap<QString, QGLShaderProgram *> cachedPrograms;

    QGLPainterExtensions *extensions();    

    QGLPainterExtensions *resolveMultiTextureExtensions()
    {
        QGLPainterExtensions *extn = extensions();
        if (!(extn->multiTextureResolved)) {
            extn->multiTextureResolved = true;
            if (!extn->qt_glActiveTexture) {
                extn->qt_glActiveTexture = (_glActiveTexture)
                    this->context->getProcAddress
                        (QLatin1String("glActiveTexture"));
            }
            if (!extn->qt_glActiveTexture) {
                extn->qt_glActiveTexture = (_glActiveTexture)
                    this->context->getProcAddress
                        (QLatin1String("glActiveTextureARB"));
            }
            if (!extn->qt_glClientActiveTexture) {
                extn->qt_glClientActiveTexture = (_glClientActiveTexture)
                    this->context->getProcAddress
                        (QLatin1String("glClientActiveTexture"));
            }
            if (!extn->qt_glClientActiveTexture) {
                extn->qt_glClientActiveTexture = (_glClientActiveTexture)
                    this->context->getProcAddress
                        (QLatin1String("glClientActiveTextureARB"));
            }
        }
        return extn;
    }

    inline void ensureEffect(QGLPainter *painter)
        { if (!effect) createEffect(painter); }
    void createEffect(QGLPainter *painter);

#ifndef QT_NO_DEBUG
    // Required field checking is only done in debug builds.
    QList<QGL::VertexAttribute> requiredFields;
    inline void setRequiredFields(const QList<QGL::VertexAttribute>& fields)
        { requiredFields = fields; }
    void removeRequiredFields(const QList<QGL::VertexAttribute>& array);
    void removeRequiredField(QGL::VertexAttribute attribute)
        { requiredFields.removeAll(attribute); }
#else
    inline void setRequiredFields(const QList<QGL::VertexAttribute>& fields)
        { Q_UNUSED(fields); }
    inline void removeRequiredFields(const QList<QGL::VertexAttribute>& array)
        { Q_UNUSED(array); }
    inline void removeRequiredField(QGL::VertexAttribute attribute)
        { Q_UNUSED(attribute); }
#endif
};

class QGLPainterPrivateCache : public QObject
{
    Q_OBJECT
public:
    QGLPainterPrivateCache();
    ~QGLPainterPrivateCache();

    QMap<const QGLContext *, QGLPainterPrivate *> cache;

    QGLPainterPrivate *fromContext(const QGLContext *context);

    static QGLPainterPrivateCache *instance();

public slots:
    void contextDestroyed(const QGLContext *context);

signals:
    void destroyedContext(const QGLContext *context);
};

QT_END_NAMESPACE

#endif
