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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QFile>
#include <QCoreApplication>
#include <QFileDialog>
#include <QDir>
#include <QStandardItemModel>
#include <QPixmap>
#include <QCloseEvent>
#include <QSettings>
#include <QErrorMessage>
#include "qglabstractscene.h"
#include "qglscenenode.h"
#include "qglmaterialcollection.h"
#include "qglmaterial.h"

#include "shaderwizardglwidget.h"
#include "qglslsyntaxhighlighter.h"
#include "qglcolladafxeffectfactory.h"
#include "qglcolladafxeffectloader.h"
#include "qglcolladafxeffect.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
    , sceneModel(0)
    , mScene(0)
    , recentFilesSeperatorAdded(false)
{
    ui->setupUi(this);

    QCoreApplication::setOrganizationName(QLatin1String("Nokia"));
    QCoreApplication::setOrganizationDomain(QLatin1String("nokia.com"));
    QCoreApplication::setApplicationName(QLatin1String("shaderwizard"));

    glDisplayWidget = new ShaderWizardGLWidget;
    mainWindowLayout = new QVBoxLayout();
    mainWindowLayout->addWidget(glDisplayWidget);

    this->ui->displayWidget->setLayout(mainWindowLayout);

    connect(ui->actionCube, SIGNAL(triggered(bool)), this, SLOT(setCubeGeometry(bool)));
    connect(ui->actionSphere, SIGNAL(triggered(bool)), this, SLOT(setSphereGeometry(bool)));
    connect(ui->actionTeapot, SIGNAL(triggered(bool)), this, SLOT(setTeapotGeometry(bool)));
    connect(ui->actionSquare, SIGNAL(triggered(bool)), this, SLOT(setSquareGeometry(bool)));
    connect(ui->actionHeightMap, SIGNAL(triggered(bool)), this, SLOT(setHeightMapGeometry(bool)));

    connect(this, SIGNAL(sceneCreated(QObject*)), glDisplayWidget, SLOT(setSceneManager(QObject*)));
    connect(this, SIGNAL(sceneSelected(QObject*)), glDisplayWidget, SLOT(setSceneObject(QObject*)));
    connect(glDisplayWidget, SIGNAL(vertexShaderChanged(QString)), ui->textEditVertexShader, SLOT(setPlainText(QString)));
    connect(glDisplayWidget, SIGNAL(fragmentShaderChanged(QString)), ui->textEditFragmentShader, SLOT(setPlainText(QString)));

    new QGLSLSyntaxHighlighter(ui->textEditVertexShader->document());
    new QGLSLSyntaxHighlighter(ui->textEditFragmentShader->document());

    setupSceneModel();
    setupSceneView();

    connect(this, SIGNAL(materialSelected(QGLMaterial*)), ui->materialInspectorWidget, SLOT(setMaterial(QGLMaterial*)));
    connect(ui->materialInspectorWidget, SIGNAL(ambientColorChanged(QColor)), glDisplayWidget, SLOT(setAmbientMaterialColor(QColor)));
    connect(ui->materialInspectorWidget, SIGNAL(diffuseColorChanged(QColor)), glDisplayWidget, SLOT(setDiffuseMaterialColor(QColor)));
    connect(ui->materialInspectorWidget, SIGNAL(specularColorChanged(QColor)), glDisplayWidget, SLOT(setSpecularMaterialColor(QColor)));
    connect(ui->materialInspectorWidget, SIGNAL(shininessChanged(int)), glDisplayWidget, SLOT(setMaterialShininess(int)));
    connect(ui->materialInspectorWidget, SIGNAL(textureChanged(QImage)), glDisplayWidget, SLOT(setTextureImage(QImage)));

    ui->materialInspectorWidget->setAmbientColor(glDisplayWidget->ambientMaterialColor());
    ui->materialInspectorWidget->setDiffuseColor(glDisplayWidget->diffuseMaterialColor());
    ui->materialInspectorWidget->setSpecularColor(glDisplayWidget->specularMaterialColor());
    ui->materialInspectorWidget->setShininess(glDisplayWidget->materialShininess());
    ui->materialInspectorWidget->setTexture(QImage(QLatin1String(":/qtlogo.png")));

    readSettings();

    doRecentFileMenu();
}


 MainWindow::~MainWindow()
{
    delete ui;
    delete sceneModel;
    delete mScene;
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
    setShadersFromFiles(QLatin1String(":/shaders/flat_color_shader.vsh"),QLatin1String(":/shaders/flat_color_shader.fsh"));
}

