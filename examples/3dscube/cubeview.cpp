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

#include "cubeview.h"

#include <QTimer>
#include <QAction>
#include <QSlider>
#include <QMenu>
#include <QMessageBox>
#include <QColorDialog>

#include <math.h>

 #ifndef M_PI
 #define M_PI 3.14159265358979323846
 #endif


#include "qglabstractscene.h"
#include "qglscenenode.h"
#include "qglpainter.h"
#include "qglcamera.h"
#include "qglgeometry.h"
#include "qglmaterialparameters.h"
#include "qglmaterialcollection.h"
#include "qline3d.h"

CubeView::CubeView(QWidget *parent)
    : QGLWidget(parent)
    , mTimer(new QTimer(this))
    , mColorTimer(new QTimer(this))
    , mSpin(0)
    , mXTilt(270)
    , mYTilt(0)
    , mZTilt(0)
    , mZoom(15)
    , mYax(0)
    , mSceneManager(0)
    , mSceneRoot(0)
    , mSceneInitialized(false)
    , mLightParameters(new QGLLightParameters(this))
    , mLightModel(new QGLLightModel(this))
    , mSelectColor(QColor(Qt::red))
    , mColorMenu(0)
    , mComponentMenu(0)
    , mSelectMaterial(-1)
    , mCurrentSelected(0)
    , mSelectColorAnimate(0)
    , mAnimationEnabled(true)
    , mWarningDisplayed(false)
{
}

CubeView::~CubeView()
{
    delete mTimer;
    delete mSceneManager;
}

void CubeView::load()
{
    QAction *act = qobject_cast<QAction*>(sender());
    openModelFile(act->text());
}

void CubeView::openModelFile(const QString &fileName)
{
    mWarningDisplayed = false;
    importModel(fileName);
    loadColors();
    loadComponents();
    emit modelLoaded(fileName);
}

void CubeView::importModel(const QString &name)
{
    delete mSceneManager;
    mSceneManager = 0;
    mSceneRoot = 0;
    mSelectMaterial = -1;
    mCurrentModelName = name;
    mSceneManager = QGLAbstractScene::loadScene(mCurrentModelName);
    if (!mSceneManager)
        return;
    QGLSceneObject *obj = mSceneManager->defaultObject(QGLSceneObject::Main);
    mSceneInitialized = false;
    mSceneRoot = qobject_cast<QGLSceneNode *>(obj);
}

void CubeView::loadColors()
{
    if (mColorMenu->actions().count())
        mColorMenu->clear();
    QGLMaterialCollection *materials = 0;
    if (mSceneRoot && mSceneRoot->geometry())
        materials = mSceneRoot->geometry()->palette();
    QStringList names;
    if (materials)
    {
        for (int i = 0; i < materials->size(); ++i)
        {
            QString name = materials->materialName(i);
            if (!name.isEmpty())
                names.append(name);
        }
    }
    if (names.count() == 0)
    {
        QAction *act = new QAction(tr("No materials"), this);
        act->setDisabled(true);
        mColorMenu->addAction(act);
        return;
    }
    for (int index = 0; index < materials->size(); ++index)
    {
        QGLMaterialParameters *mat = materials->materialByIndex(index);
        QPixmap px(16, 16);
        px.fill(mat->diffuseColor());
        QAction *act = new QAction(QIcon(px), materials->materialName(index), this);
        mColorMenu->addAction(act);
        QObject::connect(act, SIGNAL(triggered()), this, SLOT(changeColor()));
    }
}

void CubeView::changeColor()
{
    if (!mSceneRoot)
        return;
    QAction *act = qobject_cast<QAction*>(sender());
    if (!act)
        return;
    QGLMaterialCollection *materials = mSceneRoot->geometry()->palette();
    int index = materials->materialIndexByName(act->text());
    if (index == -1)
    {
        QMessageBox::warning(this, "Material Invalid", tr("Material %1 not found").arg(act->text()));
        return;
    }
    QGLMaterialParameters *mat = materials->materialByIndex(index);
    QColor color;
    if (mat)
        color = QColorDialog::getColor(mat->diffuseColor(), this);
    if (!color.isValid())
        return;
    mat->setDiffuseColor(color.darker(125));
    mat->setAmbientColor(color.darker(500));
    mat->setSpecularColor(color.lighter(125));
    QPixmap px(16, 16);
    px.fill(color);
    act->setIcon(QIcon(px));
    updateGL();
}

