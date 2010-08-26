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

#include "shaderprogram.h"
#include "shaderprogram_p.h"
#include "qglabstracteffect.h"
#include <QtOpenGL/qglshaderprogram.h>
#include <QWeakPointer>

#include "private/qdeclarativepixmapcache_p.h"
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

/*!
    \qmlclass ShaderProgram ShaderProgram
    \brief The ShaderProgram item is derivative class of the more general Effect class in QML/3d.
    Whereas the Effect class provides support for standard effects under OpenGL, the ShaderProgramEffect supports effects based on custom shader programs for the GPU.
    \since 4.8
    \ingroup qt3d::qml3d
    \inherits Effect

    The ShaderProgram class provides Qml/3d users with the ability to use a  QGLShaderProgram within the
    logical context of the normal \l Effect class provided by Qml/3d.

    \section1 Attributes

    ShaderProgram provides a standard set of 8 vertex attributes that
    can be provided via the geometry \l Mesh:

    \table
    \header \o Shader Variable \o Mesh Attribute \o Purpose
    \row \o \c qgl_Vertex \o QGL::Position
         \o The primary position of the vertex.
    \row \o \c qgl_Normal \o QGL::Normal
         \o The normal at each vertex, for lit material effects.
    \row \o \c qgl_Color \o QGL::Color
         \o The color at each vertex, for per-vertex color effects.
    \row \o \c qgl_TexCoord0 \o QGL::TextureCoord0
         \o The texture co-ordinate at each vertex for texture unit 0.
    \row \o \c qgl_TexCoord1 \o QGL::TextureCoord1
         \o Secondary texture co-ordinate at each vertex.
    \row \o \c qgl_TexCoord2 \o QGL::TextureCoord2
         \o Tertiary texture co-ordinate at each vertex.
    \row \o \c qgl_Custom0 \o QGL::CustomVertex0
         \o First custom vertex attribute that can be used for any
            user-defined purpose.
    \row \o \c qgl_Custom1 \o QGL::CustomVertex1
         \o Second custom vertex attribute that can be used for any
            user-defined purpose.
    \endtable

    These attributes are used in the vertexShader, as in the following
    example of a simple texture shader:

    \code
    attribute highp vec4 qgl_Vertex;
    attribute highp vec4 qgl_TexCoord0;
    uniform mediump mat4 qgl_ModelViewProjectionMatrix;
    varying highp vec4 texCoord;

    void main(void)
    {
        gl_Position = qgl_ModelViewProjectionMatrix * qgl_Vertex;
        texCoord = qgl_TexCoord0;
    }
    \endcode

    \section1 Standard uniform variables

    ShaderProgram provides a standard set of uniform variables for
    common values from the environment:

    \table
    \header \o Shader Variable \o Purpose
    \row \o \c qgl_ModelViewProjectionMatrix
         \o Combination of the modelview and projection matrices into a
            single 4x4 matrix.
    \row \o \c qgl_ModelViewMatrix
         \o Modelview matrix without the projection.  This is typically
            used for performing calculations in eye co-ordinates.
    \row \o \c qgl_NormalMatrix
         \o Normal matrix, which is the transpose of the inverse of the
            top-left 3x3 part of the modelview matrix.  This is typically
            used in lighting calcuations to transform \c qgl_Normal.
    \row \o \c qgl_Texture0
         \o Sampler holding the texture from the Effect::texture property.
    \row \o \c qgl_Color
         \o Set to the value of the Effect::color property.
    \row \o \c qgl_LightPosition
         \o Position or direction of the main scene light in eye
            co-ordinates.  If the w component is 0, then the
            value is directional, otherwise it is positional.
    \row \o \c qgl_SpotDirection
         \o Eye co-ordinate direction that the spot-light is shining in.
    \endtable

    The above variables are usually declared in the shaders as follows
    (where \c highp may be replaced with \c mediump or \c lowp depending
    upon the shader's precision requirements):

    \code
    uniform highp mat4 qgl_ModelViewProjectionMatrix;
    uniform highp mat4 qgl_ModelViewMatrix;
    uniform highp mat3 qgl_NormalMatrix;
    uniform sampler2D qgl_Texture0;
    uniform highp vec4 qgl_Color;
    uniform highp vec4 qgl_LightPosition;
    uniform highp vec3 qgl_SpotDirection;
    \endcode

    Other lighting and material values, such as the ambient, diffuse,
    and specular colors, can be passed to the shader program using
    custom uniform variables.

    \section1 Custom uniform variables

    Many properties defined on the ShaderProgram are automatically exposed as
    uniforms for the fragment and vertex shaders under the same name.

    QML and shader types do not match exactly, so the following table shows
    how QML properties should be declared in qml compared to shader programs:

    \table
    \header \o QML Property \o Shader Program Variable
\row \o \code property double myDouble : 1.0 \endcode \o uniform highp float myDouble;
    \row \o \code property real myReal : 1.0 \endcode \o uniform mediump float myReal;
    \row \o \code property bool myBoolean : true \endcode \o uniform bool myBoolean;
    \row \o \code property int myInt : 1 \endcode \o uniform int myInt;
    \row \o \code property variant myPoint : Qt.point(1, 1) \endcode \o uniform mediump vec2 myPoint;
    \row \o \code property variant myPointF : Qt.point(1.0, 1.0) \endcode \o uniform mediump vec2 myPointF;
    \row \o \code property variant mySize : Qt.size(1.0, 1.0) \endcode \o uniform mediump vec2 mySize;
    \row \o \code property color myColor : "#80c342" \endcode \o uniform lowp vec4 myColor;
    \row \o \code property variant myMatrix3x3 :
            [1.0, 0.0, 0.0,
             0.0, 1.0, 0.0,
             0.0, 0.0, 1.0] \endcode \o uniform mat3 myMatrix3x3;
    \row \o \code property variant myMatrix4x4 :
        [1.0 , 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0 ]\endcode \o uniform mat4 myMatrix4x4;
    \row \o \code property string imageExample :
        "http://example.com/image.png" \endcode \o uniform sampler2D imageExample;
    \endtable

    Note: The precision hints in this table are just examples.  highp,
    mediump, and lowp do not map directly onto floats, doubles, colors etc.
    Choose the most appropriate variable type for your qml or javascript, and
    the most appropriate precision for your shader program.

    Be aware that variant properties in general and matrices in particular
    can have significant performance implications.  Conversion from variants
    can be slow, and matrices can consume multiple slots for uniforms, which
    are usually limited by hardware.

    String properties are assumed to be urls to images for use in textures.
    Where these images are remote, they are loaded in the background and bound
    to the effect when they are ready.
    \sa QGLGraphicsViewportItem
*/

