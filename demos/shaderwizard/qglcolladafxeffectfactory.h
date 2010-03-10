#ifndef QGLCOLLADAFXEFFECTFACTORY_H
#define QGLCOLLADAFXEFFECTFACTORY_H

#include <QVector>
#include <QXmlStreamReader>
#include "qgl.h"
#include "qgltexture2d.h"

#include "qglcolladafxeffect.h"

typedef QStack< QList<QGLColladaParam*>* > StateStack;

class QGLColladaImageParam;
class QGLColladaSurfaceParam;
class QGLColladaSampler2DParam;



class QGLColladaFxEffectFactory
{
public:
    // Collada parsing functions
    static QList<QGLColladaFxEffect*> loadEffectsFromFile(const QString& fileName );

protected:
    static QList<QGLColladaFxEffect*> loadEffectsFromXml( QXmlStreamReader& xml );
    static void processLibraryImagesElement( QXmlStreamReader& xml, StateStack* stateStack );
    static QList<QGLColladaFxEffect*> processLibraryEffectsElement( QXmlStreamReader& xml, StateStack* stateStack );
    static QList<QGLColladaFxEffect*> processEffectElement( QXmlStreamReader& xml, StateStack* stateStack );
    static QList<QGLColladaFxEffect*> processProfileElement( QXmlStreamReader& xml, StateStack* stateStack );

    static QGLColladaParam* processPassElement( QXmlStreamReader& xml, StateStack* stateStack, QGLColladaFxEffect* effect );
    static QGLColladaFxEffect* processTechniqueElement( QXmlStreamReader& xml, StateStack* stateStack, QString &profileName );
    static QGLColladaParam* processNewparamElement( QXmlStreamReader& xml, StateStack* stateStack );
    static QGLColladaImageParam* processImageElement( QXmlStreamReader& xml, StateStack* stateStack );
    static QGLColladaSurfaceParam* processSurfaceElement( QXmlStreamReader& xml, StateStack* stateStack, QString passedInSid = "");
    static QGLColladaSampler2DParam* processSampler2DElement( QXmlStreamReader& xml, StateStack* stateStack, QString passedInSid );
    static QGLTexture2D* processTextureElement( QXmlStreamReader& xml , StateStack* stateStack );
    static QVector<float> processFloatList( QXmlStreamReader& xml );
    static QColor processColorElement( QXmlStreamReader& xml );
    static float processParamOrFloatElement( QXmlStreamReader& xml );
    static QColor processColorOrTextureElement( QXmlStreamReader& xml );
    QGLColladaFxEffectFactory();
    static void processProgramElement( QXmlStreamReader& xml, StateStack* stateStack, QGLColladaFxEffect* effect );

    // Collada generation functions
public:
    static QString exportEffect(QGLShaderProgramEffect *effect, QString effectId, QString techniqueSid);

protected:
    static QStringList glslProfileFromEffect(QGLShaderProgramEffect* effect, QString techniqueSid);
    static QStringList generateProgramElement(QGLShaderProgramEffect* effect, QString techniqueSid);
    static QStringList generateShaderElement( QGLShaderProgramEffect* effect, QString vertexShaderRefSid, QString fragmentShaderRefSid );
    static QStringList generateBindUniformElement( QGLShaderProgramEffect* effect );
    static QStringList generateBindAttributeElement( QGLShaderProgramEffect* effect );
    static QStringList generateBindUniformElements( QGLShaderProgramEffect* effect );
    static QStringList generateCodeElements( QGLShaderProgramEffect* effect, QString baseSid );
};



class QGLColladaParam
{
    friend class QGLColladaFxEffectFactory;
public:
    QGLColladaParam( QString sid, float f );
    QGLColladaParam( QString sid, float x, float y );
    QGLColladaParam( QString sid, float x, float y, float z );
    QGLColladaParam( QString sid, float x, float y, float z, float w);

    enum {
        UnknownType = 0,
        floatType,
        float2Type,
        float3Type,
        float4Type,
        Sampler2DType,
        Texture2DType,
        SurfaceType,
        ImageType,
        CodeType,
        UserDefinedType = 100
    };

    int type();
    QVector<float> value();
    QString sid();
    QString id();
    static QString typeString(int);
protected:
    QGLColladaParam(QString sid, int type);
    QString mSid;
    QString mId;
    int mType;
    QVector<float> mValue;
};



class QGLColladaTextureParam : public QGLColladaParam
{
    friend class QGLColladaFxEffectFactory;
public:
    QGLColladaTextureParam(QString sid, QGLTexture2D* texture);
    QGLTexture2D* texture();
    QString samplerSid();
protected:
    QGLTexture2D* mTexture;
    QString sampler2DSid;
    QString texCoordSid;
};



class QGLColladaSurfaceParam : public QGLColladaParam
{
    friend class QGLColladaFxEffectFactory;
public:
    QGLColladaSurfaceParam(QString sid);
protected:
    QString mInitFrom;
    QString mFormat;
    QString mFormatHint;
    QString mSize;
    QVector<int> mSizeVector;
    QPointF mViewportRatio;
    int mMipLevels;
    bool mMipMapGenerate;
    QString mExtra;
    QString mGenerator;
};



class QGLColladaSampler2DParam : public QGLColladaParam
{
    friend class QGLColladaFxEffectFactory;
public:
    QGLColladaSampler2DParam(QString sid, QGLTexture2D* sampler);
    QGLColladaSampler2DParam(QString sid, QString sourceSid);
    QGLTexture2D sampler();
    QString sourceSid();
protected:
    QGLTexture2D* mTexture;
    QString mSourceSid;
};



// "image" isn't really a param, but it shares enough that it seems sensible
// to re-use the QGLColladaParam base.
class QGLColladaImageParam : public QGLColladaParam
{
    friend class QGLColladaFxEffectFactory;
public:
    QGLColladaImageParam(QString sid, QImage image);
    QImage image();
    QString name();
protected:
    QImage mImage;
    QString mName;
};



class QGLColladaCodeParam : public QGLColladaParam
{
    friend class QGLColladaFxEffectFactory;
public:
    QGLColladaCodeParam(QString sid, QString codeText);
    const QString& code();
protected:
    const QString mCodeText;
};

#endif // QGLCOLLADAFXEFFECTFACTORY_H
