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

#ifndef QGLCOLLADAFXEFFECTFACTORY_H
#define QGLCOLLADAFXEFFECTFACTORY_H

#include <QVector>
#include <QXmlStreamReader>
#include "qgl.h"
#include "qgltexture2d.h"

#include "qglcolladafxeffect.h"

class QGLColladaImageParam;
class QGLColladaSurfaceParam;
class QGLColladaSampler2DParam;

Q_DECLARE_METATYPE(QArray<float>)

typedef struct _ResultState
{
    QMap<QString, QVariant> paramSids;
    QMap<QString, QVariant> paramIds;
    QMap<QString, QVariant> paramNames;
} ResultState;




class QGLColladaFxEffectFactory
{
public:
    // Collada parsing functions
    static QList<QGLColladaFxEffect*> loadEffectsFromFile(const QString& fileName );

protected:
    static QList<QGLColladaFxEffect*> loadEffectsFromXml( QXmlStreamReader& xml );
    static void processLibraryImagesElement( QXmlStreamReader& xml, ResultState* stateStack );
    static QList<QGLColladaFxEffect*> processLibraryEffectsElement( QXmlStreamReader& xml, ResultState* stateStack );
    static QList<QGLColladaFxEffect*> processEffectElement( QXmlStreamReader& xml, ResultState* stateStack );
    static QList<QGLColladaFxEffect*> processProfileElement( QXmlStreamReader& xml, ResultState* stateStack );

    static QGLColladaParam* processPassElement( QXmlStreamReader& xml, ResultState* stateStack, QGLColladaFxEffect* effect );
    static QGLColladaFxEffect* processTechniqueElement( QXmlStreamReader& xml, ResultState* stateStack, QString &profileName );
    static QGLColladaParam* processNewparamElement( QXmlStreamReader& xml, ResultState* stateStack );
    static void processImageElement( QXmlStreamReader& xml, ResultState* stateStack );
    static QGLColladaSurfaceParam* processSurfaceElement( QXmlStreamReader& xml, ResultState* stateStack, QString passedInSid = "");
    static QGLColladaSampler2DParam* processSampler2DElement( QXmlStreamReader& xml, ResultState* stateStack, QString passedInSid );
    static QGLTexture2D* processTextureElement( QXmlStreamReader& xml , ResultState* stateStack );
    static QVariant processFloatList( QXmlStreamReader& xml );
    static QColor processColorElement( QXmlStreamReader& xml );
    static float processParamOrFloatElement( QXmlStreamReader& xml );
    static QVariant processColorOrTextureElement( QXmlStreamReader& xml );
    QGLColladaFxEffectFactory();
    static void processProgramElement( QXmlStreamReader& xml, ResultState* stateStack, QGLColladaFxEffect* effect );

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
    enum {
        UnknownType = 0,
        Sampler2DType,
        Texture2DType,
        SurfaceType,
        ImageType,
        UserDefinedType = 100
    };

    virtual ~QGLColladaParam();

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

#endif // QGLCOLLADAFXEFFECTFACTORY_H
