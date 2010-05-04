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

#include "controls.h"
#include "ui_controls.h"
#include "optionsdialog.h"
#include "viewer.h"
#include "qmlgenerator.h"
#include "model.h"
#include "qglscenenode.h"

#include <QtCore/qdir.h>
#include <QtGui/qcolordialog.h>
#include <QtGui/qfiledialog.h>
#include <QtCore/qsettings.h>
#include <QtGui/qevent.h>
#include <QtCore/qdebug.h>

Controls::Controls(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::Controls)
    , m_view(0)
{
    m_ui->setupUi(this);
    m_view = new Viewer(m_ui->frame);
    QHBoxLayout *lay = new QHBoxLayout();
    lay->addWidget(m_view);
    m_ui->frame->setLayout(lay);
    QString initialModel = populateModelMenu();
    qDebug() << "Initial model:" << initialModel << "!!";
    m_model = new Model(this);
    connect(m_ui->actionQuit, SIGNAL(triggered()),
            this, SLOT(close()));

    connect(this, SIGNAL(openFile(QString)),
            m_model, SLOT(setFullPath(QString)));

    connect(m_model, SIGNAL(modelLoaded(QString)),
            this, SLOT(loadModelDefaults(QString)));
    connect(m_model, SIGNAL(modelLoaded(QString)),
            this, SLOT(loadSettings(QString)));
    connect(m_model, SIGNAL(modelLoaded(QString)),
            this, SLOT(setWindowTitle(QString)));
    connect(m_model, SIGNAL(modelLoaded(QString)),
            this, SLOT(addRecentFiles(QString)));
    connect(m_model, SIGNAL(modelLoaded(QString)),
            m_ui->treeView, SLOT(expandAll()));

    connect(m_model, SIGNAL(modelUnloaded(QString)),
            this, SLOT(saveModelDefaults(QString)));
    connect(m_model, SIGNAL(modelUnloaded(QString)),
            this, SLOT(saveSettings(QString)));
    connect(m_model, SIGNAL(modelLoadTime(int)),
            this , SLOT(fileLoadTimeNotified(int)));
    connect(m_model, SIGNAL(modelTriangles(int)),
            this, SLOT(triangleCountUpdated(int)));

    m_triangleCount = new QLabel(tr("0 triangles"));
    m_ui->statusbar->addPermanentWidget(m_triangleCount);
    m_model->setFullPath(initialModel);
    m_view->setModel(m_model);
    m_ui->treeView->setModel(m_model);

    connect(m_ui->actionForce_Smooth, SIGNAL(triggered(bool)),
            this, SLOT(optionMenuToggled(bool)));
    connect(m_ui->actionForce_Faceted, SIGNAL(triggered(bool)),
            this, SLOT(optionMenuToggled(bool)));
    connect(m_ui->actionNative_Indices, SIGNAL(triggered(bool)),
            this, SLOT(optionMenuToggled(bool)));
    connect(m_ui->actionCorrect_Normals, SIGNAL(triggered(bool)),
            this, SLOT(optionMenuToggled(bool)));
    connect(m_ui->actionCorrect_Acute, SIGNAL(triggered(bool)),
            this, SLOT(optionMenuToggled(bool)));
    connect(m_ui->actionShow_Warnings, SIGNAL(triggered(bool)),
            this, SLOT(optionMenuToggled(bool)));
    connect(m_ui->generateQmlPushButton, SIGNAL(clicked()),
            m_ui->actionSave_QML, SIGNAL(triggered()));
}

Controls::~Controls()
{
    delete m_ui;
}

void Controls::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Controls::load()
{
    QAction *act = qobject_cast<QAction*>(sender());
    if (act)
    {
        m_model->setFullPath(act->text());
    }
}

QString Controls::populateModelMenu()
{
    QString first;
    QMenu *menu = m_ui->menuModels;
    QStringList searchDirs;
    searchDirs << ":/" << "./";
    QStringList::const_iterator it = searchDirs.begin();
    for ( ; it != searchDirs.end(); ++it)
    {
        if (it != searchDirs.begin())
            menu->addSeparator();
        QDir modelDir(*it);
        QStringList filters("*.3ds");
        modelDir.setNameFilters(filters);
        QStringList models = modelDir.entryList();
        QStringList::const_iterator mit = models.begin();
        for ( ; mit != models.end(); ++mit)
        {
            QString name = *mit;
            name.prepend(*it);
            first = name;
            QAction *act = new QAction(name, this);
            menu->addAction(act);
            QObject::connect(act, SIGNAL(triggered()),
                             this, SLOT(load()));
        }
    }
    QString cmdlineModel;
    QStringList args = qApp->arguments();
    int ix = args.indexOf(QLatin1String("--model"));
    if (ix == -1)
        ix = args.indexOf(QLatin1String("-model"));
    if (ix == -1)
        ix = args.indexOf(QLatin1String("-m"));
    if (ix != -1)
    {
        int mdlIx = ix + 1;
        if (args.count() == mdlIx)
            qWarning("%s requires a string with the model file path", qPrintable(args[ix]));
        else
            cmdlineModel = args[mdlIx];
    }
    if (!cmdlineModel.isEmpty())
    {
        if (QFile::exists(cmdlineModel))
            return cmdlineModel;
        else
            qWarning("%s model file could not be found", qPrintable(cmdlineModel));
    }
    return first;
}