class ShaderProgramEffect;
QT_BEGIN_NAMESPACE

class ShaderProgramPrivate
{
public:
    ShaderProgramPrivate() : regenerate(false), effect(0) {}

    QString vertexShader;
    QString fragmentShader;
    bool regenerate;
    ShaderProgramEffect *effect;
};


/*
  \internal
  Construction for the ShaderProgramEffect class consists of setting the key parameter values of the
  class to undefined.  As such, a shader program effect with no further initialisation will do nothing at all
  until further creation of shader programs for it has been carried out.
*/
ShaderProgramEffect::ShaderProgramEffect(ShaderProgram* parent)
{
    this->parent = parent;
    program = 0;
    vertexAttr = -1;
    normalAttr = -1;
    colorAttr = -1;
    texCoord0Attr = -1;
    texCoord1Attr = -1;
    texCoord2Attr = -1;
    customVertex0Attr = -1;
    customVertex1Attr = -1;
    matrixUniform = -1;
    modelViewMatrixUniform = -1;
    normalMatrixUniform = -1;
    texture0 = -1;
    colorUniform = -1;
    lightPositionUniform = -1;
    spotDirectionUniform = -1;
    nextTextureUnit = 1;
    propertyListener = new ShaderProgramPropertyListenerEx(parent, this);
}

/*
  \internal
  Destruction entails deletion of the underlying \l QGLShaderProgram which forms the functional core of the
  class.
*/
ShaderProgramEffect::~ShaderProgramEffect()
{
    delete program;
    QList<QGLTexture2D*> textures = texture2Ds.values();
    QGLTexture2D* texture;
    foreach(texture, textures)
        delete texture;
    QDeclarativePixmap* declarativePixmap;
    foreach(declarativePixmap, declarativePixmaps)
        delete declarativePixmap;
}

