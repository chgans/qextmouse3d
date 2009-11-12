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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QFile>
#include "shaderwizardglwidget.h"
#include "qglslsyntaxhighlighter.h"
#include <QFileDialog>
#include <QDir>
#include <QStandardItemModel>
#include <QPixmap>
#include "qglabstractscene.h"
#include "qglscenenode.h"
#include "qglmaterialcollection.h"
#include "qglmaterialparameters.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
    , sceneModel(0)
    , mScene(0)
{
    ui->setupUi(this);

    glDisplayWidget = new ShaderWizardGLWidget;
    mainWindowLayout = new QVBoxLayout();
    mainWindowLayout->addWidget(glDisplayWidget);

    this->ui->displayWidget->setLayout(mainWindowLayout);

    connect(ui->actionCube, SIGNAL(triggered(bool)), this, SLOT(setCubeGeometry(bool)));
    connect(ui->actionSphere, SIGNAL(triggered(bool)), this, SLOT(setSphereGeometry(bool)));
    connect(ui->actionBezier, SIGNAL(triggered(bool)), this, SLOT(setBezierGeometry(bool)));
    connect(ui->actionTeapot, SIGNAL(triggered(bool)), this, SLOT(setTeapotGeometry(bool)));
    connect(ui->actionSquare, SIGNAL(triggered(bool)), this, SLOT(setSquareGeometry(bool)));
    connect(ui->actionHeightMap, SIGNAL(triggered(bool)), this, SLOT(setHeightMapGeometry(bool)));

    connect(this, SIGNAL(sceneCreated(QObject*)), glDisplayWidget, SLOT(setSceneManager(QObject*)));
    connect(this, SIGNAL(sceneSelected(QObject*)), glDisplayWidget, SLOT(setSceneObject(QObject*)));

    new QGLSLSyntaxHighlighter(ui->textEditVertexShader->document());
    new QGLSLSyntaxHighlighter(ui->textEditFragmentShader->document());

    setupSceneModel();
    setupSceneView();

    connect(ui->materialInspectorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(setColor(QColor)));
    connect(this, SIGNAL(materialSelected(QGLMaterialParameters*)), ui->materialInspectorWidget, SLOT(setMaterial(QGLMaterialParameters*)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete sceneModel;
    delete mScene;
}

void MainWindow::setBezierGeometry(bool)
{
    glDisplayWidget->setBezierGeometry();
}

void MainWindow::setSquareGeometry(bool)
{
    glDisplayWidget->setSquareGeometry();
}

void MainWindow::setCubeGeometry(bool)
{
    glDisplayWidget->setCubeGeometry();
}

void MainWindow::setSphereGeometry(bool)
{
    glDisplayWidget->setSphereGeometry();
}

void MainWindow::setTeapotGeometry(bool)
{
    glDisplayWidget->setTeapotGeometry();
}

void MainWindow::setHeightMapGeometry(bool)
{
    glDisplayWidget->setHeightMapGeometry();
}

void MainWindow::on_pushButtonApplyVertexShader_clicked()
{
    glDisplayWidget->setVertexShader(ui->textEditVertexShader->toPlainText());
}

void MainWindow::on_pushButtonApplyFragmentShader_clicked()
{
    glDisplayWidget->setFragmentShader(ui->textEditFragmentShader->toPlainText());

}

void MainWindow::on_actionFlat_Shader_triggered(bool)
{
    setShadersFromFiles("flat_color_shader.vsh","flat_color_shader.fsh");
}

void MainWindow::on_actionToon_Shader_triggered(bool)
{
    setShadersFromFiles("toon_shader.vsh","toon_shader.fsh");
}

void MainWindow::on_actionPer_Pixel_Lighting_Shader_triggered(bool)
{
    setShadersFromFiles("per_pixel_lighting.vsh","per_pixel_lighting.fsh");
}

void MainWindow::on_actionLoad_From_File_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 QDir::homePath(),
                                                 tr("Models (*.3ds *.obj)"));
    if (fileName.isEmpty())
        return;
    emit openFile(fileName);
    loadScene(fileName);
}

void MainWindow::setShadersFromFiles(QString vertexShaderFileName, QString fragmentShaderFileName)
{
    QFile vertexShaderFile(vertexShaderFileName);
    if (vertexShaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->textEditVertexShader->setText(QString(vertexShaderFile.readAll()));
        glDisplayWidget->setVertexShader(ui->textEditVertexShader->toPlainText());
    } else {
        qWarning() << "Could not open file "<<vertexShaderFileName<<", failed to load vertex shader";
    }
    QFile fragmentShaderFile(fragmentShaderFileName);
    if (fragmentShaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->textEditFragmentShader->setText(QString(fragmentShaderFile.readAll()));
        glDisplayWidget->setFragmentShader(ui->textEditFragmentShader->toPlainText());
    } else {
        qWarning() << "Could not open file "<<fragmentShaderFileName<<", failed to load fragment shader";
    }
}

