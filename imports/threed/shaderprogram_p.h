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

#ifndef SHADERPROGRAM_P_H
#define SHADERPROGRAM_P_H

#include <QWeakPointer>
#include <QGLShaderProgram>
#include "effect.h"
#include "qglshaderprogrameffect.h"
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
class ShaderProgramEffect : public QGLShaderProgramEffect
{
public:
    ShaderProgramEffect(ShaderProgram* parent);
    virtual ~ShaderProgramEffect();

    bool create(const QString& vertexShader, const QString& fragmentShader);

    void update(QGLPainter *painter, QGLPainter::Updates updates);
    bool setUniformForPropertyIndex(int propertyIndex, QGLPainter *painter);

    void setPropertiesDirty();
    void setPropertyDirty(int property);

    bool pixmapRequestFinished();
    void setAttributeFields(QGL::VertexAttribute fields);
protected:
    void processTextureUrl(int uniformLocation, QString urlString);
    void afterLink();

private:
    void setUniform(int uniformValue, const QImage& image,
                             QGLPainter* painter);
    void setUniform(int uniformValue, const QPixmap pixmap,
                             QGLPainter* painter);
    QGLTexture2D* textureForUniformValue(int uniformLocation);
    int textureUnitForUniformValue(int uniformLocation);

    QWeakPointer<ShaderProgram> parent;
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
