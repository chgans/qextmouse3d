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

#include "shaderwizardglwidget.h"
#include "qglview.h"
#include "qglcube.h"
#include "qglteapot.h"
#include "qglsphere.h"
#include <math.h>
#include <QVector3D>
#include "qglheightmap.h"
#include "rippleheightmap.h"
#include "qglabstracteffect.h"
#include "qglshaderprogrameffect.h"
#include <QGLShader>
#include <QtCore/qmath.h>
#include "qglabstractscene.h"
#include "qglscenenode.h"
#include "qglmaterial.h"
#include "qglmaterialcollection.h"
#include "qglcolladafxeffect.h"
#include "qglcolladafxeffectfactory.h"

class ShaderWizardGLWidgetPrivate
{
public:
    ShaderWizardGLWidgetPrivate() : effect(0)
            , fragmentShader("")
            , vertexShader("")
            , painterColor()
            , ambientLightColor()
            , diffuseLightColor()
            , specularLightColor()
    {
    }

    QGLShaderProgramEffect *effect;
    QString fragmentShader;
    QString vertexShader;

    QColor painterColor;
    QColor ambientLightColor;
    QColor diffuseLightColor;
    QColor specularLightColor;
    QColor ambientMaterialColor;
    QColor diffuseMaterialColor;
    QColor specularMaterialColor;
    int shininess;
};

ShaderWizardGLWidget::ShaderWizardGLWidget() :
        mSceneNode(0)
        , mSceneManager(0)
        , mDefaultSceneObject(0)
        , mSceneRoot(0)
        , mLightParameters(new QGLLightParameters(this))
        , mLightModel(new QGLLightModel(this))
        , mMaterial(new QGLMaterial())
        , mMaterialCollection(new QGLMaterialCollection(this))
        , mTexture(new QGLTexture2D())
        , cube(0)
        , teapot(0)
        , square(0)
        , ripple(0)
        , sphere(0)
{
    d = new ShaderWizardGLWidgetPrivate;

    // The defaults for specular color and shininess (black and 0) are ugly
    // so use white and 64 for a reasonable default specular effect
    setSpecularMaterialColor(QColor(255, 255, 255, 255));
    setMaterialShininess(64);
    mMaterial->setObjectName("ShaderWizardGLWidgetMaterial");

    setTeapotGeometry();
    d->effect = new QGLShaderProgramEffect;
}

ShaderWizardGLWidget::~ShaderWizardGLWidget()
{
    if( square )
    {
        delete square;
        square = 0;
    }

    if( ripple )
    {
        delete ripple;
        ripple = 0;
    }

    if( sphere )
    {
        delete sphere;
        sphere = 0;
    }

    if( mMaterialCollection)
    {
        delete mMaterialCollection;
        mMaterialCollection = 0;
    }
}

void ShaderWizardGLWidget::initializeGL(QGLPainter *painter)
{
    painter->setClearColor(QColor(0, 0, 0, 255));
    d->painterColor = QColor(17, 68, 221, 255);

    mLightParameters = new QGLLightParameters;
    mLightParameters->setDirection(QVector3D(1.0,1.0,1.0).normalized());
    mLightParameters->setAmbientColor(QColor(255 *0.2, 255 *0.2, 255 *0.2));
    mLightParameters->setDiffuseColor(QColor(255 * 0.8, 255 * 0.8, 255 * 0.8));
    mLightParameters->setSpecularColor(QColor(255, 255, 255));
    painter->setMainLight(mLightParameters);
    painter->setFaceColor(QGL::AllFaces, d->painterColor);

    mTexture->setImage(QImage (":/qtlogo.png"));

    painter->setDepthTestingEnabled(true);
    painter->setCullFaces(QGL::CullBackFaces);

    if(d->effect)
        d->effect->setActive(painter, true);
}

void ShaderWizardGLWidget::paintGL(QGLPainter *painter)
{
    painter->setColor(d->painterColor);
    painter->setClearColor(QColor(32, 32, 32, 0));
//    painter->modelViewMatrix().push();
//    painter->modelViewMatrix().scale(1.5f);
//    painter->modelViewMatrix().rotate(45.0f, 1.0f, 1.0f, 1.0f);
//    painter->modelViewMatrix().translate(0.5f, 0.0f, -3.0f);

    painter->setTexture(0, mTexture);
    painter->setFaceMaterial(QGL::FrontFaces, mMaterial);


    if( d->effect )
    {
        painter->setUserEffect(d->effect);
    }

    if(mSceneNode)
        mSceneNode->draw(painter);

//    painter->modelViewMatrix().pop();

    if (mSceneRoot)
        mSceneRoot->draw(painter);
    else if ( mDefaultSceneObject )
    {
        mDefaultSceneObject->draw(painter);
    }
}

