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

#include "shaderwizardglwidget.h"
#include "qglview.h"
#include "qglteapot.h"
#include "qglsphere.h"
#include "qglbeziergeometry.h"
#include "qglvertexarray.h"
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
            , ambientMaterialColor()
            , diffuseMaterialColor()
            , specularMaterialColor()
            , shininess(0)
    {
    }
    void setEffect(QGLShaderProgramEffect *effect)
    {
        this->effect = effect;
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
        bezierModel(0)
        , bezierArray(0)
        , bezierRawData(0)
        , mSceneManager(0)
        , mDefaultSceneObject(0)
        , mSceneRoot(0)
        , mLightParameters(new QGLLightParameters(this))
        , mLightModel(new QGLLightModel(this))
{
    d = new ShaderWizardGLWidgetPrivate;
    setTeapotGeometry();
}

ShaderWizardGLWidget::~ShaderWizardGLWidget()
{

    if(bezierModel)
    {
        delete bezierModel;
        bezierModel = 0;
    }

    if(bezierArray)
    {
        delete bezierArray;
        bezierArray = 0;
    }

    if(bezierRawData)
    {
        free(bezierRawData);
        bezierRawData = 0;
    }
}

void ShaderWizardGLWidget::initializeGL(QGLPainter *painter)
{
    painter->setClearColor(QColor(0, 0, 0, 255));
    painter->setLightEnabled(0, true);
    d->painterColor = QColor(17, 68, 221, 255);

    mLightParameters = new QGLLightParameters;
    mLightParameters->setDirection(QVector3D(1.0,1.0,1.0).normalized());
    mLightParameters->setAmbientColor(QColor(255 *0.2, 255 *0.2, 255 *0.2));
    mLightParameters->setDiffuseColor(QColor(255 * 0.8, 255 * 0.8, 255 * 0.8));
    mLightParameters->setSpecularColor(QColor(255, 255, 255));
    painter->setLightParameters(0, mLightParameters);
    painter->setFaceColor(QGL::AllFaces, d->painterColor);

    QImage textureImage(":/qtlogo.png");
    texture.setImage(textureImage);

    painter->setDepthTestingEnabled(true);
    painter->setCullFaces(QGL::CullBackFaces);
}

void ShaderWizardGLWidget::paintGL(QGLPainter *painter)
{

    painter->setColor(d->painterColor); // Nokia blue
    painter->setClearColor(QColor(125,125,0,0));
//    painter->modelViewMatrix().push();
//    painter->modelViewMatrix().scale(1.5f);
//    painter->modelViewMatrix().rotate(45.0f, 1.0f, 1.0f, 1.0f);
//    painter->modelViewMatrix().translate(0.5f, 0.0f, -3.0f);

    QGLMaterialParameters defaultMaterial;
    painter->setFaceMaterial(QGL::FrontFaces, &defaultMaterial);

    if( d->effect && d->effect->isActive() )
        painter->setUserEffect(d->effect);

    if(mGeometry)
        mGeometry->draw(painter);

    //    painter->modelViewMatrix().pop();

    if (mSceneRoot)
        mSceneRoot->draw(painter);
    else if ( mDefaultSceneObject )
    {
        mDefaultSceneObject->draw(painter);
    }

    update();
}

void ShaderWizardGLWidget::setGeometry(QGLGeometry *newGeometry)
{
    mGeometry = newGeometry;
    if(newGeometry)
        setDefaultCamera(newGeometry);

    clearScene();
}

// TODO: Reset GL environment - set lights, camera etc. back to reasonable defaults
void ShaderWizardGLWidget::clearScene()
{
    mSceneManager = 0;
    mSceneRoot = 0;
    mDefaultSceneObject = 0;
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

void ShaderWizardGLWidget::populateBezierData(float* dataArray, int width, int height, float depth)
{
    Q_UNUSED(depth);
    qWarning() << "ShaderWizardGLWidget::populateBezierData() is not working yet";
    float *pointer= dataArray;

    if(width <= 1 || height <= 1)
    {
        return;
    }

    float xCoord, yCoord, zCoord;
    float xNormal, yNormal, zNormal;

    for(int y = 0; y < height - 1; y ++)
    {
        for(int x = 0; x < width - 1; x++)
        {
            // Calculate normal

            // Top left
            // pos
            xCoord = float(x) / float(width - 1) - 0.5;
            yCoord = float(y) / float(height - 1) - 0.5;
            zCoord = zFunc(xCoord, yCoord);
            *pointer++ = xCoord;
            *pointer++ = yCoord;
            *pointer++ = zCoord;

            // normal:
            calculateNormal(&xNormal, &yNormal, &zNormal, xCoord, yCoord, zCoord, 1.0/(width - 1.0));
            *pointer++ = xNormal;
            *pointer++ = yNormal;
            *pointer++ = zNormal;
            // texture coord:
            *pointer ++ = float(x) / float(width - 1);
            *pointer ++ = float(y) / float(height - 1);

            // Bottom left
            // pos
            xCoord = float(x) / float(width - 1) - 0.5;
            yCoord = float(y + 1) / float(height - 1) - 0.5;
            zCoord = zFunc(xCoord, yCoord);
            *pointer++ = xCoord;
            *pointer++ = yCoord;
            *pointer++ = zCoord;

            // normal:
            calculateNormal(&xNormal, &yNormal, &zNormal, xCoord, yCoord, zCoord, 1.0/(width - 1.0));
            *pointer++ = xNormal;
            *pointer++ = yNormal;
            *pointer++ = zNormal;
            // texture coord:
            *pointer ++ = float(x) / float(width - 1);
            *pointer ++ = float(y + 1) / float(height - 1);

            // Bottom Right
            // pos
            xCoord = float(x + 1) / float(width - 1) - 0.5;
            yCoord = float(y + 1) / float(height - 1) - 0.5;
            zCoord = zFunc(xCoord, yCoord);
            *pointer++ = xCoord;
            *pointer++ = yCoord;
            *pointer++ = zCoord;

            // normal:
            calculateNormal(&xNormal, &yNormal, &zNormal, xCoord, yCoord, zCoord, 1.0/(width - 1.0));
            *pointer++ = xNormal;
            *pointer++ = yNormal;
            *pointer++ = zNormal;
            // texture coord:
            *pointer ++ = float(x + 1) / float(width - 1);
            *pointer ++ = float(y + 1) / float(height - 1);

            // Top Right
            // pos
            xCoord = float(x + 1) / float(width - 1) - 0.5;
            yCoord = float(y) / float(height - 1) - 0.5;
            zCoord = zFunc(xCoord, yCoord);
            *pointer++ = xCoord;
            *pointer++ = yCoord;
            *pointer++ = zCoord;

            // normal:
            calculateNormal(&xNormal, &yNormal, &zNormal, xCoord, yCoord, zCoord, 1.0/(width - 1.0));
            *pointer++ = xNormal;
            *pointer++ = yNormal;
            *pointer++ = zNormal;
            // texture coord:
            *pointer ++ = float(x + 1) / float(width - 1);
            *pointer ++ = float(y) / float(height - 1);
        }
    }
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
//static float rawData[SURFACE_SIZE];

void ShaderWizardGLWidget::setSquareGeometry()
{
    static QGLHeightMap *square = new QGLHeightMap(101,101);
    setGeometry(square);
    d->effect = new QGLShaderProgramEffect(); // QGLPainter will delete the old one;
}

// Todo: actually use bezier geometry
void ShaderWizardGLWidget::setBezierGeometry()
{
    qWarning() << "ShaderWizardGLWidget::setBezierGeometry() - bezier geometry is not working yet";
    if(!bezierRawData)
    {
        bezierRawData = (float*)malloc(sizeof(float) * SURFACE_SIZE);
        populateBezierData(bezierRawData, SURFACE_WIDTH, SURFACE_HEIGHT, 1.0f);
    }

    static QGLGeometry *geom = new QGLGeometry();
    QGLVertexArray array
        (QGL::Position, 3, QGL::Normal, 3, QGL::TextureCoord0, 2);
//    array.setRawData(cubeVertices, 4 * (3 + 3 + 2));
    array.setRawData(bezierRawData, SURFACE_WIDTH * SURFACE_HEIGHT * 4 * (3 + 3 + 2));
    geom->setDrawingMode(QGL::Triangles);
    geom->setVertexArray(array);

    QGLIndexArray indices;
    int count = array.vertexCount();
    for (int index = 0; index < count; index += 4) {
        indices.append(index);
        indices.append(index + 1);
        indices.append(index + 2);

        indices.append(index);
        indices.append(index + 2);
        indices.append(index + 3);
    }
    geom->setIndexArray(indices);

    setGeometry(geom);
    return;
}

void ShaderWizardGLWidget::setCubeGeometry()
{
    static QGLCube *cube = new QGLCube;
    setGeometry(cube);
}

void ShaderWizardGLWidget::setSphereGeometry()
{
    static QGLSphere *sphere = new QGLSphere(2.0, QGLSphere::UVSphere, 150);
    setGeometry(sphere);
}

void ShaderWizardGLWidget::setTeapotGeometry()
{
    static QGLTeapot* teapot = new QGLTeapot;
    setGeometry(teapot);
}

void ShaderWizardGLWidget::setHeightMapGeometry()
{
    static RippleHeightMap *ripple = new RippleHeightMap(101,101);
    setGeometry(ripple);
}

void ShaderWizardGLWidget::setVertexShader(QString const &shader)
{
    if(!d->effect)
        d->effect = new QGLShaderProgramEffect(); // QGLPainter will delete the old one;
    d->effect->setVertexShader(shader);
    update();
}

void ShaderWizardGLWidget::setFragmentShader(QString const & shader )
{
    if(!d->effect)
        d->effect = new QGLShaderProgramEffect(); // QGLPainter will delete the old one;
    d->effect->setFragmentShader(shader);
    update();
}

void ShaderWizardGLWidget::setSceneManager(QObject* object)
{
    if(object == mSceneManager)
        return;

    mSceneManager = qobject_cast<QGLAbstractScene*>(object);
    mDefaultSceneObject = 0;
    mSceneRoot = 0;
    mGeometry = 0; // Don't show models and geometry at the same time

    // Get basic information for default object.
    mDefaultSceneObject = mSceneManager->defaultObject(QGLSceneObject::Main);

    setSceneObject(mDefaultSceneObject);

    // Point camera at the new model
    if (mSceneRoot && mSceneRoot->geometry())
    {
        setDefaultCamera(mSceneRoot->geometry());
    }
}

void ShaderWizardGLWidget::setSceneObject(QObject* object)
{
    // Get more information about more complex scenes if available
    mSceneRoot = qobject_cast<QGLSceneNode *>(object);
}

void ShaderWizardGLWidget::setDefaultCamera(QGLGeometry* geometry)
{
    QVector3D boxOrigin(0.0f, 0.0f, 0.0f);
    qreal viewDistance = 15.0; // default zoom if there's no reasonable box;
    qreal maxDimension = 2.0f;

    QBox3D box;
    if(geometry)
        box = geometry->boundingBox();

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
}

void ShaderWizardGLWidget::setPainterColor(QColor color)
{
    d->painterColor = color;
}

void ShaderWizardGLWidget::setAmbientLightColor(QColor color)
{
    d->ambientLightColor = color;
}

void ShaderWizardGLWidget::setDiffuseLightColor(QColor color)
{
    d->diffuseLightColor = color;
}

void ShaderWizardGLWidget::setSpecularLightColor(QColor color)
{
     d->specularLightColor = color;
}


void ShaderWizardGLWidget::setAmbientMaterialColor(QColor color)
{
     d->ambientMaterialColor = color;
}

void ShaderWizardGLWidget::setDiffuseMaterialColor(QColor color)
{
     d->diffuseMaterialColor = color;
}

void ShaderWizardGLWidget::setSpecularMaterialColor(QColor color)
{
     d->specularMaterialColor = color;
}

void ShaderWizardGLWidget::setMaterialShininess(int shininess)
{
     d->shininess = shininess;
}

QColor ShaderWizardGLWidget::painterColor() { return d->painterColor; }
QColor ShaderWizardGLWidget::ambientLightColor() { return d->ambientLightColor; }
QColor ShaderWizardGLWidget::diffuseLightColor() { return d->diffuseLightColor; }
QColor ShaderWizardGLWidget::specularLightColor() { return d->specularLightColor; }
QColor ShaderWizardGLWidget::ambientMaterialColor() { return d->ambientMaterialColor; }
QColor ShaderWizardGLWidget::diffuseMaterialColor() { return d->diffuseMaterialColor; }
QColor ShaderWizardGLWidget::specularMaterialColor() { return d->specularMaterialColor; }
int ShaderWizardGLWidget::materialShininess() { return d->shininess; }
