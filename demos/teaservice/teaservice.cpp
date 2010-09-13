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

#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QKeyEvent>
#include <QAction>
#include <QDebug>
#include "qglview.h"
#include "meshobject.h"
#include "sceneobject.h"
#include "qglteapot.h"
#include "qglbuilder.h"
#if !defined(QT_OPENGL_ES_1)
#include "perpixeleffect.h"
#endif

enum {
    ObjTeapot,
    ObjTeacup1,
    ObjTeacup2,
    ObjTeaspoon1,
    ObjTeaspoon2
};

class Teapot : public MeshObject
{
    Q_OBJECT
public:
    Teapot(QObject *parent=0);
    ~Teapot() {}
};

static QGLSceneNode *createTeapot(QObject *parent)
{
    QGLBuilder builder;
    builder << QGLTeapot();
    QGLSceneNode *n = builder.finalizedSceneNode();
    n->setParent(parent);
    return n;
}

Teapot::Teapot(QObject *parent)
    : MeshObject(createTeapot(parent), parent)
{
}

class Teacup : public MeshObject
{
    Q_OBJECT
public:
    Teacup(QObject *parent=0);
    ~Teacup() {}
};

static QGLAbstractScene *loadBezier(const QString& fileName)
{
    QGLAbstractScene *scene;
    scene = QGLAbstractScene::loadScene(fileName, QLatin1String("bezier"));
    if (!scene)
        qFatal("Could not load %s, probably plugin could not be found",
               fileName.toLatin1().constData());
    return scene;
}

Teacup::Teacup(QObject *parent)
    : MeshObject(loadBezier(QLatin1String(":/teacup.txt")), parent)
{
}

class Teaspoon : public MeshObject
{
    Q_OBJECT
public:
    Teaspoon(QObject *parent=0);
    ~Teaspoon() {}
};

Teaspoon::Teaspoon(QObject *parent)
    : MeshObject(loadBezier(QLatin1String(":/teaspoon.txt")), parent)
{
}

class TeaService : public QObject
{
    Q_OBJECT
public:
    TeaService(QObject *parent=0);
    ~TeaService();

    SceneObject *service;

    Teapot *teapot;
    Teacup *teacup1;
    Teacup *teacup2;
    Teaspoon *teaspoon1;
    Teaspoon *teaspoon2;
#if !defined(QT_OPENGL_ES_1)
    PerPixelEffect *lighting;
#endif

    void changeMaterials(bool perPixel);

signals:
    void changed();

private slots:
    void teapotClicked();
    void teacup1Clicked();
    void teacup2Clicked();
    void teaspoon1Clicked();
    void teaspoon2Clicked();

private:
    QGLMaterial *china;
    QGLMaterial *chinaHighlight;
    QGLMaterial *metal;
    QGLMaterial *metalHighlight;
};

TeaService::TeaService(QObject *parent)
    : QObject(parent)
{
    china = new QGLMaterial(this);
    china->setAmbientColor(QColor(192, 150, 128));
    china->setSpecularColor(QColor(60, 60, 60));
    china->setShininess(128);

    chinaHighlight = new QGLMaterial(this);
    chinaHighlight->setAmbientColor(QColor(255, 192, 0));
    chinaHighlight->setSpecularColor(QColor(60, 60, 0));
    chinaHighlight->setShininess(128);

    metal = new QGLMaterial(this);
    metal->setAmbientColor(QColor(255, 255, 255));
    metal->setDiffuseColor(QColor(150, 150, 150));
    metal->setSpecularColor(QColor(255, 255, 255));
    metal->setShininess(128);

    metalHighlight = new QGLMaterial(this);
    metalHighlight->setAmbientColor(QColor(255, 255, 96));
    metalHighlight->setDiffuseColor(QColor(150, 150, 96));
    metalHighlight->setSpecularColor(QColor(255, 255, 255));
    metalHighlight->setShininess(128);

    service = new SceneObject(this);
    teapot = new Teapot(service);
    teacup1 = new Teacup(service);
    teacup2 = new Teacup(service);
    teacup1->setPosition(QVector3D(-2.3f, -0.75f, 0.0f));
    teacup2->setRotationAngle(180);
    teacup2->setRotationVector(QVector3D(0, 1, 0));
    teacup2->setPosition(QVector3D(2.3f, -0.75f, 0.0f));
    teaspoon1 = new Teaspoon(service);
    teaspoon2 = new Teaspoon(service);
    teaspoon1->setRotationAngle(275);
    teaspoon1->setRotationVector(QVector3D(1, 0, 0));
    teaspoon1->setPosition(QVector3D(-1.7f, -0.58f, 0.0f));
    teaspoon2->setRotationAngle(275);
    teaspoon2->setRotationVector(QVector3D(1, 0, 0));
    teaspoon2->setPosition(QVector3D(1.7f, -0.58f, 0.0f));

    teapot->setObjectId(ObjTeapot);
    teacup1->setObjectId(ObjTeacup1);
    teacup2->setObjectId(ObjTeacup2);
    teaspoon1->setObjectId(ObjTeaspoon1);
    teaspoon2->setObjectId(ObjTeaspoon2);

#if !defined(QT_OPENGL_ES_1)
    lighting = new PerPixelEffect();
#endif
    changeMaterials(false);

    connect(teapot, SIGNAL(hoverChanged()), this, SIGNAL(changed()));
    connect(teacup1, SIGNAL(hoverChanged()), this, SIGNAL(changed()));
    connect(teacup2, SIGNAL(hoverChanged()), this, SIGNAL(changed()));
    connect(teaspoon1, SIGNAL(hoverChanged()), this, SIGNAL(changed()));
    connect(teaspoon2, SIGNAL(hoverChanged()), this, SIGNAL(changed()));

    connect(teapot, SIGNAL(clicked()), this, SLOT(teapotClicked()));
    connect(teacup1, SIGNAL(clicked()), this, SLOT(teacup1Clicked()));
    connect(teacup2, SIGNAL(clicked()), this, SLOT(teacup2Clicked()));
    connect(teaspoon1, SIGNAL(clicked()), this, SLOT(teaspoon1Clicked()));
    connect(teaspoon2, SIGNAL(clicked()), this, SLOT(teaspoon2Clicked()));
}