/*
  \internal
  The act of shader programe creation can be undertakn in the manner defined for the QGLShaderProgram class.
  Failure to successfully carry out creation will result in a warning message.  Success will auto-populate the
  parameter fields of the ShaderProgramEffect with the necessary values based on the shader program.

  The vertex shader source is defined as a QString in the \a vertexShader parameter, while the fragment shader
  is provided in the \a fragmentShader parameter.
*/
void ShaderProgramEffect::create
    (const QString& vertexShader, const QString& fragmentShader)
{
    delete program;
    program = new QGLShaderProgram();
    if (!program->addShaderFromSourceCode(QGLShader::Vertex, vertexShader))
        qWarning("Could not compile vertex shader");
    if (!program->addShaderFromSourceCode(QGLShader::Fragment, fragmentShader))
        qWarning("Could not compile fragment shader");
    // Some systems require the position to be at location 0.
    program->bindAttributeLocation("qgl_Vertex", 0);
    if (!program->link())
        qWarning("Could not link shader program");
    vertexAttr = program->attributeLocation("qgl_Vertex");
    normalAttr = program->attributeLocation("qgl_Normal");
    colorAttr = program->attributeLocation("qgl_Color");
    texCoord0Attr = program->attributeLocation("qgl_TexCoord0");
    texCoord1Attr = program->attributeLocation("qgl_TexCoord1");
    texCoord2Attr = program->attributeLocation("qgl_TexCoord2");
    customVertex0Attr = program->attributeLocation("qgl_Custom0");
    customVertex1Attr = program->attributeLocation("qgl_Custom1");
    matrixUniform = program->uniformLocation("qgl_ModelViewProjectionMatrix");
    modelViewMatrixUniform = program->uniformLocation("qgl_ModelViewMatrix");
    normalMatrixUniform = program->uniformLocation("qgl_NormalMatrix");
    texture0 = program->uniformLocation("qgl_Texture0");
    colorUniform = program->uniformLocation("qgl_Color");
    lightPositionUniform = program->uniformLocation("qgl_LightPosition");
    spotDirectionUniform = program->uniformLocation("qgl_SpotDirection");
    setUniformLocationsFromParentProperties();
}

/*!
  \internal
  Convenience function to setup the relationship between object properties
  and shader uniforms for later use.
  */
inline void ShaderProgramEffect::setUniformLocationsFromParentProperties()
{
    propertyIdsToUniformLocations.clear();
    uniformLocationsToTextureUnits.clear();
    nextTextureUnit = 1;
    propertyListener->disconnect();
    if (parent.data() == 0)
    {
        return;
    }
    QObject::connect(propertyListener, SIGNAL(propertyChanged()), parent.data(), SIGNAL(effectChanged()));

    const QMetaObject* parentMetaObject = parent.data()->metaObject();
    int parentMethodCount = parentMetaObject->methodCount();

    for(int i = parentMetaObject->propertyOffset();
    i < parentMetaObject->propertyCount(); i++)
    {
        QMetaProperty metaProperty = parentMetaObject->property(i);
        QString propertyName = metaProperty.name();
        int location = program->uniformLocation(propertyName);
        // -1 indicates that the program does not use the variable,
        // so ignore those variables.
        if(location != -1)
        {
            dirtyProperties.append(i);
            propertyIdsToUniformLocations[i] = location;
            if(metaProperty.hasNotifySignal())
            {
                QMetaMethod notifySignal = metaProperty.notifySignal();

                int signalIndex = notifySignal.methodIndex();

                // Connect the myFooChanged() signal from the ShaderProgram
                // to the corresponding imaginary slot on the listener
                // Use the method count to make sure that we don't stomp on
                // real methods and add the property index to tell the
                // properties apart.
                // Warning: Subclasses of ShaderProgramPropertyListener will
                // generate spurious property updates and lots of warnings
                // and might even crash
                QMetaObject::connect(parent.data(), signalIndex,
                                     propertyListener,  parentMethodCount + i);
            } else {
                qWarning() << "Warning: No notification signal found for property: " << propertyName;
                propertiesWithoutNotificationSignal.append(i);
            }
        }
    }

    // Refresh everything
    this->setPropertiesDirty();
}

