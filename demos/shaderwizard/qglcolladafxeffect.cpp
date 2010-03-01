#include <QString>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QImage>
#include "qgl.h"
#include "qgltexture2d.h"
#include "qglcolladafxeffect.h"
#include "qglcolladafxeffect_p.h"

QGLColladaFxEffect::QGLColladaFxEffect() : QGLShaderProgramEffect()
        , d( new QGLColladaFxEffectPrivate )
{
}



QGLColladaFxEffect::~QGLColladaFxEffect()
{
    delete d;
}



QGLColladaFxEffectPrivate::QGLColladaFxEffectPrivate() : id()
        , sid()
        , name()
        , emissiveTexture(0)
        , ambientTexture(0)
        , diffuseTexture(0)
        , specularTexture(0)
        , lighting(QGLColladaFxEffect::NoLighting)
{
    resetGlueSnippets();
}



QGLColladaFxEffectPrivate::~QGLColladaFxEffectPrivate()
{
    delete emissiveTexture;
    emissiveTexture = 0;
    delete ambientTexture;
    ambientTexture = 0;
    delete diffuseTexture;
    diffuseTexture = 0;
    delete specularTexture;
    specularTexture = 0;
}



inline void QGLColladaFxEffectPrivate::updateMaterialChannelSnippets(QString channelName, QGLTexture2D* texture, int* textureUnit, QColor fallbackColor)
{
    QString qVariableName = "q" + channelName;
    if (texture != 0)
    {
        QString sourceVariableName = "texture" + channelName;
        QString texVariableName = QString("texCoord%1").arg(*textureUnit);
        // Take care of texture coordinates
        QString varyingSnippet = QString("varying vec4 %1;").arg(texVariableName);
        vertexShaderDeclarationSnippets.append(varyingSnippet);
        vertexShaderVariableNames.append(texVariableName);
        fragmentShaderDeclarationSnippets.append(varyingSnippet);
        fragmentShaderVariableNames.append(texVariableName);

        vertexShaderCodeSnippets.append(QString("%1 = texCoords; // TODO: dynamically add tex attributes\n").arg(texVariableName));
        vertexShaderVariableNames.append(texVariableName);

        // Declare the color variable in the fragment shader
        fragmentShaderDeclarationSnippets.append(QString("lowp vec4 %1;").arg(qVariableName));
        fragmentShaderVariableNames.append(qVariableName);
        fragmentShaderDeclarationSnippets.append(QString("uniform sampler2D %1;").arg(sourceVariableName));
        fragmentShaderVariableNames.append(sourceVariableName);

        // Assign a colour to the variable out of the appropriate sampler
        fragmentShaderCodeSnippets.append("    mediump vec4 " + qVariableName + " = texture2D(" + sourceVariableName + ", " + texVariableName + ".st);");
        fragmentShaderVariableNames.append(qVariableName);
        // mediump? lowp?

        *textureUnit++;
    } else {
        fragmentShaderDeclarationSnippets.append(QString ("const vec4 %1 = vec4(%2, %3, %4, %5);").arg( qVariableName).arg(fallbackColor.redF(), 0, 'f', 6).arg(fallbackColor.greenF(), 0, 'f', 6).arg(fallbackColor.blueF(), 0, 'f', 6).arg(fallbackColor.alphaF(), 0, 'f', 6 ));
        fragmentShaderVariableNames.append(qVariableName);
    }
}



inline void QGLColladaFxEffectPrivate::setTextureUniform(QGLShaderProgram *program, QGLPainter* painter, QString channelName, QGLTexture2D* texture, int* textureUnit, QColor fallbackColor)
{
    QString qVariableName = "q" + channelName;

    if (texture != 0)
    {
        QString sourceVariableName = "texture" + channelName;
        QString texVariableName = QString("texCoord%1").arg(*textureUnit);
        painter->setTexture(*textureUnit, texture);
        program->setUniformValue(sourceVariableName.toAscii().data(), *textureUnit);
    }
    else
    {
        // It's just a const value, so set it that way in the fragment shader.
        program->setUniformValue(qVariableName.toAscii().data(), fallbackColor);
    }
}



void QGLColladaFxEffect::update(QGLPainter *painter, QGLPainter::Updates updates)
{
    QGLShaderProgramEffect::update(painter, updates);

    if(updates && QGLPainter::UpdateMaterials)
    {

        if(program() == 0)
        {
            qWarning() << "no program in QGLColladaFxEffect::update()";
            return;
        }

        // Start from texture unit 1 so as not to stomp a texture set on the
        // painter.
        int textureUnit = 1;
        d->setTextureUniform(program(), painter, "Emissive", d->emissiveTexture, &textureUnit, material()->emittedLight());
        d->setTextureUniform(program(), painter, "Ambient", d->ambientTexture, &textureUnit, material()->ambientColor());
        d->setTextureUniform(program(), painter, "Diffuse", d->diffuseTexture, &textureUnit, material()->diffuseColor());
        d->setTextureUniform(program(), painter, "Specular", d->specularTexture, &textureUnit, material()->specularColor());
    }
}



void QGLColladaFxEffect::setId(QString id)
{
    d->id = id;
}



QString QGLColladaFxEffect::id()
{
    return d->id;
}



void QGLColladaFxEffect::setSid(QString sid)
{
    d->sid = sid;
}



