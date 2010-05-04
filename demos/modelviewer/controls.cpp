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

#include <QtCore/qdir.h>
#include <QtGui/qcolordialog.h>
#include <QtGui/qfiledialog.h>
#include <QtCore/qsettings.h>

#include <QtCore/qdebug.h>

Controls::Controls(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Controls)
    , mView(0)
    , mSelectColor(QColor(Qt::blue))
{
    ui->setupUi(this);
    mView = new Viewer(ui->frame);
    QHBoxLayout *lay = new QHBoxLayout();
    lay->addWidget(mView);
    ui->frame->setLayout(lay);
    connect(ui->xTiltComboBox, SIGNAL(currentIndexChanged(int)),
            mView, SLOT(xTiltChanged(int)));
    connect(ui->yTiltComboBox, SIGNAL(currentIndexChanged(int)),
            mView, SLOT(yTiltChanged(int)));
    connect(ui->zTiltComboBox, SIGNAL(currentIndexChanged(int)),
            mView, SLOT(zTiltChanged(int)));
    connect(ui->zoomSlider, SIGNAL(valueChanged(int)),
            mView, SLOT(zoomChanged(int)));
    connect(ui->yaxSlider, SIGNAL(valueChanged(int)),
            mView, SLOT(yaxChanged(int)));
    mView->setColorMenu(ui->menuColors);
    mView->setComponentMenu(ui->menuComponents);
    connect(ui->actionQuit, SIGNAL(triggered()),
            this, SLOT(close()));
    connect(ui->actionSelect_None, SIGNAL(triggered()),
            mView, SLOT(selectComponent()));
    connect(mView, SIGNAL(colorUpdate(QColor)),
            this, SLOT(signalColor(QColor)));
    connect(this, SIGNAL(updateSelectColor(QColor)),
            mView, SLOT(selectColorChanged(QColor)));
    connect(this, SIGNAL(openFile(QString)),
            mView, SLOT(openModelFile(QString)));
    connect(mView, SIGNAL(modelLoaded(QString)),
            this, SLOT(loadModelDefaults(QString)));
    connect(mView, SIGNAL(modelLoaded(QString)),
            this, SLOT(loadSettings(QString)));
    connect(mView, SIGNAL(modelLoaded(QString)),
            this, SLOT(setWindowTitle(QString)));
    connect(mView, SIGNAL(modelUnloaded(QString)),
            this, SLOT(saveModelDefaults(QString)));
    connect(mView, SIGNAL(modelUnloaded(QString)),
            this, SLOT(saveSettings(QString)));
    QPixmap px(32, 32);
    px.fill(QColor(mSelectColor));
    ui->selectColorButton->setIcon(QIcon(px));
    QString initialModel = populateModelMenu();
    mView->setInitialModel(initialModel);
    connect(ui->actionForce_Smooth, SIGNAL(triggered(bool)),
            this, SLOT(optionMenuToggled(bool)));
    connect(ui->actionForce_Faceted, SIGNAL(triggered(bool)),
            this, SLOT(optionMenuToggled(bool)));
    connect(ui->actionNative_Indices, SIGNAL(triggered(bool)),
            this, SLOT(optionMenuToggled(bool)));
    connect(ui->actionCorrect_Normals, SIGNAL(triggered(bool)),
            this, SLOT(optionMenuToggled(bool)));
    connect(ui->actionCorrect_Acute, SIGNAL(triggered(bool)),
            this, SLOT(optionMenuToggled(bool)));
    connect(ui->actionShow_Warnings, SIGNAL(triggered(bool)),
            this, SLOT(optionMenuToggled(bool)));
    connect(ui->generateQmlPushButton, SIGNAL(clicked()),
            ui->actionSave_QML, SIGNAL(triggered()));
}

Controls::~Controls()
{
    delete ui;
}

void Controls::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString Controls::populateModelMenu()
{
    QString first;
    QMenu *menu = ui->menuModels;
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
                             mView, SLOT(load()));
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

void Controls::signalColor(const QColor &color)
{
    QPixmap px(32, 32);
    px.fill(color);
    ui->selectColorButton->setIcon(QIcon(px));
}

void Controls::loadModelDefaults(const QString &model)
{
    QSettings settings;
    settings.beginGroup(model);
    int x = settings.value("UI_XTiltComboBox", 0).toInt();
    int y = settings.value("UI_YTiltComboBox", 0).toInt();
    int z = settings.value("UI_ZTiltComboBox", 0).toInt();
    int zoom = settings.value("UI_ZoomSlider", 10).toInt();
    int yax = settings.value("UI_YAxSlider", 0).toInt();
    ui->xTiltComboBox->setCurrentIndex(x);
    ui->yTiltComboBox->setCurrentIndex(y);
    ui->zTiltComboBox->setCurrentIndex(z);
    ui->zoomSlider->setValue(zoom);
    ui->yaxSlider->setValue(yax);
}

