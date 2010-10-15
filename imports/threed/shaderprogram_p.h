#ifndef SHADERPROGRAM_P_H
#define SHADERPROGRAM_P_H

#include <QWeakPointer>
#include <QGLShaderProgram>
#include "effect.h"
#include "qglabstracteffect.h"
#include "private/qdeclarativepixmapcache_p.h"

class ShaderProgram;

class ShaderProgramEffect;
class QDeclarativePixmapReply;

/*!
  \internal
    This class's purpose is to be a proxy for signals for the main shader
    program.

    This is necessary so the qt_metacall trick can be used to tell tell apart
    the update signals from QML generated properties.  (In short, each update
    signal is connected to an imaginary slot, but calling the slot is
    intercepted and replaced by a call to the appropriate update function).
*/
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
signals:
    void propertyChanged();
};

/*!
  \internal
  \sa ShaderProgramPropertyListener
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


/*!
  \internal
  The ShaderProgramEffect class underlies the ShaderProgram class in Qml/3d.
  It contains the actual QGLShaderProgram along with all of the necessary
  parameters to use that program.
*/
class ShaderProgramEffect : public QGLAbstractEffect
{
public:
    ShaderProgramEffect(ShaderProgram* parent);
    virtual ~ShaderProgramEffect();

    bool create(const QString& vertexShader, const QString& fragmentShader);

    void setActive(QGLPainter *painter, bool flag);

    void update(QGLPainter *painter, QGLPainter::Updates updates);
    bool setUniformForPropertyIndex(int propertyIndex, QGLPainter *painter);


    void setVertexAttribute
        (QGL::VertexAttribute attribute, const QGLAttributeValue& value);

    void setPropertiesDirty();
    void setPropertyDirty(int property);

    bool pixmapRequestFinished();
    void setAttributeFields(QGL::VertexAttribute fields);
protected:
    void processTextureUrl(int uniformLocation, QString urlString);

private:
    void setUniformLocationsFromParentProperties();
    void setUniform(int uniformValue, const QImage& image,
                             QGLPainter* painter);
    void setUniform(int uniformValue, const QPixmap pixmap,
                             QGLPainter* painter);
    QGLTexture2D* textureForUniformValue(int uniformLocation);
    int textureUnitForUniformValue(int uniformLocation);

    QWeakPointer<ShaderProgram> parent;
    QGLShaderProgram *program;
    int vertexAttr;
    int normalAttr;
    int colorAttr;
    int texCoord0Attr;
    int texCoord1Attr;
    int texCoord2Attr;
    int customVertex0Attr;
    int customVertex1Attr;
    int matrixUniform;
    int modelViewMatrixUniform;
    int normalMatrixUniform;
    int texture0;
    int colorUniform;
    int lightPositionUniform;
    int spotDirectionUniform;
    int nextTextureUnit;
    QMap<int, int> propertyIdsToUniformLocations;
    QMap<int, int> uniformLocationsToTextureUnits;
    QList<int> dirtyProperties;
    QArray<int> propertiesWithoutNotificationSignal;
    ShaderProgramPropertyListener* propertyListener;

    // Thes maps are all referenced by uniform location
    QMap<int, QGLTexture2D*> texture2Ds;
    QMap<int, QDeclarativePixmap*> declarativePixmaps;
    QMap<int, QString> urls;

    // These are sets of uniform locations
    QSet<int> loadingTextures;
    QSet<int> changedTextures;
};

#endif // SHADERPROGRAM_P_H