void MainWindow::on_actionToon_Shader_triggered(bool)
{
    setShadersFromFiles(QLatin1String(":/shaders/toon_shader.vsh"),QLatin1String(":/shaders/toon_shader.fsh"));
}

void MainWindow::on_actionPer_Pixel_Lighting_Shader_triggered(bool)
{
    setShadersFromFiles(QLatin1String(":/shaders/per_pixel_lighting.vsh"),QLatin1String(":/shaders/per_pixel_lighting.fsh"));
}

void MainWindow::on_actionMinimal_Texture_Shader_triggered()
{
    setShadersFromFiles(QLatin1String(":/shaders/minimal_texture_shader.vsh"),QLatin1String(":/shaders/minimal_texture_shader.fsh"));
}

void MainWindow::on_actionPer_Pixel_Lit_Texture_Shader_triggered()
{
    setShadersFromFiles(QLatin1String(":/shaders/per_pixel_texture.vsh"),QLatin1String(":/shaders/per_pixel_texture.fsh"));
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
        QByteArray all(vertexShaderFile.readAll());
        ui->textEditVertexShader->setText(QString::fromLatin1(all.constData(), all.size()));
        glDisplayWidget->setVertexShader(ui->textEditVertexShader->toPlainText());
    } else {
        qWarning() << "Could not open file "<<vertexShaderFileName<<", failed to load vertex shader";
    }
    QFile fragmentShaderFile(fragmentShaderFileName);
    if (fragmentShaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray all(fragmentShaderFile.readAll());
        ui->textEditFragmentShader->setText(QString::fromLatin1(all.constData(), all.size()));
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

    QGLSceneNode* defaultSceneObject = mScene->defaultObject(QGLSceneNode::Main);
    if(qobject_cast<QGLSceneNode *>(defaultSceneObject))
        handleScene(qobject_cast<QGLSceneNode *>(defaultSceneObject));
    else
        handleScene(mScene);

    ui->sceneTreeView->expandAll();

    emit sceneCreated(mScene);
    doRecentFileMenu(fileName);
}

void MainWindow::loadEffect(const QString& fileName)
{
    effectLoader.load(fileName);

    if(effectLoader.count() == 0)
        qWarning() << "Warning: Failed to load effects from file " << fileName;
    else
    {
        // Until we have UI to select an effect, just grab the last one
        effectLoader[effectLoader.count() - 1]->generateShaders();
        glDisplayWidget->setEffect(effectLoader[effectLoader.count() - 1]);

        ui->textEditVertexShader->setPlainText(effectLoader[0]->vertexShader());
        ui->textEditFragmentShader->setPlainText(effectLoader[0]->fragmentShader());
        update();
    }
}