/*!
  \internal
  This function returns a list of the requisite parameter fields for the shader program currently defined.
  This assists by clearly identifying the items which need to be specified for correct funcitoning of the
  program.
*/
QList<QGL::VertexAttribute> ShaderProgramEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    if (vertexAttr != -1)
        fields.append(QGL::Position);
    if (normalAttr != -1)
        fields.append(QGL::Normal);
    if (colorAttr != -1)
        fields.append(QGL::Color);
    if (texCoord0Attr != -1)
        fields.append(QGL::TextureCoord0);
    if (texCoord1Attr != -1)
        fields.append(QGL::TextureCoord1);
    if (texCoord2Attr != -1)
        fields.append(QGL::TextureCoord2);
    if (customVertex0Attr != -1)
        fields.append(QGL::CustomVertex0);
    if (customVertex1Attr != -1)
        fields.append(QGL::CustomVertex1);
    return fields;
}

/*!
  \internal
    This activates or deactivates the shader based on the \a flag paramter.
    This effectively binds or releases the QGLShaderProgram to \a painter.
*/
void ShaderProgramEffect::setActive(QGLPainter *painter, bool flag)
{
    Q_UNUSED(painter);
    if (flag) {
        program->bind();
        if (vertexAttr != -1)
            program->enableAttributeArray(vertexAttr);
        if (normalAttr != -1)
            program->enableAttributeArray(normalAttr);
        if (colorAttr != -1)
            program->enableAttributeArray(colorAttr);
        if (texCoord0Attr != -1)
            program->enableAttributeArray(texCoord0Attr);
        if (texCoord1Attr != -1)
            program->enableAttributeArray(texCoord1Attr);
        if (texCoord2Attr != -1)
            program->enableAttributeArray(texCoord2Attr);
        if (customVertex0Attr != -1)
            program->enableAttributeArray(customVertex0Attr);
        if (customVertex1Attr != -1)
            program->enableAttributeArray(customVertex1Attr);
        if (texture0 != -1)
            program->setUniformValue(texture0, 0);
    } else {
        if (vertexAttr != -1)
            program->disableAttributeArray(vertexAttr);
        if (normalAttr != -1)
            program->disableAttributeArray(normalAttr);
        if (colorAttr != -1)
            program->disableAttributeArray(colorAttr);
        if (texCoord0Attr != -1)
            program->disableAttributeArray(texCoord0Attr);
        if (texCoord1Attr != -1)
            program->disableAttributeArray(texCoord1Attr);
        if (texCoord2Attr != -1)
            program->disableAttributeArray(texCoord2Attr);
        if (customVertex0Attr != -1)
            program->disableAttributeArray(customVertex0Attr);
        if (customVertex1Attr != -1)
            program->disableAttributeArray(customVertex1Attr);
        program->release();
    }
}

/*!
  \internal
  Precondition: list is a list of floats
 */
static inline void setUniformFromFloatList(QGLShaderProgram *program, int uniformLocation, QList<QVariant> list)
{
    switch(list.length())
    {
    case 1:
        program->setUniformValue(uniformLocation, list.at(0).toFloat());
        break;
    case 2:
        program->setUniformValue(uniformLocation,
                                 list.at(0).toFloat(),
                                 list.at(1).toFloat());
        break;
    case 3:
        program->setUniformValue(uniformLocation,
                                 list.at(0).toFloat(),
                                 list.at(1).toFloat(),
                                 list.at(2).toFloat());
        break;
    case 4:
        program->setUniformValue(uniformLocation,
                                 list.at(0).toFloat(),
                                 list.at(1).toFloat(),
                                 list.at(2).toFloat(),
                                 list.at(3).toFloat());
        break;
    case 9:
        {
            QMatrix3x3 matrix;
            for(int i = 0; i < 9; i++)
            {
                matrix(i / 3, i % 3) = list.at(i).toFloat();
            }
            program->setUniformValue(uniformLocation, matrix);
        }
        break;
    case 16:
        {
            QMatrix4x4 matrix;
            for(int i = 0; i < 16; i++)
            {
                matrix( i / 4, i % 4) = list.at(i).toFloat();
            }
            program->setUniformValue(uniformLocation, matrix);
        }
        break;
    default:
        // Very little information available to make this warning any more helpful
        qWarning() << "Warning: unexpected list size: " << list.size() << ", only 1-4, 9 and 16 supported";
    }
}

