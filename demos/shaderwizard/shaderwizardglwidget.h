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

#ifndef SHADERWIZARDGLWIDGET_H
#define SHADERWIZARDGLWIDGET_H

#include <QGLWidget>
class QGLPainter;

#include "qglview.h"
#include "qglbuilder.h"
#include "qgltexture2d.h"

class ShaderWizardGLWidgetPrivate;
class QGLAbstractScene;
class QGLSceneNode;
class QGLSceneNode;
class QGLHeightMap;
class QGLSphere;
class QGLShaderProgramEffect;

class ShaderWizardGLWidget : public QGLView
{
    Q_OBJECT
    Q_PROPERTY(QColor painterColor READ painterColor WRITE setPainterColor NOTIFY effectChanged)
    Q_PROPERTY(QColor ambientLightColor READ ambientLightColor WRITE setAmbientLightColor NOTIFY effectChanged)
    Q_PROPERTY(QColor diffuseLightColor READ diffuseLightColor WRITE setDiffuseLightColor NOTIFY effectChanged)
    Q_PROPERTY(QColor specularLightColor READ specularLightColor WRITE setSpecularLightColor NOTIFY effectChanged)
    Q_PROPERTY(QColor ambientMaterialColor READ ambientMaterialColor WRITE setAmbientMaterialColor NOTIFY effectChanged)
    Q_PROPERTY(QColor diffuseMaterialColor READ diffuseMaterialColor WRITE setDiffuseMaterialColor NOTIFY effectChanged)
    Q_PROPERTY(QColor specularMaterialColor READ specularMaterialColor WRITE setSpecularMaterialColor NOTIFY effectChanged)
    Q_PROPERTY(int materialShininess READ materialShininess WRITE setMaterialShininess NOTIFY effectChanged)

public:
    ShaderWizardGLWidget();
    ~ShaderWizardGLWidget();

    QColor painterColor();
    QColor ambientLightColor();
    QColor diffuseLightColor();
    QColor specularLightColor();

    QColor ambientMaterialColor();
    QColor diffuseMaterialColor();
    QColor specularMaterialColor();

    int materialShininess();
    QGLShaderProgramEffect* effect();

public slots:
    void setSceneNode(QGLSceneNode *geom);

    void setCubeGeometry();
    void setSphereGeometry();
    void setTeapotGeometry();
    void setSquareGeometry();
    void setHeightMapGeometry();

    void setVertexShader(QString const &  shader );
    void setFragmentShader(QString const & shader );
    void setSceneManager(QObject* object);
    void setSceneObject(QObject* object);
    void setPainterColor(QColor color);

    void setAmbientLightColor(QColor color);
    void setDiffuseLightColor(QColor color);
    void setSpecularLightColor(QColor color);

    void setAmbientMaterialColor( QColor color);
    void setDiffuseMaterialColor( QColor color);
    void setSpecularMaterialColor ( QColor color );
    void setMaterialShininess(int shininess);
    void setTextureImage(const QImage &image);

    void clearScene();
    void setEffect(QGLShaderProgramEffect* effect);

signals:
    void effectChanged();

private:
    void initializeGL(QGLPainter *painter);
    void paintGL(QGLPainter *painter);

    void setDefaultCamera(QGLSceneNode* geometry);

    ShaderWizardGLWidgetPrivate* d;
    QGLSceneNode *mSceneNode;
    QGLAbstractScene *mSceneManager;
    QGLSceneNode *mDefaultSceneObject;
    QGLSceneNode *mSceneRoot;
    QGLLightParameters *mLightParameters;
    QGLLightModel *mLightModel;
    QGLMaterial *mMaterial;
    QGLMaterialCollection *mMaterialCollection;
    QGLTexture2D *mTexture;

    // Models
    QGLSceneNode* cube;
    QGLSceneNode* teapot;
    QGLSceneNode* square;
    QGLSceneNode* ripple;
    QGLSceneNode* sphere;
};

#endif // SHADERWIZARDGLWIDGET_H