void ShaderWizardGLWidget::setSceneNode(QGLSceneNode *newNode)
{
    mSceneNode = newNode;

    setDefaultCamera(mSceneNode);

    if( mSceneNode )
    {
        if(!mSceneNode->palette())
            mSceneNode->setPalette(mMaterialCollection);
        mMaterialCollection->setParent(mSceneNode);
        int materialIndex = mSceneNode->palette()->indexOf("ShaderWizardGLWidgetMaterial");
        if(materialIndex == -1)
            materialIndex = mSceneNode->palette()->addMaterial(mMaterial);
        mSceneNode->setMaterial(materialIndex);
    }

    clearScene();
    update();
}

// TODO: Reset GL environment - set lights, camera etc. back to reasonable defaults
void ShaderWizardGLWidget::clearScene()
{
    mSceneManager = 0;
    mSceneRoot = 0;
    mDefaultSceneObject = 0;
    update();
}

void ShaderWizardGLWidget::setEffect(QGLShaderProgramEffect *effect)
{
    d->effect = effect;
}

static float zFunc(float x, float y)
{
    return 0.1f * sin((3.0 * x * M_PI * 2.0) + sin(3.0 * y * M_PI * 2.0));

}

void calculateNormal(float *xNormal, float *yNormal, float *zNormal, float xCoord, float yCoord, float zCoord, float edgeLength)
{
    Q_UNUSED(zCoord);
    // Average the normal of the 4 adjacent faces.
    // It doesn't use the actual faces on the mesh, but it should be a good approximation
    QVector3D cumulativeNormal;

    float myZ = zFunc(xCoord, yCoord);

    // These are the vectors from the vertex in question to it's adjacent vertices,
    // so "up" is the distance to the next vertex in the +y direction -
    // (equivalent to that vertex minus "this" one)
    QVector3D up(0, edgeLength, zFunc(xCoord - edgeLength, yCoord + edgeLength) - myZ);
    QVector3D left(-edgeLength, 0, zFunc(xCoord - edgeLength, yCoord) - myZ);
    QVector3D down(0, -edgeLength, zFunc(xCoord, yCoord - edgeLength) - myZ);
    QVector3D right(edgeLength, 0, zFunc(xCoord + edgeLength, yCoord) - myZ);

    cumulativeNormal += QVector3D::crossProduct(up, left);
    cumulativeNormal += QVector3D::crossProduct(left, down);
    cumulativeNormal += QVector3D::crossProduct(down, right);
    cumulativeNormal += QVector3D::crossProduct(right, up);

    cumulativeNormal.normalize();
    *xNormal = cumulativeNormal.x();
    *yNormal = cumulativeNormal.y();
    *zNormal = cumulativeNormal.z();
}



#define QGL_CUBE_SIZE (6 * 4 * (3 + 3 + 2))
static float const cubeVertices[QGL_CUBE_SIZE] = {
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f
};
#define SURFACE_WIDTH 100
#define SURFACE_HEIGHT 100
#define SURFACE_SIZE ((SURFACE_WIDTH * SURFACE_HEIGHT) * (4 * (3 + 3 + 2)))

void ShaderWizardGLWidget::setSquareGeometry()
{
    if (square == 0) {
        square = new QGLDisplayList(this);
        *square << QGLHeightMap(101, 101);
        square->finalize();
    }
    setSceneNode(square);
}

void ShaderWizardGLWidget::setCubeGeometry()
{
    if (cube == 0) {
        cube = new QGLDisplayList(this);
        cube->newSection(QGL::Faceted);
        *cube << QGLCube();
        cube->finalize();
    }
    setSceneNode(cube);
}

void ShaderWizardGLWidget::setSphereGeometry()
{
    if (sphere == 0) {
        sphere = new QGLDisplayList(this);
        *sphere << QGLSphere(2.0, 3);
        sphere->finalize();
    }
    setSceneNode(sphere);
}

void ShaderWizardGLWidget::setTeapotGeometry()
{
    if (teapot == 0) {
        teapot = new QGLDisplayList(this);
        *teapot << QGLTeapot();
        teapot->finalize();
    }
    setSceneNode(teapot);
}

void ShaderWizardGLWidget::setHeightMapGeometry()
{
    if (ripple == 0) {
        ripple = new QGLDisplayList(this);
        *ripple << RippleHeightMap(101,101);
        ripple->finalize();
    }
    setSceneNode(ripple);
}