/*!
  \internal
  This performs all updates for the shader program given a QGLPainter \a painter, and the type of update
  being carried out based on the \a updates field, which is an enumeration of the possible painter updates.
*/
void ShaderProgramEffect::update
    (QGLPainter *painter, QGLPainter::Updates updates)
{
    if(changedTextures.count() > 0)
    {
        foreach (int i, changedTextures)
        {
            if(!declarativePixmaps.contains(i))
            {
                changedTextures.remove(i);
                continue;
            }

            if(declarativePixmaps[i]->isReady())
            {
                setUniform(i, declarativePixmaps[i]->pixmap(), painter);
            } else
            {
                qWarning() << "Warning: ShaderProgramEffect failed to apply texture for uniform" << i << (urls.contains(i) ? QString(" url: ") + urls[i] : "");
            }
            changedTextures.remove(i);
        }
    }

    // Update the matrix uniforms.
    if ((updates & QGLPainter::UpdateMatrices) != 0) {
        if (matrixUniform != -1)
            program->setUniformValue(matrixUniform, painter->combinedMatrix());
        if (modelViewMatrixUniform != -1)
            program->setUniformValue(modelViewMatrixUniform, painter->modelViewMatrix());
        if (normalMatrixUniform != -1)
            program->setUniformValue(normalMatrixUniform, painter->normalMatrix());
    }

    // Update the static color in if "qgl_Color" is a uniform.
    if ((updates & QGLPainter::UpdateColor) != 0 && colorUniform != -1) {
        program->setUniformValue(colorUniform, painter->color());
    }

    // Update the lighting values.
    if ((updates & QGLPainter::UpdateLights) != 0 &&
            (lightPositionUniform != -1 || spotDirectionUniform != -1)) {
        const QGLLightParameters *lparams = painter->mainLight();
        QMatrix4x4 ltransform = painter->mainLightTransform();
        if (lightPositionUniform != -1) {
            program->setUniformValue
                (lightPositionUniform, lparams->eyePosition(ltransform));
        }
        if (spotDirectionUniform != -1) {
            program->setUniformValue
                (spotDirectionUniform, lparams->eyeSpotDirection(ltransform));
        }
    }

    // Assign custom properties if they exist
    if(!parent.data() || !propertyIdsToUniformLocations.count() > 0)
        return;

    // update dirty properties and remove them from the list
    int propertyIndex;
    QList<int> propertiesNotUpdated;
    foreach (propertyIndex, dirtyProperties)
    {
        if(!setUniformForPropertyIndex(propertyIndex, painter))
        {
            propertiesNotUpdated.append(propertyIndex);
        };
    }
    dirtyProperties.clear();
    dirtyProperties.append(propertiesNotUpdated);

    // always update the properties we can't track
    foreach (propertyIndex, propertiesWithoutNotificationSignal)
    {
        setUniformForPropertyIndex(propertyIndex, painter);
    }
}

inline QGLTexture2D* ShaderProgramEffect::textureForUniformValue(int uniformLocation)
{
    QGLTexture2D* result = texture2Ds.value(uniformLocation);
    if(result == 0)
    {
        result = new QGLTexture2D();
        texture2Ds[uniformLocation] = result;
    }
    return result;
}

