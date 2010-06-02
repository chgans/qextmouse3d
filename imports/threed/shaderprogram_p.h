#ifndef SHADERPROGRAM_P_H
#define SHADERPROGRAM_P_H

#include <QWeakPointer>
#include <QGLShaderProgram>
#include "effect.h"
#include "qglabstracteffect.h"

class ShaderProgram;

class ShaderProgramEffect;
class QDeclarativePixmapReply;

class ShaderProgramPropertyListener : public QObject
{
    Q_OBJECT
public:
    ShaderProgramPropertyListener(QObject* parent = 0)
        : QObject(parent)
    {
    }
    virtual ~ShaderProgramPropertyListener()
    {
    }
};

class ShaderProgramPropertyListenerEx : public ShaderProgramPropertyListener
{
public:
    ShaderProgramPropertyListenerEx(ShaderProgram* parent, ShaderProgramEffect* effect);
    ~ShaderProgramPropertyListenerEx();

protected:
    virtual int qt_metacall(QMetaObject::Call c, int id, void **a);
    ShaderProgramEffect* effect;
private:
    int shaderProgramMethodCount;
};


/*
  The ShaderProgramEffect class underlies the ShaderProgram class in Qml/3d.  It contains the actual
  QGLShaderProgram along with all of the necessary parameters to use that program.
*/
class ShaderProgramEffect : public QGLAbstractEffect
{
public:
    ShaderProgramEffect(ShaderProgram* parent);
    ~ShaderProgramEffect();

    void create(const QString& vertexShader, const QString& fragmentShader);

    QList<QGL::VertexAttribute> requiredFields() const;

    void setActive(QGLPainter *painter, bool flag);

    void update(QGLPainter *painter, QGLPainter::Updates updates);
    bool setUniformForPropertyIndex(int propertyIndex, QGLPainter *painter);


    void setVertexAttribute
        (QGL::VertexAttribute attribute, const QGLAttributeValue& value);

    void setCommonNormal(const QVector3D& value);
    void setPropertiesDirty();
    void setPropertyDirty(int property);

    void processFinishedRequest(QDeclarativePixmapReply* reply);
private:
    void setUniformLocationsFromParentProperties();
    void setUniform(int uniformValue, const QImage& image,
                             QGLPainter* painter);
    void setUniform(int uniformValue, const QPixmap* pixmap,
                             QGLPainter* painter);
    QGLTexture2D* textureForUniformValue(int uniformLocation);

    QWeakPointer<ShaderProgram> parent;
    QGLShaderProgram *program;
    int vertexAttr;
    int normalAttr;
    int colorAttr;
    int texCoord0Attr;
    int texCoord1Attr;
    int matrixUniform;
    int modelViewMatrixUniform;
    int normalMatrixUniform;
    int texture0;
    int texture1;
    int colorUniform;
    QMap<int, int> propertyIdsToUniformLocations;
    QList<int> dirtyProperties;
    QArray<int> propertiesWithoutNotificationSignal;
    ShaderProgramPropertyListener* propertyListener;

    QMap<int, QGLTexture2D*> texture2DsByUniformValue;
    QMap <int, QPixmap*> pendingPixmapsByUniformLocations;
    QMap <QDeclarativePixmapReply*, int> pendingPixmapRequestsWithUniformLocations;
    QMap <int, QDeclarativePixmapReply*> pendingPixmapRequests;
    QMap <int, QString> urls;
    QArray<int> dirtyTextureUniforms;
};

#endif // SHADERPROGRAM_P_H