void ShaderWizardGLWidget::setVertexShader(QString const &shader)
{
    if(!d->effect)
        d->effect = new QGLShaderProgramEffect(); // QGLPainter will delete the old one;
    d->effect->setVertexShader(shader);
    update();
    emit vertexShaderChanged(shader);
}

void ShaderWizardGLWidget::setFragmentShader(QString const & shader )
{
    if(!d->effect)
        d->effect = new QGLShaderProgramEffect(); // QGLPainter will delete the old one;

    d->effect->setFragmentShader(shader);
    update();
    emit fragmentShaderChanged(shader);
}

void ShaderWizardGLWidget::setSceneManager(QObject* object)
{
    if(object == mSceneManager)
        return;

    mSceneManager = qobject_cast<QGLAbstractScene*>(object);
    mDefaultSceneObject = 0;
    mSceneRoot = 0;
    mSceneNode = 0; // Don't show models and geometry at the same time

    // Get basic information for default object.
    mDefaultSceneObject = mSceneManager->defaultObject(QGLSceneObject::Main);

    setSceneObject(mDefaultSceneObject);

    // Point camera at the new model
    if (mSceneRoot && mSceneRoot->geometry())
    {
        setDefaultCamera(mSceneRoot);
    }
    update();
}

void ShaderWizardGLWidget::setSceneObject(QObject* object)
{
    // Get more information about more complex scenes if available
    mSceneRoot = qobject_cast<QGLSceneNode *>(object);
    update();
}

void ShaderWizardGLWidget::setDefaultCamera(QGLSceneNode* sceneNode)
{
    QVector3D boxOrigin(0.0f, 0.0f, 0.0f);
    qreal viewDistance = 15.0; // default zoom if there's no reasonable box;
    qreal maxDimension = 2.0f;

    QBox3D box;
    if(sceneNode && sceneNode->geometry())
    {
        box = sceneNode->geometry()->boundingBox();
    }

    if (!box.isNull())
    {
        boxOrigin = box.center();
        if (box.isFinite())
        {
            QVector3D ext = box.size();
            maxDimension = qMax(ext.x(), qMax(ext.y(), ext.z()));
            viewDistance = viewDistance + maxDimension;
        }

    }

    this->camera()->setEye(QVector3D(0.0, 0.0, viewDistance));
    this->camera()->setCenter(boxOrigin);
    update();
}

void ShaderWizardGLWidget::setPainterColor(QColor color)
{
    d->painterColor = color;
    update();
}

void ShaderWizardGLWidget::setAmbientLightColor(QColor color)
{
    d->ambientLightColor = color;
    update();
}

void ShaderWizardGLWidget::setDiffuseLightColor(QColor color)
{
    d->diffuseLightColor = color;
    update();
}

void ShaderWizardGLWidget::setSpecularLightColor(QColor color)
{
     d->specularLightColor = color;
    update();
}


void ShaderWizardGLWidget::setAmbientMaterialColor(QColor color)
{
     mMaterial->setAmbientColor(color);
    update();
}

void ShaderWizardGLWidget::setDiffuseMaterialColor(QColor color)
{
     mMaterial->setDiffuseColor(color);
    update();
}

void ShaderWizardGLWidget::setSpecularMaterialColor(QColor color)
{
     mMaterial->setSpecularColor(color);
    update();
}

void ShaderWizardGLWidget::setMaterialShininess(int shininess)
{
     mMaterial->setShininess(shininess);
    update();
}

void ShaderWizardGLWidget::setTextureImage(const QImage &image)
{
   mTexture->setImage(image);
}

QColor ShaderWizardGLWidget::painterColor() { return d->painterColor; }
QColor ShaderWizardGLWidget::ambientLightColor() { return d->ambientLightColor; }
QColor ShaderWizardGLWidget::diffuseLightColor() { return d->diffuseLightColor; }
QColor ShaderWizardGLWidget::specularLightColor() { return d->specularLightColor; }
QColor ShaderWizardGLWidget::ambientMaterialColor() { return mMaterial->ambientColor(); }
QColor ShaderWizardGLWidget::diffuseMaterialColor() { return mMaterial->diffuseColor(); }
QColor ShaderWizardGLWidget::specularMaterialColor() { return mMaterial->specularColor(); }
int ShaderWizardGLWidget::materialShininess() { return mMaterial->shininess(); }
QGLShaderProgramEffect* ShaderWizardGLWidget::effect() { return d->effect; }
