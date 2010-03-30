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
#include "qglfogparameters.h"
#include "qglmatrixstack.h"
#include "qglcamera.h"
#include "qvectorarray.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLAbstractEffect;
class QGLPainterPrivate;
class QGLStencilBufferOptions;
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

    enum Update
    {
        UpdateColor                 = 0x00000001,
        UpdateModelViewMatrix       = 0x00000002,
        UpdateProjectionMatrix      = 0x00000004,
        UpdateMatrices              = 0x00000006,
        UpdateLights                = 0x00000008,
        UpdateMaterials             = 0x00000010,
        UpdateFog                   = 0x00000020,
        UpdateAll                   = 0x7FFFFFFF
    };
    Q_DECLARE_FLAGS(Updates, Update);

    void clear(QGL::ClearBuffers buffers = QGL::ClearColorBuffer | QGL::ClearDepthBuffer);
    void setClearColor(const QColor& color);
    void setClearDepth(qreal depth);
    void setClearStencil(GLint value);

    void setDepthTestingEnabled(bool value);

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

    QGLMatrixStack& projectionMatrix();
    QGLMatrixStack& modelViewMatrix();
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

    const QGLFogParameters *fogParameters() const;
    void setFogParameters(const QGLFogParameters *value);

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

    friend class QGLStencilBufferOptions;
    friend class QGLBlendOptions;
    friend class QGLAbstractEffect;

#ifndef QT_NO_DEBUG
    void checkRequiredFields();
#endif
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLPainter::Updates)

QT_END_NAMESPACE

QT_END_HEADER

#endif
