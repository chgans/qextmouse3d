#ifndef SHADERPROGRAM_P_H
#define SHADERPROGRAM_P_H

#include <QWeakPointer>
#include <QGLShaderProgram>
#include <effect.h>
#include "qglabstracteffect.h"

class ShaderProgram;

class ShaderProgramEffect;

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

/*
  The ShaderProgramEffect class underlies the ShaderProgram class in Qml/3d.  It contains the actual
  QGLShaderProgram along with all of the necessary parameters to use that program.

  An instance of the ShaderProgramEffect class can be found in the private part of the ShaderProgram
  class, thus abstracting much of the complexity away from the user.
*/
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


class ShaderProgramEffect : public QGLAbstractEffect
{
public:
    ShaderProgramEffect(ShaderProgram* parent);
    ~ShaderProgramEffect();

    void create(const QString& vertexShader, const QString& fragmentShader);

    QList<QGL::VertexAttribute> requiredFields() const;

    void setActive(QGLPainter *painter, bool flag);

    void update(QGLPainter *painter, QGLPainter::Updates updates);
    void setUniformForPropertyIndex(int propertyIndex);

    void setVertexAttribute
        (QGL::VertexAttribute attribute, const QGLAttributeValue& value);

    void setCommonNormal(const QVector3D& value);
    void setPropertiesDirty();
    void setPropertyDirty(int property);

private:
    void setUniformLocationsFromParentProperties();

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
    QList<int> propertiesWithoutNotifications;
    ShaderProgramPropertyListener* propertyListener;
};

#endif // SHADERPROGRAM_P_H
