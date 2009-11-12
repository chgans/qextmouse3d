/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qglmaterialparameters.h"
#include <QtCore/private/qobject_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLMaterialParameters
    \brief The QGLMaterialParameters class describes material properties for OpenGL drawing.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::painting
*/

class QGLMaterialParametersPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGLMaterialParameters)
public:
    explicit QGLMaterialParametersPrivate(int version = QObjectPrivateVersion);

    QColor ambientColor;
    QColor diffuseColor;
    QColor specularColor;
    QColor emittedLight;
    int shininess;
};

QGLMaterialParametersPrivate::QGLMaterialParametersPrivate(int version)
    : QObjectPrivate(version),
      specularColor(0, 0, 0, 255),
      emittedLight(0, 0, 0, 255),
      shininess(0)
{
    ambientColor.setRgbF(0.2f, 0.2f, 0.2f, 1.0f);
    diffuseColor.setRgbF(0.8f, 0.8f, 0.8f, 1.0f);
}

/*!
    Constructs a QGLMaterialParameters object with its default values,
    and attaches it to \a parent.
*/
QGLMaterialParameters::QGLMaterialParameters(QObject *parent)
    : QObject(*new QGLMaterialParametersPrivate(), parent)
{
}

/*!
    Destroys this QGLMaterialParameters object.
*/
QGLMaterialParameters::~QGLMaterialParameters()
{
}

/*!
    \property QGLMaterialParameters::ambientColor
    \brief the ambient color of the material.  The default value
    is (0.2, 0.2, 0.2, 1.0).

    \sa diffuseColor(), specularColor(), ambientColorChanged()
*/
QColor QGLMaterialParameters::ambientColor() const
{
    Q_D(const QGLMaterialParameters);
    return d->ambientColor;
}

void QGLMaterialParameters::setAmbientColor(const QColor& value)
{
    Q_D(QGLMaterialParameters);
    if (d->ambientColor != value) {
        d->ambientColor = value;
        emit ambientColorChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterialParameters::diffuseColor
    \brief the diffuse color of the material.  The default value
    is (0.8, 0.8, 0.8, 1.0).

    \sa ambientColor(), specularColor(), diffuseColorChanged()
*/
QColor QGLMaterialParameters::diffuseColor() const
{
    Q_D(const QGLMaterialParameters);
    return d->diffuseColor;
}

void QGLMaterialParameters::setDiffuseColor(const QColor& value)
{
    Q_D(QGLMaterialParameters);
    if (d->diffuseColor != value) {
        d->diffuseColor = value;
        emit diffuseColorChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterialParameters::specularColor
    \brief the specular color of the material.  The default value
    is (0, 0, 0, 1).

    \sa ambientColor(), diffuseColor(), specularColorChanged()
*/
QColor QGLMaterialParameters::specularColor() const
{
    Q_D(const QGLMaterialParameters);
    return d->specularColor;
}

void QGLMaterialParameters::setSpecularColor(const QColor& value)
{
    Q_D(QGLMaterialParameters);
    if (d->specularColor != value) {
        d->specularColor = value;
        emit specularColorChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterialParameters::emittedLight
    \brief the emitted light intensity of the material.
    The default value is (0.0, 0.0, 0.0, 1.0), which indicates
    that the material does not emit any light.

    \sa emittedLightChanged()
*/
QColor QGLMaterialParameters::emittedLight() const
{
    Q_D(const QGLMaterialParameters);
    return d->emittedLight;
}

void QGLMaterialParameters::setEmittedLight(const QColor& value)
{
    Q_D(QGLMaterialParameters);
    if (d->emittedLight != value) {
        d->emittedLight = value;
        emit emittedLightChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterialParameters::shininess
    \brief the specular exponent of the material, or how shiny it is.
    Must be between 0 and 128.  The default value is 0.  A value outside
    this range will be clamped to the range when the property is set.

    \sa shininessChanged()
*/
int QGLMaterialParameters::shininess() const
{
    Q_D(const QGLMaterialParameters);
    return d->shininess;
}

void QGLMaterialParameters::setShininess(int value)
{
    Q_D(QGLMaterialParameters);
    if (value < 0)
        value = 0;
    else if (value > 128)
        value = 128;
    if (d->shininess != value) {
        d->shininess = value;
        emit shininessChanged();
        emit materialChanged();
    }
}

/*!
    \fn void QGLMaterialParameters::ambientColorChanged()

    This signal is emitted when ambientColor() changes.

    \sa ambientColor(), setAmbientColor(), materialChanged()
*/

/*!
    \fn void QGLMaterialParameters::diffuseColorChanged()

    This signal is emitted when diffuseColor() changes.

    \sa diffuseColor(), setDiffuseColor(), materialChanged()
*/

/*!
    \fn void QGLMaterialParameters::specularColorChanged()

    This signal is emitted when specularColor() changes.

    \sa specularColor(), setSpecularColor(), materialChanged()
*/

/*!
    \fn void QGLMaterialParameters::emittedLightChanged()

    This signal is emitted when emittedLight() changes.

    \sa emittedLight(), setEmittedLight(), materialChanged()
*/

/*!
    \fn void QGLMaterialParameters::shininessChanged()

    This signal is emitted when shininess() changes.

    \sa shininess(), setShininess(), materialChanged()
*/

/*!
    \fn void QGLMaterialParameters::materialChanged()

    This signal is emitted when one of ambientColor(), diffuseColor(),
    specularColor(), emittedLight(), or shiniess() changes.

    \sa ambientColorChanged(), diffuseColorChanged()
    \sa specularColorChanged(), emittedLightChanged(), shininessChanged()
*/

/*!
    Returns true if this QGLMaterial parameters has the same properties
    as \a rhs.
*/
bool QGLMaterialParameters::operator==(const QGLMaterialParameters &rhs) const
{
    Q_D(const QGLMaterialParameters);
    return (d->ambientColor == rhs.d->ambientColor &&
            d->diffuseColor == rhs.d->diffuseColor &&
            d->specularColor == rhs.d->specularColor &&
            d->shininess == rhs.d->shininess);
}

QT_END_NAMESPACE