inline bool ShaderProgramEffect::setUniformForPropertyIndex(int propertyIndex, QGLPainter *painter)
{
    int uniformLocation = propertyIdsToUniformLocations[propertyIndex];

    QVariant value =
            parent.data()->metaObject()->property(propertyIndex).read(parent.data());

    switch(value.type())
    {
    case QVariant::Double:
        // Convert double to float to pass to shader program
    case QMetaType::Float:
        program->setUniformValue(uniformLocation, value.toFloat());
        break;
    case QVariant::Int:
        program->setUniformValue(uniformLocation, value.toInt());
        break;
    case QVariant::UInt:
        program->setUniformValue(uniformLocation, value.toUInt());
        break;
    case QVariant::Bool:
        program->setUniformValue(uniformLocation, value.toBool());
        break;
    case QVariant::Color:
        program->setUniformValue(uniformLocation, value.value<QColor>());
        break;
    case QVariant::List:
        setUniformFromFloatList(program, uniformLocation, value.toList());
        break;
    case QVariant::Point:
        program->setUniformValue(uniformLocation, value.toPoint());
        break;
    case QVariant::PointF:
        program->setUniformValue(uniformLocation, value.toPointF());
        break;
    case QVariant::Size:
        program->setUniformValue(uniformLocation, value.toSize());
        break;
    case QVariant::SizeF:
        program->setUniformValue(uniformLocation, value.toSizeF());
        break;
    case QVariant::Matrix4x4:
        program->setUniformValue(uniformLocation, value.value<QMatrix4x4>());
        break;
    case QVariant::Vector2D:
        program->setUniformValue(uniformLocation, value.value<QVector2D>());
        break;
    case QVariant::Vector3D:
        program->setUniformValue(uniformLocation, value.value<QVector3D>());
        break;
    case QVariant::Vector4D:
        program->setUniformValue(uniformLocation, value.value<QVector4D>());
        break;
    case QVariant::String:
        {
            // We assume strings are URLs to images for textures
            QString urlString = value.toString();
            processTextureUrl(uniformLocation, urlString);
        }
        break;
    case QVariant::Image:
        {
            QImage image(value.toString());
            setUniform(uniformLocation, image, painter);
        }
        break;
    default:
        qWarning() << "Unrecognized variant for property " << parent.data()->metaObject()->property(propertyIndex).name() << " of type " << value.typeName() << ", could not set corresponding shader variable";
    }
    return true;
}

/*!
  \internal Helper function for applying a \a pixmap to a texture for a shader program.  This function should be called from within update() in order to have access to the right GL context through the \a painter.
  */
void ShaderProgramEffect::setUniform
        (int uniformLocation, const QPixmap pixmap, QGLPainter* painter)
{
    // TODO: Perspective correction
    QGLTexture2D* texture = textureForUniformValue(uniformLocation);
    int unit = textureUnitForUniformValue(uniformLocation);
    if(texture != 0)
    {
        texture->setPixmap(pixmap);
        painter->glActiveTexture(GL_TEXTURE0 + unit);
        texture->bind();
        program->setUniformValue(uniformLocation, unit);
    }
}

/*!
  \internal Helper function for applying an \a images to a texture for a shader program.  This function should be called from within update() in order to have access to the right GL context through the \a painter.
  */
void ShaderProgramEffect::setUniform
        (int uniformLocation, const QImage& image, QGLPainter* painter)
{
    // TODO: Perspective correction
    QGLTexture2D* texture = textureForUniformValue(uniformLocation);
    int unit = textureUnitForUniformValue(uniformLocation);
    if(texture != 0)
    {
        texture->setImage(image);
        painter->glActiveTexture(GL_TEXTURE0 + unit);
        texture->bind();
        program->setUniformValue(uniformLocation, unit);
    }
}

/*!
  \internal Find the texture unit to associate with \a uniformLocation.
*/
int ShaderProgramEffect::textureUnitForUniformValue(int uniformLocation)
{
    int unit = uniformLocationsToTextureUnits.value(uniformLocation, -1);
    if (unit == -1) {
        unit = nextTextureUnit++;
        uniformLocationsToTextureUnits[uniformLocation] = unit;
    }
    return unit;
}

/*
  The function facilitates setting of a given vertex attribute \attribute with the value defined by
  \a value.
*/
void ShaderProgramEffect::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
    switch (attribute) {
    case QGL::Position:
        if (vertexAttr != -1)
            setAttributeArray(program, vertexAttr, value);
        break;
    case QGL::Normal:
        if (normalAttr != -1) {
            setAttributeArray(program, normalAttr, value);
            // Must explicitly enable in case setCommonNormal() was called.
            program->enableAttributeArray(normalAttr);
        }
        break;
    case QGL::Color:
        if (colorAttr != -1)
            setAttributeArray(program, colorAttr, value);
        break;
    case QGL::TextureCoord0:
        if (texCoord0Attr != -1)
            setAttributeArray(program, texCoord0Attr, value);
        break;
    case QGL::TextureCoord1:
        if (texCoord1Attr != -1)
            setAttributeArray(program, texCoord1Attr, value);
        break;
    case QGL::TextureCoord2:
        if (texCoord2Attr != -1)
            setAttributeArray(program, texCoord2Attr, value);
        break;
    case QGL::CustomVertex0:
        if (customVertex0Attr != -1)
            setAttributeArray(program, customVertex0Attr, value);
        break;
    case QGL::CustomVertex1:
        if (customVertex1Attr != -1)
            setAttributeArray(program, customVertex1Attr, value);
        break;
    default: break;
    }
}