void QGLColladaFxEffectPrivate::addMaterialChannelsToShaderSnippets(const QGLMaterialParameters *material)
{
    int textureUnit = 1;

    updateMaterialChannelSnippets("Emissive", emissiveTexture, &textureUnit, material->emittedLight());
    updateMaterialChannelSnippets("Ambient", ambientTexture, &textureUnit, material->ambientColor());
    updateMaterialChannelSnippets("Diffuse", diffuseTexture, &textureUnit, material->diffuseColor());
    updateMaterialChannelSnippets("Specular", specularTexture, &textureUnit, material->specularColor());
}



void QGLColladaFxEffect::addBlinnPhongLighting()
{
    d->addMaterialChannelsToShaderSnippets(material());

    // Fragment shader declarations:
    d->fragmentShaderDeclarationSnippets.append("uniform mediump sampler2D texture0;");
    d->fragmentShaderVariableNames.append("texture0");
    d->fragmentShaderDeclarationSnippets.append("varying highp vec4 qTexCoord0;");
    d->fragmentShaderVariableNames.append("qTexCoord0");

    // Fragment Shader code
    d->fragmentShaderCodeSnippets.append(
            "    vec4 specularComponent = vec4( 0.0, 0.0, 0.0, 0.0 );\n"\
            "    if(intensity > 0.0)\n"\
            "   {\n"\
            "       float specularIntensity = max( dot(perPixelNormal, qHalfVector), 0.0 );\n"\
            "       if(specularIntensity > 0.0)\n"\
            "           specularComponent = qSpecular  * pow(specularIntensity, shininess);\n"\
            "   }\n");
    d->fragmentShaderVariableNames.append("lighting");


    // Replace the "end glue" to set colour from lighting
    d->fragmentShaderEndGlueSnippet =
            "    vec4 texture0Color = texture2D(texture0, qTexCoord0.st);\n"\
            "    vec4 diffuseColor = qDiffuse;\n"\
            "    vec4 lightingColor = qAmbient + diffuseColor * intensity + specularComponent;\n"\
            "   vec4 texturedColor = vec4(lightingColor.xyz * (1.0 - texture0Color.a)\n"\
            "+ (texture0Color.xyz + specularComponent.rgb) * texture0Color.a, lightingColor.a);\n"\
            "    gl_FragColor = texturedColor;\n"\
            "}";
    generateShaders();
}



void QGLColladaFxEffect::generateShaders()
{
    setVertexShader(
            d->vertexShaderDeclarationSnippets.join("\n")
            + "\n" + d->vertexShaderMainGlueSnippet
            + d->vertexShaderCodeSnippets.join("\n")
            + "\n" + d->vertexShaderEndGlueSnippet);

    setFragmentShader(
            d->fragmentShaderDeclarationSnippets.join("\n")
            + "\n" + d->fragmentShaderMainGlueSnippet
            +  d->fragmentShaderCodeSnippets.join("\n")
            + "\n" + d->fragmentShaderEndGlueSnippet);

    // Set inactive to trigger relinking later
    if(isActive())
        setActive(false);
}



void QGLColladaFxEffectPrivate::resetGlueSnippets()
{
    vertexShaderMainGlueSnippet = QString(
            "attribute highp vec4 vertex;\n"\
            "attribute highp vec4 normal;\n"\
            "attribute highp vec4 texCoords;\n"\
            "uniform highp mat4 matrix;\n"\
            "varying mediump vec3 qNormal;\n"\
            "varying mediump vec3 qLightDirection;\n"\
            "varying mediump vec3 qHalfVector;\n"\
            "uniform mediump vec3 pli;       // Position of the light\n"\
            "varying highp vec4 qTexCoord0; // TEMP\n" /* Got to get rid of this*/\
            "\n"\
            "void qLightVertex(vec4 vertex, vec3 normal)\n"\
            "{\n"\
            "    vec3 toEye;\n"\
            "    qLightDirection = normalize(pli);\n"\
            "    toEye = vec3(0, 0, 1); // assume viewer at infinity\n"\
            "    qHalfVector = normalize(qLightDirection + toEye);\n"\
            "}\n"\
            "\n"\
            "void main(void)\n"\
            "{\n"\
            "    qNormal = normalize(gl_NormalMatrix * vec3(normal));\n"\
            "    qLightVertex(vertex, qNormal);\n"\
            "    qTexCoord0 = texCoords;\n"\
            );

    vertexShaderEndGlueSnippet = QString (
            "    gl_Position = matrix * vertex;\n"\
            "}\n");


    fragmentShaderEndGlueSnippet = QString(
            "    gl_FragColor = color;\n"\
            "}\n"
            );

    fragmentShaderMainGlueSnippet = QString(
            "varying mediump vec3 qNormal;\n"\
            "varying mediump vec3 qLightDirection;\n"\
            "varying mediump vec3 qHalfVector;\n"\
            "uniform float shininess;\n"\
            "uniform vec4 color;\n"\
            "vec3 perPixelNormal;"
            "\n"\
            "void main()\n"\
            "{\n"\
            "    perPixelNormal = normalize(qNormal);\n"\
            "    float intensity =  max(dot(perPixelNormal, qLightDirection), 0.0);\n"
            );
}



QString QGLColladaFxEffect::sid()
{
    return d->sid;
}



QGLTexture2D* QGLColladaFxEffect::diffuseTexture()
{
    return d->diffuseTexture;
}

void QGLColladaFxEffect::setLighting(int lighting)
{
    d->lighting = lighting;
}

int QGLColladaFxEffect::lighting()
{
    return d->lighting;
}
