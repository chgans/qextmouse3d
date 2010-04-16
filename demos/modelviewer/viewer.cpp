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

#include "viewer.h"

#include <QTimer>
#include <QAction>
#include <QSlider>
#include <QMenu>
#include <QMessageBox>
#include <QColorDialog>
#include <QTime>
#include <QSettings>

#include <math.h>

 #ifndef M_PI
 #define M_PI 3.14159265358979323846
 #endif


#include "qglabstractscene.h"
#include "qglscenenode.h"
#include "qglpainter.h"
#include "qglcamera.h"
#include "qgeometrydata.h"
#include "qglmaterial.h"
#include "qglmaterialcollection.h"
#include "qline3d.h"

Viewer::Viewer(QWidget *parent)
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
    , mNewModelLoaded(false)
{
}

Viewer::~Viewer()
{
    delete mTimer;
    delete mSceneManager;
}

void Viewer::load()
{
    QAction *act = qobject_cast<QAction*>(sender());
    openModelFile(act->text());
}

void Viewer::openModelFile(const QString &fileName)
{
    mWarningDisplayed = false;
    importModel(fileName);
    loadColors();
    loadComponents();
}

QString Viewer::getOptions(const QString &name)
{
    QSettings settings;
    settings.beginGroup(name);
    QString options;
    QStringList keys = settings.childKeys();
    QStringList groups = settings.childGroups();
    QString key;
    QString group;
    while (keys.size() > 0 || groups.size() > 0)
    {
        if (keys.size() > 0)
        {
            key = keys.takeFirst();
            if (!key.startsWith("UI_"))
            {
                bool value = settings.value(key, false).toBool();
                if (value)
                {
                    QString op = group.isEmpty() ? key : QString("%1=%2").arg(group).arg(key);
                    options = options.isEmpty() ? op : options + " " + op;
                }
            }
        }
        if (keys.size() == 0 && groups.size() > 0)
        {
            if (!group.isEmpty())
                settings.endGroup();
            group = groups.takeFirst();
            settings.beginGroup(group);
            keys = settings.childKeys();
        }
    }
    return options;
}

void Viewer::importModel(const QString &name)
{
    delete mSceneManager;
    mSceneManager = 0;
    mSceneRoot = 0;
    mSelectMaterial = -1;
    QString options = Viewer::getOptions(name);
    QTime time;
    time.start();
    mSceneManager = QGLAbstractScene::loadScene(name, QString(), options);
    int ms = time.elapsed();
    mPrevFileName = mCurrentModelName;
    mCurrentModelName = name;
    if (!mSceneManager)
        return;
    mNewModelLoaded = true;
    QGLSceneObject *obj = mSceneManager->defaultObject(QGLSceneObject::Main);
    mSceneInitialized = false;
    mSceneRoot = qobject_cast<QGLSceneNode *>(obj);
#ifndef QT_NO_DEBUG_STREAM
    //qDumpScene(mSceneRoot);
    int totalIndexes = 0;
    QList<QGLSceneNode *> children = mSceneRoot->allChildren();
    QList<QGLSceneNode*>::const_iterator it(children.begin());
    for ( ; it != children.end(); ++it)
    {
        QGLSceneNode  *n = *it;
        totalIndexes += n->count();
        //if (n->objectName().startsWith("BatteryCov"))
        //    n->setNormalViewEnabled(true);
    }
    qDebug() << "Loaded:" << name << "/" << options << "--"
            << (totalIndexes / 3) << "triangles, in" << ms << "ms";
#endif
}

void Viewer::loadColors()
{
    if (mColorMenu->actions().count())
        mColorMenu->clear();
    QGLMaterialCollection *materials = 0;
    if (mSceneRoot)
        materials = mSceneRoot->palette();
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
        QGLMaterial *mat = materials->material(index);
        QPixmap px(16, 16);
        px.fill(mat->diffuseColor());
        QAction *act = new QAction(QIcon(px), materials->materialName(index), this);
        mColorMenu->addAction(act);
        QObject::connect(act, SIGNAL(triggered()), this, SLOT(changeColor()));
    }
}

void Viewer::changeColor()
{
    if (!mSceneRoot)
        return;
    QAction *act = qobject_cast<QAction*>(sender());
    if (!act)
        return;
    QGLMaterialCollection *materials = mSceneRoot->palette();
    int index = materials->indexOf(act->text());
    if (index == -1)
    {
        QMessageBox::warning(this, "Material Invalid", tr("Material %1 not found").arg(act->text()));
        return;
    }
    QGLMaterial *mat = materials->material(index);
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
    update();
}

void Viewer::loadComponents()
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
    mComponents.clear();
    for ( ; it != components.end(); ++it)
    {
        QGLSceneNode *nodeObj = qobject_cast<QGLSceneNode *>(*it);
        if (!nodeObj)
        {
            qWarning("Could not make scene node from scene object %s",
                     qPrintable((*it)->objectName()));
            continue;
        }
        QGeometryData *mesh = nodeObj->geometry();
        if (!mesh)
            continue;
        QString meshName = nodeObj->objectName();
        QString componentName = meshName;
        int pos = componentName.indexOf(":");
        if (pos != -1)
            componentName.truncate(pos);
        if (!mComponents.contains(componentName))
        {
            mComponents.append(componentName);
            QAction *act = new QAction(componentName, this);
            mComponentMenu->addAction(act);
            QObject::connect(act, SIGNAL(triggered()), this, SLOT(selectComponent()));
        }
    }
    mCurrentSelected = 0;
    makeSelectColor(mSelectColor);
}