void CubeView::loadComponents()
{
    QList<QGLSceneObject*> components;
    if (mSceneManager)
        components = mSceneManager->objects(QGLSceneObject::Mesh);
    mComponentMenu->clear();
    if (components.count() == 0)
    {
        QAction *act = new QAction(tr("No components"), this);
        act->setDisabled(true);
        mComponentMenu->addAction(act);
        return;
    }
    QAction *selectNoneAct = new QAction(tr("Select None"), this);
    mComponentMenu->addAction(selectNoneAct);
    QObject::connect(selectNoneAct, SIGNAL(triggered()), this, SLOT(selectComponent()));
    mComponentMenu->addSeparator();
    QList<QGLSceneObject*>::const_iterator it(components.begin());
    for ( ; it != components.end(); ++it)
    {
        QGLSceneNode *nodeObj = qobject_cast<QGLSceneNode *>(*it);
        if (!nodeObj)
        {
            qWarning("Could not make scene node from scene object %s",
                     qPrintable((*it)->objectName()));
            continue;
        }
        QGLGeometry *mesh = nodeObj->geometry();
        if (!mesh)
            continue;
        QString meshName = mesh->objectName();
        QAction *act = new QAction(meshName, this);
        mComponentMenu->addAction(act);
        QObject::connect(act, SIGNAL(triggered()), this, SLOT(selectComponent()));
    }
    mCurrentSelected = 0;
    makeSelectColor(mSelectColor);
}

void CubeView::restoreMaterial(QGLSceneNode *root)
{
    // recurse down from the root node finding all the materials
    // this doesn't go far enough tho' - could go down into the
    // faces which may have materials individually assigned
    QGLGeometry *g;
    QList<QGLGeometry *> modList;
    modList.append(root->geometry());
    while (modList.count() > 0)
    {
        QGLGeometry *mod = modList.takeFirst();
        Q_ASSERT(mSaveMaterials.contains(mod));
        mod->setMaterial(mSaveMaterials[mod]);
        QObjectList ch = mod->children();
        QObjectList::iterator it(ch.begin());
        for ( ; it != ch.end(); ++it)
            if ((g = qobject_cast<QGLGeometry*>(*it)))
                modList.append(g);
    }
}

void CubeView::setMaterial(QGLSceneNode *root, int material)
{
    QGLGeometry *g;
    QList<QGLGeometry *> modList;
    modList.append(root->geometry());
    mSaveMaterials.clear();
    while (modList.count() > 0)
    {
        QGLGeometry *mod = modList.takeFirst();
        mSaveMaterials.insert(mod, mod->material());
        mod->setMaterial(material);
        QObjectList ch = mod->children();
        QObjectList::iterator it(ch.begin());
        for ( ; it != ch.end(); ++it)
            if ((g = qobject_cast<QGLGeometry*>(*it)))
                modList.append(g);
    }
}

void CubeView::selectComponent()
{
    if (!mSceneManager)
        return;
    QAction *act = qobject_cast<QAction*>(sender());
    QGLSceneObject *component = mSceneManager->object(QGLSceneObject::Mesh, act->text());
    QGLSceneNode *nodeObj = qobject_cast<QGLSceneNode *>(component);
    if (mCurrentSelected)
        restoreMaterial(mCurrentSelected);
    mCurrentSelected = NULL;
    if (nodeObj == NULL) // not found or "Select none"
        return;
    mCurrentSelected = nodeObj;
    setMaterial(mCurrentSelected, mSelectMaterial);
}

/*!
    Ensure the model knows about the material color used to indicate
    a selection, by adding it to the scene palette.

    If there is currently a selection, adjust the colour by the animation
    value to produce a pulsing effect.
*/
void CubeView::makeSelectColor(QColor color)
{
    QGLMaterialParameters *mat = 0;
    QGLMaterialCollection *palette = mSceneRoot->geometry()->palette();
    if (mSelectMaterial == -1)
    {
        mat = new QGLMaterialParameters();
        mSelectMaterial = palette->addMaterial(mat);
    }
    else
    {
        mat = palette->materialByIndex(mSelectMaterial);
    }
    mat->setAmbientColor(color.darker(500));  // set ambient to 20%
    mat->setDiffuseColor(color.darker(125));  // set diffuse to 80%
    mat->setSpecularColor(color.lighter(125));  // set specular to 20% x bright
}

void CubeView::zoomChanged()
{
    QSlider *slider = qobject_cast<QSlider *>(sender());
    if (mZoom != slider->value())
    {
        mZoom = slider->value();
        updateGL();
    }
}

void CubeView::xTiltChanged(int tilt)
{
    int t = tilt * 45;
    if (t != mXTilt)
    {
        mXTilt = t;
        updateGL();
    }
}

void CubeView::yTiltChanged(int tilt)
{
    int t = tilt * 45;
    if (t != mYTilt)
    {
        mYTilt = t;
        updateGL();
    }
}

void CubeView::zTiltChanged(int tilt)
{
    int t = tilt * 45;
    if (t != mZTilt)
    {
        mXTilt = t;
        updateGL();
    }
}