/*
  Sets the common normal (a QVector3d) for the ShaderProgramEffect to \a value.
*/
void ShaderProgramEffect::setCommonNormal(const QVector3D& value)
{
    if (normalAttr != -1) {
        program->disableAttributeArray(normalAttr);
        program->setAttributeValue(normalAttr, value);
    }
}

/*!
  \internal set all properties dirty so they are reuploaded
  next update()
  */
void ShaderProgramEffect::setPropertiesDirty()
{
    dirtyProperties = this->propertyIdsToUniformLocations.keys();
}

/*!
  \internal Set a specific property as dirty so that it is reuploaded
  next update()
  */
void ShaderProgramEffect::setPropertyDirty(int property)
{
    if(dirtyProperties.indexOf(property) == -1)
    {
        dirtyProperties.append(property);
    }
}

/*!
  \internal Notification recieved via parent ShaderProgram parent that
  a requested network resource is ready, so load the pixmap and get
  ready for painting with it.

  Returns true if the Effect has finished loading all it's remote resources.
*/
bool ShaderProgramEffect::pixmapRequestFinished()
{
    bool removedTexture = false;
    foreach (int i, loadingTextures)
    {
        if(!declarativePixmaps.contains(i))
        {
            loadingTextures.remove(i);
            removedTexture = true;
            continue;
        }

        if(declarativePixmaps[i]->isReady())
        {
            changedTextures.insert(i);
            loadingTextures.remove(i);
            removedTexture = true;
        } else if (!declarativePixmaps[i]->isLoading())
        {
            qWarning() << "Error loading " << urls[i] << ": "
                    << declarativePixmaps[i]->error();
            loadingTextures.remove(i);
            removedTexture = true;
        }
    }
    return (removedTexture && loadingTextures.count() == 0);
}

/*!
  \internal Update the image for the texture bound at \a uniform location with
    the the image at \a urlString.  If \a urlString is a remote resource, this
    starts an asycnrounous loading process.

    Note: Consecutive calls with the same url for a given uniform are ignored.
*/
void ShaderProgramEffect::processTextureUrl(int uniformLocation, QString urlString)
{
    QUrl url(urlString);
    if(urlString.isEmpty() &&
       urls.contains(uniformLocation) &&
       !urls[uniformLocation].isNull())
    {
        if(!declarativePixmaps[uniformLocation]->isNull())
        {
            declarativePixmaps[uniformLocation]->clear(parent.data());
            urls.remove(uniformLocation);
            changedTextures.insert(uniformLocation);
            return;
        }
    };

    bool async = true;
    // Try to make path absolute:
    if (url.isRelative())
    {
        async = false;
        // Get the baseUrl from the declarative engine
        QDeclarativeContext *context =
                QDeclarativeEngine::contextForObject(parent.data());

        if(context)
        {
            QUrl baseurl = context->baseUrl();
            QUrl absolute =  baseurl.resolved(urlString);

            if(absolute.isValid())
            {
                url = absolute;
                urlString = absolute.toString();
            } else {
                qWarning() << "Warning: failed to resolve relative path " <<
                        urlString;
            }
        }
    };

    if (urlString != urls[uniformLocation])
    {
        urls.insert(uniformLocation, urlString);
        if(declarativePixmaps.contains(uniformLocation))
            declarativePixmaps[uniformLocation]->clear();
        else
            declarativePixmaps[uniformLocation] = new QDeclarativePixmap();

        QDeclarativePixmap* declarativePixmap =
                declarativePixmaps[uniformLocation];
        QDeclarativeEngine *engine = qmlEngine(parent.data());
        declarativePixmap->load(engine, urlString, async);

        QDeclarativePixmap::Status status =
                declarativePixmap->status();
        if( status == QDeclarativePixmap::Ready)
        {
            changedTextures.insert(uniformLocation);
            return;
        } else if (status == QDeclarativePixmap::Loading)
        {
            declarativePixmap->connectFinished(parent.data(), SLOT(pixmapRequestFinished()));
            loadingTextures.insert(uniformLocation);
        } else {
            qWarning() << "Failed to load texture " << urlString << ": "
                    << declarativePixmap->error();
        }
    }
}

