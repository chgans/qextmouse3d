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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QGLWidget>
#include <QVBoxLayout>
#include <QModelIndex>
#include <QMenu>

class ShaderWizardGLWidget;
class QAbstractItemModel;
class QStandardItemModel;
class QGLAbstractScene;
class QGLSceneNode;
class QGLMaterial;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


    enum DataRoles
    {
        SceneObjectRole = Qt::UserRole
                      };

private slots:
    void on_actionExport_Collada_Effect_triggered();
    void on_actionLoad_Collada_Effect_triggered();
    void on_actionMultiTexture_Shader_triggered();
    void on_actionPer_Pixel_Lit_Texture_Shader_triggered();
    void on_actionMinimal_Texture_Shader_triggered();
    void on_actionLoad_From_File_triggered();
    void on_actionPer_Pixel_Lighting_Shader_triggered(bool checked);
    void on_actionToon_Shader_triggered(bool checked);
    void on_actionFlat_Shader_triggered(bool checked);
    void on_pushButtonApplyFragmentShader_clicked();
    void on_pushButtonApplyVertexShader_clicked();

    void setCubeGeometry(bool ignored);
    void setSphereGeometry(bool ignored);
    void setSquareGeometry(bool ignored);
    void setTeapotGeometry(bool ignored);
    void setHeightMapGeometry(bool ignored);
    void modelItemActivated(QModelIndex);
    void recentFileActionTriggered();
signals:
    void openFile(const QString &);
    void openEffect(const QString &);
    void sceneCreated(QObject* object);
    void sceneSelected(QObject* object);
    void materialSelected(QGLMaterial*);
protected:
    void closeEvent(QCloseEvent *event);
    void setupSceneModel();
    void setupSceneView();
    void loadScene(const QString &fileName);
    void loadEffect(const QString &fileName);
    bool saveEffect();
    void handleScene(QGLAbstractScene *scene);
    void handleScene(QGLSceneNode *scene);
    void writeSettings();
    void readSettings();
    void doRecentFileMenu(QString newFile = QString());

private:
    Ui::MainWindow *ui;

    ShaderWizardGLWidget *glDisplayWidget;
    QVBoxLayout *mainWindowLayout;
    void setShadersFromFiles(QString vertexShaderFileName, QString fragmentShaderFileName);
    QStandardItemModel *sceneModel;
    QGLAbstractScene *mScene;
    QList<QAction *> recentFileActions;
    QStringList recentFiles;
    bool recentFilesSeperatorAdded;
};

#endif // MAINWINDOW_H