void CubeView::yaxChanged()
{
    QSlider *slider = qobject_cast<QSlider *>(sender());
    if (mYax != slider->value())
    {
        mYax = slider->value();
        updateGL();
    }
}

void CubeView::selectColorChanged(const QColor &color)
{
    if (mSelectColor != color)
    {
        mSelectColor = color;
        updateGL();
    }
}

void CubeView::initializeGL()
{
    if (mInitialModel.isEmpty())
    {
        QMessageBox::warning(this, tr("No Models Found"),
                             tr("Could not find any model to load.  Select "
                                "Open from the File menu to navigate to a model file."));
    }
    else
    {
        importModel(mInitialModel);
        loadColors();
        loadComponents();
    }

    QGLPainter painter(this);
    initializeGL(&painter);
}

void CubeView::initializeGL(QGLPainter *painter)
{
    painter->setClearColor(QColor(32, 32, 128));
    painter->setDepthTestingEnabled(true);
    painter->setCullFaces(QGL::CullBackFaces);

    mLightModel->setAmbientSceneColor(QColor(196, 196, 196));
    painter->setLightModel(mLightModel);

    mLightParameters->setPosition(QVector3D(-0.5, 1.0, 3.0));
    painter->setLightParameters(0, mLightParameters);
    painter->setLightEnabled(0, true);

    connect(mTimer, SIGNAL(timeout()), this, SLOT(animate()));
    mTimer->start(25);

    connect(mColorTimer, SIGNAL(timeout()), this, SLOT(animateColor()));
    mColorTimer->start(50);
}

/*!
    \internal
*/
void CubeView::resizeGL(int w, int h)
{
    // Set up the standard viewport for the new window size.
    glViewport(0, 0, w, h);
    updateGL();
}

void CubeView::paintGL()
{
    QGLPainter painter(this);
    paintGL(&painter);
}

void CubeView::paintGL(QGLPainter *painter)
{
    if (!mSceneInitialized)
    {
        //mSceneRoot->apply(painter);
        mSceneInitialized = true;
    }

    painter->clear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Set perspective transformation and position model-view matrix
    QVector3D sceneOrigin;
    qreal viewDistance = mZoom;
    qreal maxDimension = 2.0f;
    if (mSceneRoot && mSceneRoot->geometry())
    {
        QBox3D box = mSceneRoot->geometry()->boundingBox();
        if (!box.isNull())
            sceneOrigin = box.center();
        if (box.isFinite())
        {
            QVector3D ext = box.size();
            maxDimension = qMax(ext.x(), qMax(ext.y(), ext.z()));
            viewDistance = viewDistance + maxDimension;
        }
    }

    QGLCamera camera;
    //camera.setCenter(sceneOrigin);
    camera.setEye(QVector3D(0.0f, 0.0f, viewDistance));
    camera.rotateCenter(camera.pan(mSpin));
    camera.apply(painter);

    painter->modelViewMatrix().push();
    painter->modelViewMatrix().translate(0.0f, mYax, 0.0f);

    QQuaternion xt = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, mXTilt);
    QQuaternion yt = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, mYTilt);
    QQuaternion zt = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, mZTilt);
    painter->modelViewMatrix().rotate(xt * yt * zt);

    painter->modelViewMatrix().translate(-sceneOrigin);

    if (mSceneRoot)
    {
        mSceneRoot->draw(painter);
    }
    else
    {
        if (!mWarningDisplayed)
        {
            QMessageBox::warning(this,
                                 tr("Unable to load model"),
                                 (mSceneManager ?
                                  tr("Could not find main scene") :
                                  tr("Could not load file %1").arg(mCurrentModelName))
                                 );
            mWarningDisplayed = true;
        }
    }

    painter->modelViewMatrix().pop();
}

void CubeView::animate()
{
    if (!mSceneRoot)
        return;
    if (!mAnimationEnabled)
        return;
    mSpin = (mSpin + 1) % 360;
    updateGL();
}

void CubeView::animateColor()
{
    if (mCurrentSelected)
    {
        mSelectColorAnimate = (mSelectColorAnimate + 5) % 180;
        float colorMod = sin(((float)mSelectColorAnimate / 180.0f) * M_PI);
        // colorMod ranges sinusoidally from 0.5 to 1.0 and back
        if (colorMod < 1.0f)
        {
            mPulse = QColor::fromRgbF(mSelectColor.redF() * colorMod,
                                            mSelectColor.greenF() * colorMod,
                                            mSelectColor.blueF() * colorMod);
            makeSelectColor(mPulse);
        }
        else
        {
            if (mPulse != mSelectColor)
            {
                mPulse = mSelectColor;
                makeSelectColor(mPulse);
            }
        }
    }
}

void CubeView::enableAnimation(bool enabled)
{
    mAnimationEnabled = enabled;
}
