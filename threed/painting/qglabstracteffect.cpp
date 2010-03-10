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

#include "qglabstracteffect.h"
#include "qglpainter_p.h"
#include <QtOpenGL/qglshaderprogram.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLAbstractEffect
    \since 4.7
    \brief The QGLAbstractEffect class provides a standard interface for rendering surface material effects with GL.
    \ingroup qt3d
    \ingroup qt3d::painting
*/

/*!
    Constructs a new effect object.
*/
QGLAbstractEffect::QGLAbstractEffect()
{
}

/*!
    Destroys this effect object.
*/
QGLAbstractEffect::~QGLAbstractEffect()
{
}

/*!
    \fn QList<QGL::VertexAttribute> QGLAbstractEffect::requiredFields() const

    Returns a list of the vertex array fields that are required
    to be provided via setVertexAttribute().  QGLPainter::draw()
    uses this to warn when the application has not supplied
    sufficient arguments to use the effect.

    \sa setVertexAttribute(), QGLPainter::draw()
*/

/*!
    Returns true if this effect supports object picking; false otherwise.
    The default implementation returns false, which causes QGLPainter
    to use the effect associated with QGL::FlatColor to perform
    object picking.

    Effects that support object picking render fragments with
    QGLPainter::pickColor() when QGLPainter::isPicking() returns true.
    By default, only the effect associated with QGL::FlatColor does this,
    rendering the entire fragment with the flat pick color.

    In some cases, rendering the entire fragment with the pick color
    may not be appropriate.  An alpha-blended icon texture that is
    drawn to the screen as a quad may have an irregular shape smaller
    than the quad.  For picking, the application may not want the
    entire quad to be "active" for object selection as it would appear
    to allow the user to click off the icon to select it.

    This situation can be handled by implementing an icon rendering
    effect that draws the icon normally when QGLPainter::isPicking()
    is false, and draws a mask texture defining the outline of the icon
    with QGLPainter::pickColor() when QGLPainter::isPicking() is true.

    \sa QGLPainter::setPicking()
*/
bool QGLAbstractEffect::supportsPicking() const
{
    return false;
}

/*!
    \fn void QGLAbstractEffect::setActive(QGLPainter *painter, bool flag)

    Activates or deactivates this effect on \a painter,
    according to \a flag, on the current GL context by selecting
    shader programs, setting lighting and material parameters, etc.

    \sa update()
*/

/*!
    Updates the current GL context with information from \a painter
    just prior to the drawing of triangles, quads, etc.

    The \a updates parameter specifies the properties on \a painter
    that have changed since the last call to update() or setActive().

    \sa setActive()
*/
void QGLAbstractEffect::update(QGLPainter *painter, QGLPainter::Updates updates)
{
    Q_UNUSED(painter);
    Q_UNUSED(updates);
}

/*!
    \fn void QGLAbstractEffect::setVertexAttribute(QGL::VertexAttribute attribute, const QGLAttributeValue& value)

    Sets the specified vertex \a attribute to \a value.

    The default implementation sets the attribute on the GL fixed
    function pipeline.  For example, the attribute QGL::Normal will
    be set using \c{glNormalPointer()}.

    OpenGL/ES 2.0 implementations do not have a fixed function
    pipeline, so subclasses must handle all relevant attributes
    themselves on that platform.
*/
// Implemented in qglpainter.cpp.

/*!
    Sets the common normal value for all vertices to \a value and
    disable any active normal arrays.

    The default implementation calls \c{glNormal3f()} to set \a value
    on the GL fixed function pipeline.

    OpenGL/ES 2.0 implementations do not have a fixed function
    pipeline, so subclasses must override this function and
    use QGLShaderProgram::setAttributeValue() to set the normal
    attribute on the shader program to \a value.
*/
void QGLAbstractEffect::setCommonNormal(const QVector3D& value)
{
#if !defined(QT_OPENGL_ES_2)
    glNormal3f(value.x(), value.y(), value.z());
    glDisableClientState(GL_NORMAL_ARRAY);
#else
    Q_UNUSED(value)
#endif
}

#if !defined(QT_OPENGL_ES_2) || defined(Q_QDOC)

static void setLight(int light, const QGLLightParameters *parameters,
                     const QMatrix4x4& transform)
{
    GLfloat params[4];

    QColor color = parameters->ambientColor();
    params[0] = color.redF();
    params[1] = color.greenF();
    params[2] = color.blueF();
    params[3] = color.alphaF();
    glLightfv(light, GL_AMBIENT, params);

    color = parameters->diffuseColor();
    params[0] = color.redF();
    params[1] = color.greenF();
    params[2] = color.blueF();
    params[3] = color.alphaF();
    glLightfv(light, GL_DIFFUSE, params);

    color = parameters->specularColor();
    params[0] = color.redF();
    params[1] = color.greenF();
    params[2] = color.blueF();
    params[3] = color.alphaF();
    glLightfv(light, GL_SPECULAR, params);

    QVector4D vector = parameters->eyePosition(transform);
    params[0] = vector.x();
    params[1] = vector.y();
    params[2] = vector.z();
    params[3] = vector.w();
    glLightfv(light, GL_POSITION, params);

    QVector3D spotDirection = parameters->eyeSpotDirection(transform);
    params[0] = spotDirection.x();
    params[1] = spotDirection.y();
    params[2] = spotDirection.z();
    glLightfv(light, GL_SPOT_DIRECTION, params);

    params[0] = parameters->spotExponent();
    glLightfv(light, GL_SPOT_EXPONENT, params);

    params[0] = parameters->spotAngle();
    glLightfv(light, GL_SPOT_CUTOFF, params);

    params[0] = parameters->constantAttenuation();
    glLightfv(light, GL_CONSTANT_ATTENUATION, params);

    params[0] = parameters->linearAttenuation();
    glLightfv(light, GL_LINEAR_ATTENUATION, params);

    params[0] = parameters->quadraticAttenuation();
    glLightfv(light, GL_QUADRATIC_ATTENUATION, params);
}

