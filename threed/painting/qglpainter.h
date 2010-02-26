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
#include "qglvertexarray.h"
#include "qglindexarray.h"
#include "qgllightmodel.h"
#include "qgllightparameters.h"
#include "qglmaterialparameters.h"
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

    QRect scissorRect() const;
    void setScissorRect(const QRect& rect);
    void resetScissorRect();

    QGLMatrixStack& projectionMatrix();
    QGLMatrixStack& modelViewMatrix();
    QMatrix4x4 combinedMatrix() const;

    bool isVisible(const QVector3D& point) const;
    bool isVisible(const QBox3D& box) const;

    qreal aspectRatio() const;
    qreal aspectRatio(const QSize& viewportSize) const;

    QGLAbstractEffect *effect() const;

    QGLAbstractEffect *userEffect() const;
    void setUserEffect(QGLAbstractEffect *effect);

    QGL::StandardEffect standardEffect() const;
    void setStandardEffect(QGL::StandardEffect effect);

    void disableEffect();

    QColor color() const;
    void setColor(const QColor& color);

    void setVertexAttribute
        (QGL::VertexAttribute attribute, const QGLAttributeValue& value);
    void setVertexBuffer(const QGLVertexBuffer& buffer);

    void setVertexArray(const QGLVertexArray& array);

    void setCommonNormal(const QVector3D& value);

    int textureUnitCount() const;
    bool isTextureUnitActive(int unit = 0) const;

    void setTexture(int unit, const QGLTexture2D *texture);
    void setTexture(int unit, const QGLTextureCube *texture);
    void setTexture(const QGLTexture2D *texture) { setTexture(0, texture); }
    void setTexture(const QGLTextureCube *texture) { setTexture(0, texture); }

    void update();

    void draw(QGL::DrawingMode mode, int count, int index = 0);
    void draw(QGL::DrawingMode mode, const QGLIndexArray& indices);
    void draw(QGL::DrawingMode mode, const QGLIndexArray& indices, int offset, int count);

    void setPointSize(qreal size);
    void setLineWidth(qreal width);

    void setCullFaces(QGL::CullFaces faces);

    const QGLLightModel *lightModel() const;
    void setLightModel(const QGLLightModel *value);

    int lightCount() const;
    bool hasEnabledLights() const;
    bool isLightEnabled(int number) const;
    void setLightEnabled(int number, bool value) const;

    const QGLLightParameters *lightParameters(int number) const;
    QMatrix4x4 lightTransform(int number) const;
    void setLightParameters(int number, const QGLLightParameters *parameters);
    void setLightParameters(int number, const QGLLightParameters *parameters,
                            const QMatrix4x4& transform);

    const QGLMaterialParameters *faceMaterial(QGL::Face face) const;
    void setFaceMaterial(QGL::Face face, const QGLMaterialParameters *value);
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
