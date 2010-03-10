#ifndef QGLCOLLADAFXEFFECT_P_H
#define QGLCOLLADAFXEFFECT_P_H
#include <QString>
#include <QMap>
#include <QColor>
#include "qgltexture2d.h"
#include "qglcolladafxeffect.h"

class QGLPainter;
class QGLShaderProgram;
class QGLMaterialParameters;

class QGLColladaFxEffectPrivate
{
    friend class QGLColladaFxEffectFactory;
public:
    QGLColladaFxEffectPrivate();
    ~QGLColladaFxEffectPrivate();

    void addMaterialChannelsToShaderSnippets(const QGLMaterialParameters *material);
    void resetGlueSnippets();
    void setTextureUniform(QGLShaderProgram *program, QGLPainter* painter, QString channelName, QGLTexture2D* texture, int* textureUnit, QColor fallbackColor);
    void updateMaterialChannelSnippets(QString channelName, QGLTexture2D* texture, int* textureUnit, QColor fallbackColor);

    QString id;
    QString sid;
    QString name;

    // The spec allows for 3D textures as well, but for now only 2D is
    // supported
    QGLTexture2D* emissiveTexture;
    QGLTexture2D* ambientTexture;
    QGLTexture2D* diffuseTexture;
    QGLTexture2D* specularTexture;
    int lighting;

    QStringList vertexShaderCodeSnippets;
    QStringList vertexShaderDeclarationSnippets;
    QStringList vertexShaderVariableNames;

    QStringList fragmentShaderCodeSnippets;
    QStringList fragmentShaderDeclarationSnippets;
    QStringList fragmentShaderVariableNames;

    QString vertexShaderEndGlueSnippet;
    QString vertexShaderMainGlueSnippet;
    QString fragmentShaderEndGlueSnippet;
    QString fragmentShaderMainGlueSnippet;

    bool hasCustomVertexShader;
    bool hasCustomFragmentShader;
};

#endif // QGLCOLLADAFXEFFECT_P_H