static void setMaterial(int face, const QGLMaterialParameters *parameters)
{
    GLfloat params[17];

    QColor mcolor = parameters->ambientColor();
    params[0] = mcolor.redF();
    params[1] = mcolor.greenF();
    params[2] = mcolor.blueF();
    params[3] = mcolor.alphaF();

    mcolor = parameters->diffuseColor();
    params[4] = mcolor.redF();
    params[5] = mcolor.greenF();
    params[6] = mcolor.blueF();
    params[7] = mcolor.alphaF();

    mcolor = parameters->specularColor();
    params[8] = mcolor.redF();
    params[9] = mcolor.greenF();
    params[10] = mcolor.blueF();
    params[11] = mcolor.alphaF();

    mcolor = parameters->emittedLight();
    params[12] = mcolor.redF();
    params[13] = mcolor.greenF();
    params[14] = mcolor.blueF();
    params[15] = mcolor.alphaF();

    params[16] = parameters->shininess();

    glMaterialfv(face, GL_AMBIENT, params);
    glMaterialfv(face, GL_DIFFUSE, params + 4);
    glMaterialfv(face, GL_SPECULAR, params + 8);
    glMaterialfv(face, GL_EMISSION, params + 12);
    glMaterialfv(face, GL_SHININESS, params + 16);
}

/*!
    Updates the lighting parameters in the fixed-function pipeline
    on \a painter using \c{glLight()}, \c{glLightModel()}, and
    \c{glMaterial()}.  The \a updates parameter specifies the painter
    properties that have changed since the last call to update().

    This function is provided as a convenience for use by subclasses
    that want to implement lighting using the fixed-function pipeline
    rather than shader programs.

    The subclass is responsible for calling \c{glEnable(GL_LIGHTING)}
    or \c{glDisable(GL_LIGHTING)}, but all other fixed-function lighting
    parameters are managed by this function.

    This function does not exist on OpenGL/ES 2.0 systems.

    \sa update()
*/
void QGLAbstractEffect::updateLighting
        (const QGLPainter *painter, QGLPainter::Updates updates)
{
    // Update the lights if they have changegd.
    bool hasEnabledLights = painter->hasEnabledLights();
    if ((updates & QGLPainter::UpdateLights) != 0) {
        // Save the current modelview matrix and load the identity.
        // We need to apply the light in the modelview transformation
        // that was active when the light was specified.
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Enable the lights.
        int count = painter->lightCount();
        for (int light = 0; light < count; ++light) {
            if (painter->isLightEnabled(light)) {
                setLight(GL_LIGHT0 + light, painter->lightParameters(light),
                         painter->lightTransform(light));
                glEnable(GL_LIGHT0 + light);
            } else {
                glDisable(GL_LIGHT0 + light);
            }
        }

        // Restore the previous modelview transformation.
        if (hasEnabledLights)
            glPopMatrix();

        // Set up the light model parameters if at least one light is enabled.
        if (hasEnabledLights) {
            const QGLLightModel *lightModel = painter->lightModel();
            GLfloat values[4];
#ifdef GL_LIGHT_MODEL_TWO_SIDE
            if (lightModel->model() == QGLLightModel::TwoSided)
                values[0] = 1.0f;
            else
                values[0] = 0.0f;
            glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, values);
#endif
#ifdef GL_LIGHT_MODEL_COLOR_CONTROL
            if (lightModel->colorControl() == QGLLightModel::SeparateSpecularColor)
                values[0] = GL_SEPARATE_SPECULAR_COLOR;
            else
                values[0] = GL_SINGLE_COLOR;
            glLightModelfv(GL_LIGHT_MODEL_COLOR_CONTROL, values);
#endif
#ifdef GL_LIGHT_MODEL_LOCAL_VIEWER
            if (lightModel->viewerPosition() == QGLLightModel::LocalViewer)
                values[0] = 1.0f;
            else
                values[0] = 0.0f;
            glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, values);
#endif
#ifdef GL_LIGHT_MODEL_AMBIENT
            QColor color = lightModel->ambientSceneColor();
            values[0] = color.redF();
            values[1] = color.blueF();
            values[2] = color.greenF();
            values[3] = color.alphaF();
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, values);
#endif
        }
    }

    // Update the materials if they have changed.
    if ((updates & QGLPainter::UpdateMaterials) != 0 && hasEnabledLights) {
        const QGLMaterialParameters *frontMaterial = painter->faceMaterial(QGL::FrontFaces);
        const QGLMaterialParameters *backMaterial = painter->faceMaterial(QGL::BackFaces);
        if (frontMaterial == backMaterial) {
            setMaterial(GL_FRONT_AND_BACK, frontMaterial);
        } else {
            setMaterial(GL_FRONT, frontMaterial);
            setMaterial(GL_BACK, backMaterial);
        }
    }
}