void Controls::loadModelDefaults(const QString &model)
{
    m_view->reset();
    QSettings settings;
    if (settings.childGroups().contains(model))
    {
        settings.beginGroup(model);
        int x = settings.value("UI_x", 0).toInt();
        int y = settings.value("UI_y", 0).toInt();
        int z = settings.value("UI_z", 0).toInt();
        int rotX = settings.value("UI_rotX", 0).toInt();
        int rotY = settings.value("UI_rotY", 0).toInt();
        int rotZ = settings.value("UI_rotZ", 0).toInt();
        m_view->setX(x);
        m_view->setY(y);
        m_view->setZ(z);
        m_view->setRotX(rotX);
        m_view->setRotY(rotY);
        m_view->setRotZ(rotZ);
    }
    else
    {
        QBox3D box = m_model->scene()->boundingBox();
        QVector3D sceneOrigin = box.center();
        QVector3D ext = box.size();
        qreal maxDimension = qMax(ext.x(), qMax(ext.y(), ext.z()));
        m_view->setX(sceneOrigin.x());
        m_view->setY(sceneOrigin.y());
        m_view->setZ(qMax(sceneOrigin.z(), maxDimension));
    }
    m_ui->xTranSpin->setValue(m_view->x());
    m_ui->yTranSpin->setValue(m_view->y());
    m_ui->zTranSpin->setValue(m_view->z());
    m_ui->xRotSpin->setValue(m_view->rotX());
    m_ui->yRotSpin->setValue(m_view->rotY());
    m_ui->zRotSpin->setValue(m_view->rotZ());
}

void Controls::saveModelDefaults(const QString &model)
{
    QSettings settings;
    settings.beginGroup(model);
    settings.setValue("UI_x", m_view->x());
    settings.setValue("UI_y", m_view->y());
    settings.setValue("UI_z", m_view->z());
    settings.setValue("UI_rotX", m_view->rotX());
    settings.setValue("UI_rotY", m_view->rotY());
    settings.setValue("UI_rotZ", m_view->rotZ());
}

void Controls::on_spinCheckBox_stateChanged(int state)
{
    m_view->enableAnimation(state == Qt::Checked);
}

void Controls::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 QDir::homePath(),
                                                 tr("Models (*.3ds *.obj)"));
    if (fileName.isEmpty())
        return;
    setWindowTitle(fileName);
    emit openFile(fileName);
}

void Controls::on_actionComponent_triggered()
{
    OptionsDialog *od = new OptionsDialog(m_model->fullPath(), m_model->components(), this);
    od->setAttribute(Qt::WA_DeleteOnClose);
    int result = od->exec();
    if (result == QDialog::Accepted)
    {
        emit openFile(m_model->fullPath());
    }
}

void Controls::optionMenuToggled(bool checked)
{
    QAction *act = qobject_cast<QAction *>(sender());
    Q_ASSERT(act);
    if (act == m_ui->actionForce_Smooth && checked && m_ui->actionForce_Faceted->isChecked())
        m_ui->actionForce_Faceted->setChecked(false);
    if (act == m_ui->actionForce_Faceted && checked && m_ui->actionForce_Smooth->isChecked())
        m_ui->actionForce_Smooth->setChecked(false);
    saveSettings(m_model->fullPath());
    emit openFile(m_model->fullPath());
}

void Controls::saveSettings(const QString &model)
{
    QSettings settings;
    settings.beginGroup(model);
    bool forceSmooth = m_ui->actionForce_Smooth->isChecked();
    settings.setValue("ForceSmooth", forceSmooth);
    bool forceFaceted = m_ui->actionForce_Faceted->isChecked();
    settings.setValue("ForceFaceted", forceFaceted);
    bool nativeIndices = m_ui->actionNative_Indices->isChecked();
    settings.setValue("NativeIndices", nativeIndices);
    bool correctNormals = m_ui->actionCorrect_Normals->isChecked();
    settings.setValue("CorrectNormals", correctNormals);
    bool correctAcute = m_ui->actionCorrect_Acute->isChecked();
    settings.setValue("CorrectAcute", correctAcute);
    bool showWarnings = m_ui->actionShow_Warnings->isChecked();
    settings.setValue("ShowWarnings", showWarnings);
}

