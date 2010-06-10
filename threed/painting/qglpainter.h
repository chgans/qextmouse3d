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

#ifndef QGLPAINTER_H
#define QGLPAINTER_H

#include <QtOpenGL/qgl.h>
#include "qglnamespace.h"
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>
#include <QtGui/qmatrix4x4.h>
#include "qbox3d.h"
#include "qglvertexbuffer.h"
#include "qglindexbuffer.h"
#include "qgllightmodel.h"
#include "qgllightparameters.h"
#include "qglmaterial.h"
#include "qmatrix4x4stack.h"
#include "qglcamera.h"
#include "qvectorarray.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLAbstractEffect;
class QGLPainterPrivate;
class QGLBlendOptions;
class QGLTexture2D;
class QGLTextureCube;
class QGeometryData;
class QGLShaderProgram;
class QGLFramebufferObject;

class Q_QT3D_EXPORT QGLPainter
{
public:
    QGLPainter();
    explicit QGLPainter(const QGLContext *context);
    explicit QGLPainter(QPaintDevice *device);
    explicit QGLPainter(QPainter *painter);
    virtual ~QGLPainter();

    bool begin();
    bool begin(const QGLContext *context);
    bool begin(QPaintDevice *device);
    bool begin(QPainter *painter);
    bool end();
    bool isActive() const;

    const QGLContext *context() const;

    bool isFixedFunction() const;

    enum Update
    {
        UpdateColor                 = 0x00000001,
        UpdateModelViewMatrix       = 0x00000002,
        UpdateProjectionMatrix      = 0x00000004,
        UpdateMatrices              = 0x00000006,
        UpdateLights                = 0x00000008,
        UpdateMaterials             = 0x00000010,
        UpdateAll                   = 0x7FFFFFFF
    };
    Q_DECLARE_FLAGS(Updates, Update);

    void clear(QGL::ClearBuffers buffers = QGL::ClearColorBuffer | QGL::ClearDepthBuffer);
    void setClearColor(const QColor& color);
    void setClearDepth(qreal depth);
    void setClearStencil(GLint value);

    void setDepthTestingEnabled(bool value);
    void setStencilTestingEnabled(bool value);
    void setBlendingEnabled(bool value);

    QRect viewport() const;
    void setViewport(const QRect& rect);
    void setViewport(const QSize& size);
    void setViewport(int width, int height);
    void resetViewport();

    QRect scissor() const;
    void setScissor(const QRect& rect);
    void intersectScissor(const QRect& rect);
    void expandScissor(const QRect& rect);
    void resetScissor();

    QMatrix4x4Stack& projectionMatrix();
    QMatrix4x4Stack& modelViewMatrix();
    QMatrix4x4 combinedMatrix() const;
    QMatrix3x3 normalMatrix() const;

    QGL::Eye eye() const;
    void setEye(QGL::Eye eye);

    void setCamera(QGLCamera *camera);

    bool isCullable(const QVector3D& point) const;
    bool isCullable(const QBox3D& box) const;

    qreal aspectRatio() const;

    QGLAbstractEffect *effect() const;

    QGLAbstractEffect *userEffect() const;
    void setUserEffect(QGLAbstractEffect *effect);

    QGL::StandardEffect standardEffect() const;
    void setStandardEffect(QGL::StandardEffect effect);

    void disableEffect();

    QGLShaderProgram *cachedProgram(const QString& name) const;
    void setCachedProgram(const QString& name, QGLShaderProgram *program);

    QColor color() const;
    void setColor(const QColor& color);

    void setVertexAttribute
        (QGL::VertexAttribute attribute, const QGLAttributeValue& value);
    void setVertexBuffer(const QGLVertexBuffer& buffer);

    void setCommonNormal(const QVector3D& value);

    int textureUnitCount() const;

    void setTexture(int unit, const QGLTexture2D *texture);
    void setTexture(int unit, const QGLTextureCube *texture);
    void setTexture(const QGLTexture2D *texture) { setTexture(0, texture); }
    void setTexture(const QGLTextureCube *texture) { setTexture(0, texture); }

    void update();
    void updateFixedFunction(QGLPainter::Updates updates);

    void draw(QGL::DrawingMode mode, int count, int index = 0);
    void draw(QGL::DrawingMode mode, const ushort *indices, int count);
    void draw(QGL::DrawingMode mode, const QGLIndexBuffer& indices);
    void draw(QGL::DrawingMode mode, const QGLIndexBuffer& indices, int offset, int count);

    void pushSurface(QGLFramebufferObject *fbo);
    QGLFramebufferObject *popSurface();
    QGLFramebufferObject *currentSurface() const;
    QSize surfaceSize() const;

    void setPointSize(qreal size);
    void setLineWidth(qreal width);

    void setCullFaces(QGL::CullFaces faces);

    const QGLLightModel *lightModel() const;
    void setLightModel(const QGLLightModel *value);

    const QGLLightParameters *mainLight() const;
    void setMainLight(QGLLightParameters *parameters);
    void setMainLight
        (QGLLightParameters *parameters, const QMatrix4x4& transform);
    QMatrix4x4 mainLightTransform() const;

    const QGLMaterial *faceMaterial(QGL::Face face) const;
    void setFaceMaterial(QGL::Face face, const QGLMaterial *value);
    void setFaceColor(QGL::Face face, const QColor& color);

    bool isPicking() const;
    void setPicking(bool value);

    int objectPickId() const;
    void setObjectPickId(int value);
    void clearPickObjects();

    QColor pickColor() const;

    int pickObject(int x, int y) const;

private:
    Q_DISABLE_COPY(QGLPainter)

    QGLPainterPrivate *d_ptr;

    QGLPainterPrivate *d_func() const { return d_ptr; }

    friend class QGLBlendOptions;
    friend class QGLAbstractEffect;

#ifndef QT_NO_DEBUG
    void checkRequiredFields();
#endif
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLPainter::Updates)

inline void QGLPainter::setDepthTestingEnabled(bool value)
{
    if (value)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

inline void QGLPainter::setStencilTestingEnabled(bool value)
{
    if (value)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);
}

inline void QGLPainter::setBlendingEnabled(bool value)
{
    if (value)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}

// Provide some useful OpenGL extension definitions.
#if !defined(QT_OPENGL_ES)

extern void Q_QT3D_EXPORT qt_gl_ClientActiveTexture(GLenum texture);
extern void Q_QT3D_EXPORT qt_gl_ActiveTexture(GLenum texture);

#elif defined(QT_OPENGL_ES_2)

#define qt_gl_ActiveTexture         glActiveTexture

#else

#define qt_gl_ClientActiveTexture   glClientActiveTexture
#define qt_gl_ActiveTexture         glActiveTexture

#endif

#ifndef GL_TEXTURE0
#define GL_TEXTURE0 0x84C0
#endif
#ifndef GL_TEXTURE1
#define GL_TEXTURE1 0x84C1
#endif
#ifndef GL_TEXTURE2
#define GL_TEXTURE2 0x84C2
#endif
#ifndef GL_TEXTURE3
#define GL_TEXTURE3 0x84C3
#endif
#ifndef GL_TEXTURE4
#define GL_TEXTURE4 0x84C4
#endif
#ifndef GL_TEXTURE5
#define GL_TEXTURE5 0x84C5
#endif
#ifndef GL_TEXTURE6
#define GL_TEXTURE6 0x84C6
#endif
#ifndef GL_TEXTURE7
#define GL_TEXTURE7 0x84C7
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