/*!
    Updates the fog parameters in the fixed-function pipeline
    on \a painter using \c{glFog()}.

    This function is provided as a convenience for use by subclasses
    that want to implement fog using the fixed-function pipeline
    rather than shader programs.

    This function does not exist on OpenGL/ES 2.0 systems.

    \sa update()
*/
void QGLAbstractEffect::updateFog(const QGLPainter *painter)
{
    const QGLFogParameters *fog = painter->fogParameters();
    if (!fog) {
        glDisable(GL_FOG);
    } else {
        GLfloat color[4];
        QColor col = fog->color();
        color[0] = col.redF();
        color[1] = col.greenF();
        color[2] = col.blueF();
        color[3] = col.alphaF();
        int fmode;
        QGLFogParameters::Mode mode = fog->mode();
        if (mode == QGLFogParameters::Linear)
            fmode = GL_LINEAR;
        else if (mode == QGLFogParameters::Exponential)
            fmode = GL_EXP;
        else if (mode == QGLFogParameters::Exponential2)
            fmode = GL_EXP2;
        else
            fmode = GL_LINEAR;  // Just in case.
        glFogf(GL_FOG_MODE, fmode);
        glFogf(GL_FOG_DENSITY, fog->density());
        glFogf(GL_FOG_START, fog->nearDistance());
        glFogf(GL_FOG_END, fog->farDistance());
        glFogfv(GL_FOG_COLOR, color);
        glEnable(GL_FOG);
    }
}

/*!
    \fn void QGLAbstractEffect::enableVertexAttribute(QGL::VertexAttribute attribute)

    Enables the vertex array specified by \a attribute on the fixed-function
    pipeline using \c{glEnableClientState()}.

    This function is provided as a convenience for use by subclasses
    that want to implement an effect using the fixed-function pipeline
    rather than shader programs.  It is called from the subclass
    setActive() override.

    This function does not exist on OpenGL/ES 2.0 systems.

    \sa setActive(), setVertexAttribute(), disableVertexAttribute()
*/
// Implemented in qglpainter.cpp.

/*!
    \fn void QGLAbstractEffect::disableVertexAttribute(QGL::VertexAttribute attribute)

    Disables the vertex array specified by \a attribute on the fixed-function
    pipeline using \c{glDisableClientState()}.

    This function is provided as a convenience for use by subclasses
    that want to implement an effect using the fixed-function pipeline
    rather than shader programs.  It is called from the subclass
    setActive() override.

    This function does not exist on OpenGL/ES 2.0 systems.

    \sa setActive(), setVertexAttribute(), enableVertexAttribute()
*/
// Implemented in qglpainter.cpp.

#endif // !QT_OPENGL_ES_2 || Q_QDOC

/*!
    Sets the vertex attribute at \a location on \a program to \a value.
    It is assumed that \a program is bound to the current context.
    Has no effect on systems without shader support.

    This function is provided as a convenience for use by subclasses
    that want to implement an effect using shaders.
*/
void QGLAbstractEffect::setAttributeArray
        (QGLShaderProgram *program, int location,
         const QGLAttributeValue& value)
{
    // This function is working around a wart in the Qt 4.6 QGLShaderProgram
    // API that doesn't allow the vertex attribute type (GL_FLOAT, etc) to
    // be specified via the public API.  XXX - Need to fix this in 4.7.
#if defined(QT_OPENGL_ES_2)
    Q_UNUSED(program);
    glVertexAttribPointer(GLuint(location), value.tupleSize(),
                          GLenum(value.type()), GL_FALSE,
                          value.stride(), value.data());
#elif QT_VERSION >= 0x040700
    program->setAttributeArray
        (location, value.type(), value.data(),
         value.tupleSize(), value.stride());
#elif !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
    Q_UNUSED(program);
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return;
    QGLPainterPrivate *painter =
        QGLPainterPrivateCache::instance()->fromContext(ctx);
    QGLPainterExtensions *extensions = painter->extensions();
    if (!extensions->vertexAttribPointer) {
        extensions->vertexAttribPointer = (q_glVertexAttribPointer)
            ctx->getProcAddress(QLatin1String("glVertexAttribPointer"));
        if (!extensions->vertexAttribPointer)
            return;
    }
    (*extensions->vertexAttribPointer)
        (GLuint(location), value.tupleSize(), GLenum(value.type()), GL_FALSE,
         value.stride(), value.data());
#else
    Q_UNUSED(program);
    Q_UNUSED(location);
    Q_UNUSED(value);
#endif
}

QT_END_NAMESPACE