TeaService::~TeaService()
{
#if !defined(QT_OPENGL_ES_1)
    delete lighting;
#endif
}

void TeaService::changeMaterials(bool perPixel)
{
    teapot->setMaterial(china);
    teapot->setHoverMaterial(chinaHighlight);
    teacup1->setMaterial(china);
    teacup1->setHoverMaterial(chinaHighlight);
    teacup2->setMaterial(china);
    teacup2->setHoverMaterial(chinaHighlight);
#if !defined(QT_OPENGL_ES_1)
    if (perPixel) {
        teapot->setEffect(lighting);
        teacup1->setEffect(lighting);
        teacup2->setEffect(lighting);
    } else
#endif
    {
        teapot->setEffect(0);
        teacup1->setEffect(0);
        teacup2->setEffect(0);
    }

    teaspoon1->setMaterial(metal);
    teaspoon1->setHoverMaterial(metalHighlight);
    teaspoon2->setMaterial(metal);
    teaspoon2->setHoverMaterial(metalHighlight);
#if !defined(QT_OPENGL_ES_1)
    if (perPixel) {
        teaspoon1->setEffect(lighting);
        teaspoon2->setEffect(lighting);
    } else
#endif
    {
        teaspoon1->setEffect(0);
        teaspoon2->setEffect(0);
    }
}

void TeaService::teapotClicked()
{
    qDebug("teapot clicked");
}

void TeaService::teacup1Clicked()
{
    qDebug("teacup1 clicked");
}

void TeaService::teacup2Clicked()
{
    qDebug("teacup2 clicked");
}

void TeaService::teaspoon1Clicked()
{
    qDebug("teaspoon1 clicked");
}

void TeaService::teaspoon2Clicked()
{
    qDebug("teaspoon2 clicked");
}

class TeaServiceView : public QGLView
{
    Q_OBJECT
public:
    TeaServiceView(QWidget *parent=0);

public slots:
    void standardLighting();
    void perPixelLighting();

protected:
    void initializeGL(QGLPainter *painter);
    void paintGL(QGLPainter *painter);
    void keyPressEvent(QKeyEvent *e);

private:
    TeaService *teaService;
};

TeaServiceView::TeaServiceView(QWidget *parent)
    : QGLView(parent)
{
    teaService = new TeaService(this);

    setOption(QGLView::ObjectPicking, true);

    connect(teaService, SIGNAL(changed()), this, SLOT(updateGL()));
}

void TeaServiceView::initializeGL(QGLPainter *painter)
{
    teaService->service->initialize(this, painter);
}

void TeaServiceView::paintGL(QGLPainter *painter)
{
    teaService->service->draw(painter);
}

void TeaServiceView::standardLighting()
{
    teaService->changeMaterials(false);
    updateGL();
}

void TeaServiceView::perPixelLighting()
{
    teaService->changeMaterials(true);
    updateGL();
}

void TeaServiceView::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Tab) {
        // The Tab key turns the ShowPicking option on and off,
        // which helps show what the pick buffer looks like.
        setOption(QGLView::ShowPicking, ((options() & QGLView::ShowPicking) == 0));
        updateGL();
    }
    QGLView::keyPressEvent(e);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow mainw;
    mainw.setMinimumSize(850, 480);
    mainw.setWindowTitle("Tea Service");

    TeaServiceView view;
    mainw.setCentralWidget(&view);
    view.setFocus();

    view.camera()->setEye(QVector3D(0, 3, 10));

    QMenu *menu = mainw.menuBar()->addMenu("Effects");

    QAction *standardLighting = new QAction("Standard lighting", &mainw);
    menu->addAction(standardLighting);
    QObject::connect(standardLighting, SIGNAL(triggered()), &view, SLOT(standardLighting()));

#if !defined(QT_OPENGL_ES_1)
    QAction *perPixelLighting = new QAction("Per-pixel lighting", &mainw);
    menu->addAction(perPixelLighting);
    QObject::connect(perPixelLighting, SIGNAL(triggered()), &view, SLOT(perPixelLighting()));
#endif

    menu->addSeparator();

    QAction *exitAction = new QAction("E&xit", &mainw);
    menu->addAction(exitAction);
    QObject::connect(exitAction, SIGNAL(triggered()), &app, SLOT(quit()));

    mainw.show();
    return app.exec();
}

#include "teaservice.moc"