/*!
  \internal
  Construction of the shader program and assignment of its \a parent object.
*/
ShaderProgram::ShaderProgram(QObject *parent)
    : Effect(parent)
{
    d = new ShaderProgramPrivate();
}

/*!
  \internal
  Destruction of the ShaderProgram entails deletion of private data, and explicit deletion of the
  underlying ShaderProgramEffect defined by the class.
*/
ShaderProgram::~ShaderProgram()
{
    delete d->effect;
    delete d;
}

/*!
  \qmlproperty string ShaderProgram::vertexShader

  This property defines the source for the vertex shader to be implemented by this
  instance of the ShaderProgram.

  \sa fragmentShader
*/
QString ShaderProgram::vertexShader() const
{
    return d->vertexShader;
}

void ShaderProgram::setVertexShader(const QString& value)
{
    d->vertexShader = value;
    d->regenerate = true;
    emit effectChanged();
}


/*!
  \qmlproperty string ShaderProgram::fragmentShader
  This property defines the source for the fragment shader (ie. pixel shader) to be
  implemented by this instance of the ShaderProgram.

  \sa vertexShader
*/
QString ShaderProgram::fragmentShader() const
{
    return d->fragmentShader;
}

void ShaderProgram::setFragmentShader(const QString& value)
{
    d->fragmentShader = value;
    d->regenerate = true;
    emit effectChanged();
}

/*!
  \internal
  Enables the effect for a given \a painter.  If the effect has not been created yet, this function will
  attempt to do so.
*/
void ShaderProgram::enableEffect(QGLPainter *painter)
{
    if (!d->effect) {
        d->effect = new ShaderProgramEffect(this);
        d->effect->create(d->vertexShader, d->fragmentShader);
    } else if (d->regenerate) {
        d->effect->create(d->vertexShader, d->fragmentShader);
    }
    d->regenerate = false;
    painter->setUserEffect(d->effect);
    painter->setColor(color());
    QGLTexture2D *tex = texture2D();
    painter->glActiveTexture(GL_TEXTURE0);
    if (tex)
        tex->bind();
    else
        glBindTexture(GL_TEXTURE_2D, 0);
}

/*!
  \internal
  Mark all properties as dirty to be re-uploaded in the next update
*/
void ShaderProgram::markAllPropertiesDirty()
{
    d->effect->setPropertiesDirty();
}

/*!
  \internal
  Mark a \a property as dirty to be re-uploaded in the next update
  */
void ShaderProgram::markPropertyDirty(int property)
{
    d->effect->setPropertyDirty(property);
}

/*!
  \qmlsignal ShaderProgram::onFinishedLoading()
  Emitted when the last remote resource request is resolved, and implies that
  the effect is ready to be displayed.
*/

/*!
 \internal Proxy the signal for the ShaderProgramEffect class
 */
void ShaderProgram::pixmapRequestFinished()
{
    if(d->effect->pixmapRequestFinished())
    {
        emit finishedLoading();
    }
}

/*!
    \internal
    A subclass without the Q_OBJECT macro in order to use the qt_metacall trick to track property changes.

    It is also conveniently placed to connect appropriate properties to
    Effect::effectChanged() to trigger timely updates.
  */
ShaderProgramPropertyListenerEx::ShaderProgramPropertyListenerEx(ShaderProgram *parent, ShaderProgramEffect* effect)
    : ShaderProgramPropertyListener(parent), effect(effect)
{
    shaderProgramMethodCount = parent->metaObject()->methodCount();
}

/*!
    \internal
*/
ShaderProgramPropertyListenerEx::~ShaderProgramPropertyListenerEx()
{
}

/*!
    \internal
    Find calls to the "imaginary" slots, and mark the appropriate property
    as dirty.
*/
int ShaderProgramPropertyListenerEx::qt_metacall(QMetaObject::Call c, int id, void **a)
{
    if (c == QMetaObject::InvokeMetaMethod )
    {
        if(id >= shaderProgramMethodCount) {
            effect->setPropertyDirty(id - shaderProgramMethodCount);
            emit propertyChanged();
        }
        // Consume the metacall
        return -1;
    }

    return ShaderProgramPropertyListener::qt_metacall(c, id, a);
}

QT_END_NAMESPACE
