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
#include "qglmaterial.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDeclarative/qdeclarativeengine.h>
#include "private/qdeclarativepixmapcache_p.h"
#include <qdeclarativeinfo.h>

/*!
    \class Effect
    \brief The Effect class defines simple effects within the QML/3d environment.  Examples 
    of such effects include textures, simple material and lighting effects, and so on.
    \since 4.8
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

class EffectPrivate
{
public:
    EffectPrivate()
        : color(255, 255, 255, 255),
          useLighting(true),
          textureChanged(false),
          texture2D(0),
          material(0),
          pixmapCacheReply(0),
          pendingPixmapCache(false) {}
    ~EffectPrivate()
    {
        delete texture2D;
    }

    QColor color;
    bool useLighting;
    bool textureChanged;
    QImage texture;
    QGLTexture2D *texture2D;
    QUrl textureUrl;
    QGLMaterial *material;

    QDeclarativePixmapReply* pixmapCacheReply;
    bool pendingPixmapCache;
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
    \property Effect::useLighting
    \brief the lighting control parameter; true if this effect should
    use lighting or false if this effect should use flat colors and
    textures.

    The default value for this true.
*/
bool Effect::useLighting() const
{
    return d->useLighting;
}

void Effect::setUseLighting(bool value)
{
    d->useLighting = value;
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

    // got a new value, so abort any in-progress request
    cancelLoadingTexture();

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
            d->textureUrl = value;
            emit effectChanged();

            // Load texture from url
            QSize impsize;
            QString errorString;
            QPixmap pixmap;
            // async is true because we've already handled the syncronous case
            QDeclarativePixmapReply::Status status =
                    QDeclarativePixmapCache::get(d->textureUrl, &pixmap, &errorString, &impsize, true);
            if (status != QDeclarativePixmapReply::Ready && status !=
                QDeclarativePixmapReply::Error)
            {
                d->pixmapCacheReply =
                        QDeclarativePixmapCache::request(qmlEngine(this), d->textureUrl);
                d->pendingPixmapCache = true;
                connect(d->pixmapCacheReply, SIGNAL(finished()), this,
                        SLOT(textureRequestFinished()));
            } else {
                if(status == QDeclarativePixmapReply::Ready)
                {
                    setTextureImage(pixmap.toImage());
                } else {
                    qWarning() << "Error loading texture image: " << errorString;
                }
            }
        }
    }
}

void Effect::cancelLoadingTexture()
{
    if (d->pendingPixmapCache) {
        QDeclarativePixmapCache::cancel(d->textureUrl, this);
        d->pendingPixmapCache = false;
        // cancel invalidates our reply as well, so delete that
        if(d->pixmapCacheReply != 0)
        {
            d->pixmapCacheReply->deleteLater();
            d->pixmapCacheReply = 0;
        }
    }
}

/*!
    This function is used to emit the appropriate signal when the texture request has finished.

    \sa effectChanged()
*/
void Effect::textureRequestFinished()
{
    if(d->pixmapCacheReply && d->pixmapCacheReply->status() == QDeclarativePixmapReply::Ready)
    {
        QPixmap pixmap;
        QString errorString;
        QDeclarativePixmapReply::Status status = QDeclarativePixmapCache::get(d->textureUrl, &pixmap, &errorString);
        setTextureImage(pixmap.toImage());
        d->pixmapCacheReply = 0;
        d->pendingPixmapCache = false;
    }
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
    \c material property.  This specifies a set of \l QGLMaterial which
    are then used when creating the effect.
*/
QGLMaterial *Effect::material() const
{
    return d->material;
}

void Effect::setMaterial(QGLMaterial *value)
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
    Enable the effect on for a given \a painter.
*/
void Effect::enableEffect(QGLPainter *painter)
{
    QGLTexture2D *tex = texture2D();
    if (tex == NULL && d->material)
        tex = d->material->texture();
    if (d->useLighting) {
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
}

/*!
    Disable the effect for a given \a painter.
*/
void Effect::disableEffect(QGLPainter *painter)
{
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(Qt::white);
    painter->setTexture((QGLTexture2D *)0);
}

/*! \fn void Effect::effectChanged();

  Signals that a preoperty of the Effect has changed in some way, this may be a texture, material,
  or other parameter.
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