void Controls::loadSettings(const QString &model)
{
    QSettings settings;
    settings.beginGroup(model);
    bool forceSmooth = settings.value("ForceSmooth", false).toBool();
    m_ui->actionForce_Smooth->setChecked(forceSmooth);
    bool forceFaceted = settings.value("ForceFaceted", false).toBool();
    m_ui->actionForce_Faceted->setChecked(forceFaceted);
    bool nativeIndices = settings.value("NativeIndices", false).toBool();
    m_ui->actionNative_Indices->setChecked(nativeIndices);
    bool correctNormals = settings.value("CorrectNormals", false).toBool();
    m_ui->actionCorrect_Normals->setChecked(correctNormals);
    bool correctAcute = settings.value("CorrectAcute", false).toBool();
    m_ui->actionCorrect_Acute->setChecked(correctAcute);
    bool showWarnings = settings.value("ShowWarnings", false).toBool();
    m_ui->actionShow_Warnings->setChecked(showWarnings);
}

void Controls::addRecentFiles(const QString &fileName)
{
    qDebug() << "addRecentFiles(" << fileName << ")";
    QMenu *rf = m_ui->menuRecent_Models;
    QSettings settings;
    QStringList files = settings.value("RecentFiles", QStringList()).toStringList();
    qDebug() << "current files:" << files;
    files.removeAll(fileName);
    files.push_front(fileName);
    if (files.size() > 10)
        files.pop_back();
    QAction *act;
    while (rf->actions().count() > 0)
    {
        act = rf->actions().at(0);
        rf->removeAction(act);
        delete act;
    }
    for (int i = 0; i < files.count(); ++i)
    {
        act = new QAction(files.at(i), this);
        connect(act, SIGNAL(triggered()),
                this, SLOT(load()));
        rf->addAction(act);
    }
    settings.setValue("RecentFiles", files);
}

void Controls::on_actionQuit_triggered()
{
    QString model = m_model->fullPath();
    saveSettings(model);
    saveModelDefaults(model);
    close();
}

void Controls::on_actionSave_QML_triggered()
{
    QString modelFileName = m_model->fullPath();
    QFileInfo fi(modelFileName);
    QString qmlName = fi.baseName();
    qmlName = qmlName.mid(1).prepend(qmlName[0].toUpper()) + ".qml";
    QString path = fi.absoluteDir().absoluteFilePath(qmlName);
    QString file = QFileDialog::getSaveFileName(this, tr("Save QML file"), path,
                                                tr("QML files (*.qml)"));
    if (!file.isEmpty())
    {
        QFileInfo fi2(file);
        QDir d = fi2.absoluteDir();
        QString relName = d.relativeFilePath(modelFileName);
        QString n2 = fi2.baseName();
        n2 = n2.mid(1).prepend((n2[0].toUpper())) + ".qml";
        file = fi2.absolutePath() + "/" + n2;
        QmlGenerator gen(file);
        gen.setProperty("modelFileName", relName);
        saveSettings(modelFileName);
        QString options = m_model->getOptions();
        if (!options.isEmpty())
            gen.setProperty("options", options);
        gen.save();
    }
}

void Controls::triangleCountUpdated(int count)
{
    m_triangleCount->setText(tr("%1 triangles").arg(count));
}

void Controls::fileLoadTimeNotified(int time)
{
    m_ui->statusbar->showMessage(tr("file loaded in %1 ms").arg(time), 30000);
}

void Controls::on_viewComboBox_currentIndexChanged(int view)
{
    m_view->setView(static_cast<Viewer::View>(view));
}

void Controls::on_actionSpin_triggered()
{
    m_ui->spinCheckBox->setChecked(m_ui->actionSpin->isChecked());
}

void Controls::on_actionShow_Floor_triggered()
{
    m_ui->floorCheckBox->setChecked(m_ui->actionShow_Floor->isChecked());
}

void Controls::on_xRotSpin_editingFinished()
{
    m_view->setRotX(m_ui->xRotSpin->value());
}

void Controls::on_yRotSpin_editingFinished()
{
    m_view->setRotY(m_ui->yRotSpin->value());
}

void Controls::on_zRotSpin_editingFinished()
{
    m_view->setRotZ(m_ui->zRotSpin->value());
}

void Controls::on_xTranSpin_editingFinished()
{
    m_view->setX(m_ui->xTranSpin->value());
}

void Controls::on_yTranSpin_editingFinished()
{
    m_view->setY(m_ui->yTranSpin->value());
}

void Controls::on_zTranSpin_editingFinished()
{
    m_view->setZ(m_ui->zTranSpin->value());
}

void Controls::on_floorCheckBox_toggled(bool checked)
{
    m_view->setFloorEnabled(checked);
}
