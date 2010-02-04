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

#include "effect.h"
#include "qglpainter.h"
#include "qglmaterialparameters.h"
#include "qglfogparameters.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDeclarative/qmlengine.h>

/*!
    \class Effect
    \brief The Effect class defines simple effects within the QML/3d environment.  Examples 
    of such effects include fog effects, simple material and lighting effects, and so on.
    \since 4.6.?
    \ingroup qt3d
    \ingroup qt3d::qml3d

    \section1 Defining an Effect

    Effects can be defined within QML using the normal syntax for creating any QML/3d object.  To create a
    default effect with no extra information, for example, we can simply using the following:

    \code
    Effect {}
    \endcode

    More complex effects use the usual QML syntax for accessing and updating properties.  In order to specify
    a texture, for example, the following could be used:

    \code
    Effect {
        id: myTextureEffect
        texture: "C:\textures\texture.png"
    }
    \endcode
*/

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,Effect,Effect)
QML_DEFINE_TYPE(Qt,4,6,Material,QGLMaterialParameters)
QML_DEFINE_TYPE(Qt,4,6,Fog,QGLFogParameters)

class EffectPrivate
{
public:
    EffectPrivate()
        : color(255, 255, 255, 255),
          textureChanged(false),
          texture2D(0),
          material(0),
          fog(0),
          textureReply(0) {}
    ~EffectPrivate()
    {
        delete texture2D;
    }

    QColor color;
    bool textureChanged;
    QImage texture;
    QGLTexture2D *texture2D;
    QUrl textureUrl;
    QGLMaterialParameters *material;
    QGLFogParameters *fog;
    QNetworkReply *textureReply;
};

/*!
    Constructs the Effect object with \a parent as its parent.
*/
Effect::Effect(QObject *parent)
    : QObject(parent)
{
    d = new EffectPrivate;
}

/*!
    Destroy the \l Effect object and delete any unneeded data.
*/
Effect::~Effect()
{
    delete d;
}

/*!
    \property Effect::color
    \brief The color of the object defined by a QColor object.

    The default value for this property is \c 255,255,255,255
*/
QColor Effect::color() const
{
    return d->color;
}

void Effect::setColor(const QColor& value)
{
    d->color = value;
    emit effectChanged();
}

/*!
    \property Effect::texture
    \brief Texture effects are defined by this property.  A texture is 
    provided by means of a \l QUrl which is then used for texturing.

    Textures can also be defined directly as images using the \l textureImage
    property.

    \sa textureImage()
*/
QUrl Effect::texture() const
{
    return d->textureUrl;
}

void Effect::setTexture(const QUrl& value)
{
    if (d->textureUrl == value)
        return;
    d->textureUrl = value;

    if (d->textureUrl.isEmpty()) {
        d->texture = QImage();
        d->textureChanged = true;
    } else {

//#define QT_NO_LOCALFILE_OPTIMIZED_QML
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
        if (d->textureUrl.scheme() == QLatin1String("file")) {
            QString fileName = value.path();
            d->texture = QImage(fileName);
            d->textureChanged = true;
            emit effectChanged();
        } else
#endif
        {
            if (d->textureReply)
                d->textureReply->deleteLater();
            QNetworkRequest req(d->textureUrl);
            req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
            d->textureReply = qmlEngine(this)->networkAccessManager()->get(req);
            QObject::connect(d->textureReply, SIGNAL(finished()),
                             this, SLOT(textureRequestFinished()));
        }
    }
}

/*!
    This function is used to emit the appropriate signal when the texture request has finished.

    \sa effectChanged()
*/
void Effect::textureRequestFinished()
{
    d->texture.load(d->textureReply,0);
    d->textureChanged = true;
    d->textureReply->deleteLater();
    d->textureReply = 0;
    emit effectChanged();
}

/*!
    \property Effect::textureImage
    \brief This property directly defines a texture using an existing \l QImage.

    Textures can also be defined based on a URL using the \l texture property.

    \sa texture()
*/
QImage Effect::textureImage() const
{
    return d->texture;
}

void Effect::setTextureImage(const QImage& value)
{
    d->texture = value;
    d->textureChanged = true;
    emit effectChanged();
}

/*!
    \property Effect::material
    \brief Material parameters are defined in an \l Effect in QML/3d via the
    \c material property.  This specifies a set of \l QGLMaterialParameters which
    are then used when creating the effect.
*/
QGLMaterialParameters *Effect::material() const
{
    return d->material;
}

void Effect::setMaterial(QGLMaterialParameters *value)
{
    if (d->material != value) {
        if (d->material) {
            disconnect(d->material, SIGNAL(materialChanged()),
                       this, SIGNAL(effectChanged()));
        }
        d->material = value;
        if (d->material) {
            connect(d->material, SIGNAL(materialChanged()),
                    this, SIGNAL(effectChanged()));
        }
        emit effectChanged();
    }
}

/*!
    \property Effect::fog
    \brief fog effects can be achieved using the \c fog property of the \l Effect class.

    They are specified using the standard \l QGLFogParameters class.
*/
QGLFogParameters *Effect::fog() const
{
    return d->fog;
}

void Effect::setFog(QGLFogParameters *value)
{
    if (d->fog != value) {
        if (d->fog) {
            disconnect(d->fog, SIGNAL(fogChanged()),
                       this, SIGNAL(effectChanged()));
        }
        d->fog = value;
        if (d->fog) {
            connect(d->fog, SIGNAL(fogChanged()),
                    this, SIGNAL(effectChanged()));
        }
        emit effectChanged();
    }
}

/*!
    Enable the effect on for a given \a painter.
*/
void Effect::enableEffect(QGLPainter *painter)
{
    QGLTexture2D *tex = texture2D();
    if (painter->hasEnabledLights()) {
        if (tex && !tex->isNull()) {
            painter->setStandardEffect(QGL::LitDecalTexture2D);
            painter->setTexture(tex);
        } else {
            painter->setStandardEffect(QGL::LitMaterial);
        }
        if (d->material)
            painter->setFaceMaterial(QGL::AllFaces, d->material);
        else
            painter->setFaceColor(QGL::AllFaces, d->color);
    } else {
        if (tex && !tex->isNull()) {
            painter->setStandardEffect(QGL::FlatDecalTexture2D);
            painter->setTexture(tex);
            painter->setColor(d->color);
        } else {
            painter->setStandardEffect(QGL::FlatColor);
            painter->setColor(d->color);
        }
    }
    painter->setFogParameters(d->fog);
}

/*!
    Disable the effect for a given \a painter.
*/
void Effect::disableEffect(QGLPainter *painter)
{
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(Qt::white);
    painter->setTexture((QGLTexture2D *)0);
    painter->setFogParameters(0);
}

/*! \fn void Effect::effectChanged();

  Signals that a preoperty of the Effect has changed in some way, this may be a texture, material,
  fog, or other parameter.
*/

/*!
    This function returns a \l QGLTexture2D based on the \c texture property of the \l Effect.
*/
QGLTexture2D *Effect::texture2D()
{
    if (d->textureChanged) {
        delete d->texture2D;
        QGLTexture2D *newtex = new QGLTexture2D();
        if (!d->texture.isNull())
            newtex->setImage(d->texture);
        d->texture2D = newtex;
        d->textureChanged = false;
    }
    return d->texture2D;
}

QT_END_NAMESPACE