bool MainWindow::saveEffect()
{
    QGLColladaFxEffect *effectToExport = glDisplayWidget->effect();

    if(effectToExport == 0)
    {
        QErrorMessage* noEffectMessage = new QErrorMessage(this);
        noEffectMessage->setWindowTitle(tr("Shader Wizard"));
        noEffectMessage->showMessage(tr("Unable to find effect to export, aborting export"), tr("Error Getting Effect"));
        return false;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Effect as ColladaFx"),
                                                    QDir::homePath(),
                                                    tr("Collada Effects (*.dae *.xml)"));
    QFile saveFile(fileName);
    if(!saveFile.open(QFile::WriteOnly))
    {
        QErrorMessage* fileOpenError = new QErrorMessage(this);
        fileOpenError->setWindowTitle(tr("Shader Wizard"));
        fileOpenError->showMessage(tr("Unable to open file for writing: ") + fileName, tr("Error Opening File"));
        return false;
    }

    QString colladaString = QGLColladaFxEffectFactory::exportEffect(effectToExport, QFileInfo(saveFile).fileName() + QLatin1String("Effect"), QFileInfo(saveFile).fileName() + QLatin1String("Technique"));
    QTextStream out(&saveFile);
    out << colladaString;
    return true;
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
    QList<QGLSceneNode::Type> types;

    types << QGLSceneNode::Main;
    typeStrings << tr("Main Objects");
    types << QGLSceneNode::Mesh;
    typeStrings << tr("Meshes");

    QStandardItem *parentItem;
    QStandardItem *workingItem;

    for(int i=0; i < types.size() && i < typeStrings.size(); i++)
    {
        QList<QGLSceneNode *> objectList = scene->objects(types.at(i));

        if(!objectList.size())
            continue;

        // create the parent item
        parentItem = new QStandardItem( typeStrings.at(i) );
        parentItem->setData(QVariant::fromValue(qobject_cast<QObject*>(scene->defaultObject(types.at(i)))), SceneObjectRole);

        // create the children items
        foreach( QGLSceneNode *sceneObject , objectList )
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
        if (scene && scene->materialIndex() != -1)
        {
            emit materialSelected(scene->material());
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
 {
         writeSettings();
         event->accept();
 }

 void MainWindow::readSettings()
 {
     QSettings settings;

     settings.beginGroup(QLatin1String("MainWindow"));
     resize(settings.value(QLatin1String("size"), QSize(1280, 640)).toSize());
     move(settings.value(QLatin1String("pos"), QPoint(100, 100)).toPoint());
     settings.endGroup();

     settings.beginGroup(QLatin1String("Recent Files"));
     this->recentFiles = settings.value(QLatin1String("recentFileList"), QStringList()).toStringList();
     settings.endGroup();
 }

 void MainWindow::writeSettings()
 {
     QSettings settings;

     settings.beginGroup(QLatin1String("MainWindow"));
     settings.setValue(QLatin1String("size"), size());
     settings.setValue(QLatin1String("pos"), pos());
     settings.endGroup();

     settings.beginGroup(QLatin1String("Recent Files"));
     settings.setValue(QLatin1String("recentFileList"), recentFiles);
     settings.endGroup();
 }

void MainWindow::doRecentFileMenu(QString newFileName)
{
    const int maxRecent = 4;
    int indexInRecent = recentFiles.indexOf(newFileName);

    if(indexInRecent != -1)
    {
        recentFiles.removeAt(indexInRecent);
    }

    if(!newFileName.isEmpty())
    {
        while(recentFiles.size() >= maxRecent)
            recentFiles.pop_back();
        recentFiles.push_front(newFileName);
        writeSettings();
    }

    if(recentFiles.size() > 0 && !recentFilesSeperatorAdded)
    {
        ui->menuModel->addSeparator();
        recentFilesSeperatorAdded = true;
    }

    for(int i = 0; i < recentFiles.size(); i++)
    {
        QAction *workingAction;
        if(recentFileActions.size() < i + 1)
        {
            recentFileActions.append(( workingAction = new QAction(recentFiles.at(i), this) ));
            ui->menuModel->addAction(workingAction);
            connect(workingAction, SIGNAL(triggered()), this, SLOT(recentFileActionTriggered()));
        }
        else
            workingAction = recentFileActions.at(i);

        workingAction->setText(recentFiles.at(i));
    }
}

void MainWindow::recentFileActionTriggered()
{
    QAction *triggeredAction = qobject_cast<QAction*>(sender());
    if(triggeredAction)
    {
        loadScene(triggeredAction->text());
    }

};

void MainWindow::on_actionMultiTexture_Shader_triggered()
{    
    setShadersFromFiles(QLatin1String(":/shaders/per_pixel_texture.vsh"),QLatin1String(":/shaders/multitexture.fsh"));
}

void MainWindow::on_actionLoad_Collada_Effect_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 QDir::homePath(),
                                                 tr("Collada Effects (*.dae *.xml)"));
    if (fileName.isEmpty())
        return;
    emit openEffect(fileName);
    loadEffect(fileName);
}

void MainWindow::on_actionExport_Collada_Effect_triggered()
{
    saveEffect();
}