void MainWindow::setupSceneModel()
{
    sceneModel = new QStandardItemModel();
}

void MainWindow::setupSceneView()
{
    ui->sceneTreeView->setModel(sceneModel);
    connect(ui->sceneTreeView, SIGNAL(clicked(QModelIndex)), this
            , SLOT(modelItemActivated(QModelIndex)));
}

void MainWindow::loadScene(const QString& fileName)
{
    delete mScene;
    mScene = QGLAbstractScene::loadScene(fileName);

    if(!mScene)
        return;

    QGLSceneObject* defaultSceneObject = mScene->defaultObject(QGLSceneObject::Main);
    if(qobject_cast<QGLSceneNode *>(defaultSceneObject))
        handleScene(qobject_cast<QGLSceneNode *>(defaultSceneObject));
    else
        handleScene(mScene);

    ui->sceneTreeView->expandAll();

    emit sceneCreated(mScene);
}

static void addSceneNodeToDataModel(QStandardItem *parent, QGLSceneNode *node)
{
    if(!node)
        return;

    QStandardItem *newItem = new QStandardItem( node->objectName() );
    newItem->setData(QVariant::fromValue(qobject_cast<QObject*>(node)), MainWindow::SceneObjectRole);

    for(int i = 0; i < node->children().size(); i++)
    {

        addSceneNodeToDataModel(newItem, qobject_cast<QGLSceneNode*>(node->children().at(i)));
    }

    parent->appendRow(newItem);
}

void MainWindow::handleScene(QGLSceneNode *scene)
{
    sceneModel->clear();

    QStandardItem* rootItem = new QStandardItem(scene->objectName());
    rootItem->setData(QVariant::fromValue(qobject_cast<QObject*>(scene)), SceneObjectRole);

    addSceneNodeToDataModel(rootItem, scene);
    sceneModel->appendRow(rootItem);
}

void MainWindow::handleScene(QGLAbstractScene* scene)
{
    // Create data model for scene
    if(sceneModel)
        sceneModel->clear();
    else
        sceneModel = new QStandardItemModel();

    QList<QString> typeStrings;
    QList<QGLSceneObject::Type> types;

    types << QGLSceneObject::Main;
    typeStrings << tr("Main Objects");
    types << QGLSceneObject::World;
    typeStrings << tr("World");
    types << QGLSceneObject::Camera;
    typeStrings << tr("Cameras");
    types << QGLSceneObject::Effect;
    typeStrings << tr("Effects");
    types << QGLSceneObject::Light;
    typeStrings << tr("Lights");
    types << QGLSceneObject::Mesh;
    typeStrings << tr("Meshes");

    QStandardItem *parentItem;
    QStandardItem *workingItem;

    for(int i=0; i < types.size() && i < typeStrings.size(); i++)
    {
        QList<QGLSceneObject *> objectList = scene->objects(types.at(i));

        if(!objectList.size())
            continue;

        // create the parent item
        parentItem = new QStandardItem( typeStrings.at(i) );
        parentItem->setData(QVariant::fromValue(qobject_cast<QObject*>(scene->defaultObject(types.at(i)))), SceneObjectRole);

        // create the children items
        foreach( QGLSceneObject *sceneObject , objectList )
        {
            workingItem = new QStandardItem(sceneObject->objectName());
            workingItem->setData(QVariant::fromValue(qobject_cast<QObject*>(sceneObject)), SceneObjectRole);
            parentItem->appendRow(workingItem);
        }
        sceneModel->appendRow(parentItem);
    }
}

void MainWindow::modelItemActivated(QModelIndex index)
{
    QStandardItem *item = sceneModel->itemFromIndex(index);
    QVariant data = item->data(SceneObjectRole);
    QObject* object = item->data(SceneObjectRole).value<QObject*>();
    if(object)
    {
        emit sceneSelected(object);
        QGLSceneNode *scene = qobject_cast<QGLSceneNode *>(object);
        if(scene && scene->geometry() && scene->geometry()->material() != -1)
        {
            emit materialSelected(scene->geometry()->palette()->materialByIndex( scene->geometry()->material() ));
        }
    }
}