void Viewer::restoreMaterial(QGLSceneNode *root)
{
    // recurse down from the root node finding all the materials
    // this doesn't go far enough tho' - could go down into the
    // faces which may have materials individually assigned
    QList<QGLSceneNode *> modList;
    modList.append(root);
    while (modList.count() > 0)
    {
        QGLSceneNode *mod = modList.takeFirst();
        Q_ASSERT(mSaveMaterials.contains(mod));
        mod->setMaterial(mSaveMaterials[mod]);
        QList<QGLSceneNode*> ch = mod->childNodes();
        QList<QGLSceneNode*>::const_iterator it = ch.begin();
        for ( ; it != ch.end(); ++it)
            if (!modList.contains(*it))
                modList.append(*it);
    }
}

void Viewer::setMaterial(QGLSceneNode *root, int material)
{
    QList<QGLSceneNode *> modList;
    modList.append(root);
    mSaveMaterials.clear();
    while (modList.count() > 0)
    {
        QGLSceneNode *mod = modList.takeFirst();
        mSaveMaterials.insert(mod, mod->material());
        mod->setMaterial(material);
        QList<QGLSceneNode*> ch = mod->childNodes();
        QList<QGLSceneNode*>::const_iterator it = ch.begin();
        for ( ; it != ch.end(); ++it)
            if (!modList.contains(*it))
                modList.append(*it);
    }
}

void Viewer::selectComponent()
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
void Viewer::makeSelectColor(QColor color)
{
    QGLMaterial *mat = 0;
    QGLMaterialCollection *palette = mSceneRoot->palette();
    if (mSelectMaterial == -1)
    {
        mat = new QGLMaterial();
        mSelectMaterial = palette->addMaterial(mat);
    }
    else
    {
        mat = palette->material(mSelectMaterial);
    }
    mat->setAmbientColor(color.darker(500));  // set ambient to 20%
    mat->setDiffuseColor(color.darker(125));  // set diffuse to 80%
    mat->setSpecularColor(color.lighter(125));  // set specular to 20% x bright
}

void Viewer::zoomChanged(int value)
{
    if (mZoom != value)
    {
        mZoom = value;
        update();
    }
}

void Viewer::xTiltChanged(int tilt)
{
    int t = tilt * 45;
    if (t != mXTilt)
    {
        mXTilt = t;
        update();
    }
}

void Viewer::yTiltChanged(int tilt)
{
    int t = tilt * 45;
    if (t != mYTilt)
    {
        mYTilt = t;
        update();
    }
}

void Viewer::zTiltChanged(int tilt)
{
    int t = tilt * 45;
    if (t != mZTilt)
    {
        mXTilt = t;
        update();
    }
}

void Viewer::yaxChanged(int value)
{
    if (mYax != value)
    {
        mYax = value;
        update();
    }
}

void Viewer::selectColorChanged(const QColor &color)
{
    if (mSelectColor != color)
    {
        mSelectColor = color;
        update();
    }
}

void Viewer::initializeGL()
{
    if (mInitialModel.isEmpty())
    {
        QMessageBox::warning(this, tr("No Models Found"),
                             tr("Could not find any model to load.  Select "
                                "Open from the File menu to navigate to a model file."));
    }
    else
    {
        openModelFile(mInitialModel);
    }

    QGLPainter painter(this);
    initializeGL(&painter);
}

void Viewer::initializeGL(QGLPainter *painter)
{
    painter->setClearColor(QColor(32, 32, 128));
    painter->setDepthTestingEnabled(true);
    painter->setCullFaces(QGL::CullBackFaces);

    mLightModel->setAmbientSceneColor(QColor(196, 196, 196));
    painter->setLightModel(mLightModel);

    mLightParameters->setPosition(QVector3D(-0.5, 1.0, 3.0));
    painter->setMainLight(mLightParameters);

    connect(mTimer, SIGNAL(timeout()), this, SLOT(animate()));
    mTimer->start(25);

    connect(mColorTimer, SIGNAL(timeout()), this, SLOT(animateColor()));
    mColorTimer->start(50);
}

/*!
    \internal
*/
void Viewer::resizeGL(int w, int h)
{
    // Set up the standard viewport for the new window size.
    glViewport(0, 0, w, h);
    update();
}

void Viewer::paintGL()
{
    QGLPainter painter(this);
    paintGL(&painter);
}

void Viewer::paintGL(QGLPainter *painter)
{
    if (mNewModelLoaded)
    {
        mNewModelLoaded = false;
        if (!mPrevFileName.isEmpty())
            emit modelUnloaded(mPrevFileName);
        emit modelLoaded(mCurrentModelName);
    }

    if (!mSceneInitialized)
    {
        mSceneInitialized = true;
    }

    painter->clear();

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
    painter->setCamera(&camera);

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
                                  + tr(".  Check console output for more details.")
                                 );
            mWarningDisplayed = true;
        }
    }

    painter->modelViewMatrix().pop();
}

void Viewer::animate()
{
    if (!mSceneRoot)
        return;
    if (!mAnimationEnabled)
        return;
    mSpin = (mSpin + 1) % 360;
    update();
}

void Viewer::animateColor()
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

void Viewer::enableAnimation(bool enabled)
{
    mAnimationEnabled = enabled;
}