void Controls::saveModelDefaults(const QString &model)
{
    QSettings settings;
    settings.beginGroup(model);
    settings.setValue("UI_XTiltComboBox", ui->xTiltComboBox->currentIndex());
    settings.setValue("UI_YTiltComboBox", ui->yTiltComboBox->currentIndex());
    settings.setValue("UI_ZTiltComboBox", ui->zTiltComboBox->currentIndex());
    settings.setValue("UI_ZoomSlider", ui->zoomSlider->value());
    settings.setValue("UI_YAxSlider", ui->yaxSlider->value());
}

void Controls::on_selectColorButton_clicked()
{
    QColor color = QColorDialog::getColor(mSelectColor, this);
    if (color.isValid() && color != mSelectColor)
    {
        mSelectColor = color;
        emit updateSelectColor(color);
        QPixmap px(32, 32);
        px.fill(color);
        ui->selectColorButton->setIcon(QIcon(px));
    }
}

void Controls::on_spinCheckBox_stateChanged(int state)
{
    mView->enableAnimation(state == Qt::Checked);
}

void Controls::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 QDir::homePath(),
                                                 tr("Models (*.3ds *.obj)"));
    if (fileName.isEmpty())
        return;
    setWindowTitle(fileName);
    ui->yaxSlider->setValue(0);
    ui->zoomSlider->setValue(15);
    ui->xTiltComboBox->setCurrentIndex(6);
    ui->yTiltComboBox->setCurrentIndex(0);
    ui->zTiltComboBox->setCurrentIndex(0);
    emit openFile(fileName);
}

void Controls::on_actionComponent_triggered()
{
    OptionsDialog *od = new OptionsDialog(mView->currentModel(), mView->components(), this);
    od->setAttribute(Qt::WA_DeleteOnClose);
    int result = od->exec();
    if (result == QDialog::Accepted)
    {
        emit openFile(mView->currentModel());
    }
}

void Controls::optionMenuToggled(bool checked)
{
    QAction *act = qobject_cast<QAction *>(sender());
    Q_ASSERT(act);
    if (act == ui->actionForce_Smooth && checked && ui->actionForce_Faceted->isChecked())
        ui->actionForce_Faceted->setChecked(false);
    if (act == ui->actionForce_Faceted && checked && ui->actionForce_Smooth->isChecked())
        ui->actionForce_Smooth->setChecked(false);
    saveSettings(mView->currentModel());
    emit openFile(mView->currentModel());
}

void Controls::saveSettings(const QString &model)
{
    QSettings settings;
    settings.beginGroup(model);
    bool forceSmooth = ui->actionForce_Smooth->isChecked();
    settings.setValue("ForceSmooth", forceSmooth);
    bool forceFaceted = ui->actionForce_Faceted->isChecked();
    settings.setValue("ForceFaceted", forceFaceted);
    bool nativeIndices = ui->actionNative_Indices->isChecked();
    settings.setValue("NativeIndices", nativeIndices);
    bool correctNormals = ui->actionCorrect_Normals->isChecked();
    settings.setValue("CorrectNormals", correctNormals);
    bool correctAcute = ui->actionCorrect_Acute->isChecked();
    settings.setValue("CorrectAcute", correctAcute);
    bool showWarnings = ui->actionShow_Warnings->isChecked();
    settings.setValue("ShowWarnings", showWarnings);
}

void Controls::loadSettings(const QString &model)
{
    QSettings settings;
    settings.beginGroup(model);
    bool forceSmooth = settings.value("ForceSmooth", false).toBool();
    ui->actionForce_Smooth->setChecked(forceSmooth);
    bool forceFaceted = settings.value("ForceFaceted", false).toBool();
    ui->actionForce_Faceted->setChecked(forceFaceted);
    bool nativeIndices = settings.value("NativeIndices", false).toBool();
    ui->actionNative_Indices->setChecked(nativeIndices);
    bool correctNormals = settings.value("CorrectNormals", false).toBool();
    ui->actionCorrect_Normals->setChecked(correctNormals);
    bool correctAcute = settings.value("CorrectAcute", false).toBool();
    ui->actionCorrect_Acute->setChecked(correctAcute);
    bool showWarnings = settings.value("ShowWarnings", false).toBool();
    ui->actionShow_Warnings->setChecked(showWarnings);
}

void Controls::on_actionQuit_triggered()
{
    QString model = mView->currentModel();
    saveSettings(model);
    saveModelDefaults(model);
    close();
}

void Controls::on_actionSave_QML_triggered()
{
    QString modelFileName = mView->currentModel();
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
        QString options = Viewer::getOptions(mView->currentModel());
        if (!options.isEmpty())
            gen.setProperty("options", options);
        gen.save();
    }
}
