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
    , m_scaleLinked(true)
{
    m_ui->setupUi(this);

    m_view = new Viewer(m_ui->frame);
    QHBoxLayout *lay = new QHBoxLayout();
    lay->addWidget(m_view);
    m_ui->frame->setLayout(lay);
    connect(m_view, SIGNAL(manualControlEngaged()),
            this, SLOT(setManualControl()));

    QString initialModel = populateModelMenu();

    m_model = new Model(this);

    connect(this, SIGNAL(openFile(QString)),
            m_model, SLOT(setFullPath(QString)));
    connect(this, SIGNAL(openFile(QString)),
            this, SLOT(addRecentFiles(QString)));

    connect(m_model, SIGNAL(modelLoaded(QString)),
            this, SLOT(loadModelDefaults(QString)));
    connect(m_model, SIGNAL(modelLoaded(QString)),
            this, SLOT(loadSettings(QString)));
    connect(m_model, SIGNAL(modelLoaded(QString)),
            this, SLOT(setWindowTitle(QString)));
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

    m_view->setModel(m_model);
    m_ui->treeView->setModel(m_model);
    m_view->setTreeView(m_ui->treeView);

    connect(m_view, SIGNAL(viewTypeChanged()), this, SLOT(viewTypeChanged()));

    emit openFile(initialModel);

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

    connect(m_ui->actionFront, SIGNAL(triggered(bool)),
            this, SLOT(front_view_triggered()));
    connect(m_ui->actionBack, SIGNAL(triggered(bool)),
            this, SLOT(back_view_triggered()));
    connect(m_ui->actionTop, SIGNAL(triggered(bool)),
            this, SLOT(top_view_triggered()));
    connect(m_ui->actionBottom, SIGNAL(triggered(bool)),
            this, SLOT(bottom_view_triggered()));
    connect(m_ui->actionLeft, SIGNAL(triggered(bool)),
            this, SLOT(left_view_triggered()));
    connect(m_ui->actionRight, SIGNAL(triggered(bool)),
            this, SLOT(right_view_triggered()));
    connect(m_ui->actionFront_Quarter, SIGNAL(triggered(bool)),
            this, SLOT(frontquarter_view_triggered()));
    connect(m_ui->actionBack_Quarter, SIGNAL(triggered(bool)),
            this, SLOT(rearquarter_view_triggered()));

    m_ui->actionShow_Floor->setChecked(true);
}

Controls::~Controls()
{
    delete m_ui;
}

void Controls::front_view_triggered() {
    m_view->setView(Viewer::FrontView);
}

void Controls::back_view_triggered() {
    m_view->setView(Viewer::BackView);
}

void Controls::top_view_triggered() {
    m_view->setView(Viewer::TopView);
}

void Controls::bottom_view_triggered() {
    m_view->setView(Viewer::BottomView);
}

void Controls::left_view_triggered() {
    m_view->setView(Viewer::LeftView);
}

void Controls::right_view_triggered() {
    m_view->setView(Viewer::RightView);
}

void Controls::frontquarter_view_triggered() {
    m_view->setView(Viewer::FrontRightView);
}

void Controls::rearquarter_view_triggered() {
    m_view->setView(Viewer::BackLeftView);
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

void Controls::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    QString model = m_model->fullPath();
    saveSettings(model);
    saveModelDefaults(model);
}

void Controls::load()
{
    QAction *act = qobject_cast<QAction*>(sender());
    if (act)
    {
        m_model->setFullPath(act->text());
    }
}

void Controls::viewTypeChanged()
{
    switch (m_view->view()) {
    case Viewer::SelectView:
        m_ui->actionFront->setChecked(false);
        m_ui->actionBack->setChecked(false);
        m_ui->actionTop->setChecked(false);
        m_ui->actionBottom->setChecked(false);
        m_ui->actionLeft->setChecked(false);
        m_ui->actionRight->setChecked(false);
        m_ui->actionFront_Quarter->setChecked(false);
        m_ui->actionBack_Quarter->setChecked(false);
        break;
    case Viewer::FrontView:
        m_ui->actionFront->setChecked(true);
        m_ui->actionBack->setChecked(false);
        m_ui->actionTop->setChecked(false);
        m_ui->actionBottom->setChecked(false);
        m_ui->actionLeft->setChecked(false);
        m_ui->actionRight->setChecked(false);
        m_ui->actionFront_Quarter->setChecked(false);
        m_ui->actionBack_Quarter->setChecked(false);
        break;
    case Viewer::BackView:
        m_ui->actionFront->setChecked(false);
        m_ui->actionBack->setChecked(true);
        m_ui->actionTop->setChecked(false);
        m_ui->actionBottom->setChecked(false);
        m_ui->actionLeft->setChecked(false);
        m_ui->actionRight->setChecked(false);
        m_ui->actionFront_Quarter->setChecked(false);
        m_ui->actionBack_Quarter->setChecked(false);
        break;
    case Viewer::TopView:
        m_ui->actionFront->setChecked(false);
        m_ui->actionBack->setChecked(false);
        m_ui->actionTop->setChecked(true);
        m_ui->actionBottom->setChecked(false);
        m_ui->actionLeft->setChecked(false);
        m_ui->actionRight->setChecked(false);
        m_ui->actionFront_Quarter->setChecked(false);
        m_ui->actionBack_Quarter->setChecked(false);
        break;
    case Viewer::BottomView:
        m_ui->actionFront->setChecked(false);
        m_ui->actionBack->setChecked(false);
        m_ui->actionTop->setChecked(false);
        m_ui->actionBottom->setChecked(true);
        m_ui->actionLeft->setChecked(false);
        m_ui->actionRight->setChecked(false);
        m_ui->actionFront_Quarter->setChecked(false);
        m_ui->actionBack_Quarter->setChecked(false);
        break;
    case Viewer::LeftView:
        m_ui->actionFront->setChecked(false);
        m_ui->actionBack->setChecked(false);
        m_ui->actionTop->setChecked(false);
        m_ui->actionBottom->setChecked(false);
        m_ui->actionLeft->setChecked(true);
        m_ui->actionRight->setChecked(false);
        m_ui->actionFront_Quarter->setChecked(false);
        m_ui->actionBack_Quarter->setChecked(false);
        break;
    case Viewer::RightView:
        m_ui->actionFront->setChecked(false);
        m_ui->actionBack->setChecked(false);
        m_ui->actionTop->setChecked(false);
        m_ui->actionBottom->setChecked(false);
        m_ui->actionLeft->setChecked(false);
        m_ui->actionRight->setChecked(true);
        m_ui->actionFront_Quarter->setChecked(false);
        m_ui->actionBack_Quarter->setChecked(false);
        break;
    case Viewer::FrontRightView:
        m_ui->actionFront->setChecked(false);
        m_ui->actionBack->setChecked(false);
        m_ui->actionTop->setChecked(false);
        m_ui->actionBottom->setChecked(false);
        m_ui->actionLeft->setChecked(false);
        m_ui->actionRight->setChecked(false);
        m_ui->actionFront_Quarter->setChecked(true);
        m_ui->actionBack_Quarter->setChecked(false);
        break;
    case Viewer::BackLeftView:
        m_ui->actionFront->setChecked(false);
        m_ui->actionBack->setChecked(false);
        m_ui->actionTop->setChecked(false);
        m_ui->actionBottom->setChecked(false);
        m_ui->actionLeft->setChecked(false);
        m_ui->actionRight->setChecked(false);
        m_ui->actionFront_Quarter->setChecked(false);
        m_ui->actionBack_Quarter->setChecked(true);
        break;
    case Viewer::Centered:
        m_ui->actionFront->setChecked(false);
        m_ui->actionBack->setChecked(false);
        m_ui->actionTop->setChecked(false);
        m_ui->actionBottom->setChecked(false);
        m_ui->actionLeft->setChecked(false);
        m_ui->actionRight->setChecked(false);
        m_ui->actionFront_Quarter->setChecked(false);
        m_ui->actionBack_Quarter->setChecked(false);
        break;
    }
}

QString Controls::populateModelMenu()
{
    QString cmdlineModel;
    QSettings settings;
    settings.beginGroup(QLatin1String("General"));
    cmdlineModel = settings.value(QLatin1String("LastModel")).toString();
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
    return QLatin1String(":/trolltech-q.3ds");
}

void Controls::loadModelDefaults(const QString &model)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("ModelDefaults"));

    QByteArray coded = QUrl::toPercentEncoding(model);
    QString modelEncoded = QString::fromLatin1(coded.constData());

    QVector3D p, o, s, e, up;
    bool spin = false;
    if (settings.childGroups().contains(modelEncoded))
    {
        settings.beginGroup(modelEncoded);
        p = qvariant_cast<QVector3D>(settings.value(QLatin1String("position"), QVector3D()));
        m_view->setPosition(p);
        o = qvariant_cast<QVector3D>(settings.value(QLatin1String("orientation"), QVector3D()));
        m_view->setOrientation(o);
        s = qvariant_cast<QVector3D>(settings.value(QLatin1String("scale"), QVector3D()));
        m_view->setScale(s);
        e = QVector3D(0.0f, 2.0f, -10.0f);
        e = qvariant_cast<QVector3D>(settings.value(QLatin1String("eye"), e));
        up = QVector3D(0.0f, 1.0f, 0.0f);
        up = qvariant_cast<QVector3D>(settings.value(QLatin1String("upVector"), up));
        m_view->camera()->setEye(e);
        m_view->camera()->setUpVector(up);
       // spin = settings.value(QLatin1String("spin"), false).toBool();
		bool showFloor = settings.value(QLatin1String("ShowFloor"), false).toBool();
		m_ui->actionShow_Floor->setChecked(showFloor);
		on_actionShow_Floor_triggered();
		bool spinView = settings.value(QLatin1String("spin"), false).toBool();
		m_ui->actionSpin->setChecked(spinView);
		on_actionSpin_triggered();
    }
    else
    {
        QBox3D box = m_model->scene()->boundingBox();
        m_view->setPosition(-box.center());
        QVector3D ext = box.size();
        qreal maxDimension = qMax(ext.x(), qMax(ext.y(), ext.z()));
        e.setZ(-qMax(qreal(10.0f), maxDimension));
        e.setY(qAbs(e.z()) / 5.0f);
        m_view->camera()->setEye(e);
    }
    m_ui->xTranSpin->setValue(p.x());
    m_ui->yTranSpin->setValue(p.y());
    m_ui->zTranSpin->setValue(p.z());
    m_ui->xRotSpin->setValue(o.x());
    m_ui->yRotSpin->setValue(o.y());
    m_ui->zRotSpin->setValue(o.z());
    m_ui->xScaleSpin->setValue(s.x());
    m_ui->yScaleSpin->setValue(s.y());
    m_ui->zScaleSpin->setValue(s.z());
}

void Controls::saveModelDefaults(const QString &model)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("General"));
    settings.setValue(QLatin1String("LastModel"), model);
    settings.endGroup();
    settings.beginGroup(QLatin1String("ModelDefaults"));

    QByteArray coded = QUrl::toPercentEncoding(model);
    QString modelEncoded = QString::fromLatin1(coded.constData());
    settings.beginGroup(modelEncoded);

    QVector3D p = m_view->position();
    QVector3D o = m_view->orientation();
    QVector3D s = m_view->scale();
    QVector3D e = m_view->camera()->eye();
    QVector3D up = m_view->camera()->upVector();
    settings.setValue(QLatin1String("position"), p);
    settings.setValue(QLatin1String("orientation"), o);
    settings.setValue(QLatin1String("scale"), s);
    settings.setValue(QLatin1String("eye"), e);
    settings.setValue(QLatin1String("upVector"), up);
  //  settings.setValue(QLatin1String("spin"), m_ui->actionSpin->isChecked());
	bool showFloor = m_ui->actionShow_Floor->isChecked();
	settings.setValue(QLatin1String("ShowFloor"), showFloor);
	bool spinView = m_ui->actionSpin->isChecked();
	settings.setValue(QLatin1String("spin"), spinView);
}

void Controls::on_actionOpen_triggered()
{
    QFileDialog::Options options = 0;
#ifdef Q_OS_LINUX
    // work around data loss bug with KDE: https://bugs.kde.org/show_bug.cgi?id=210904
    options = QFileDialog::DontUseNativeDialog;
#endif
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 QDir::homePath(),
                                                 tr("Models (*.3ds *.obj *.dae)"), 0, options);
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
    settings.beginGroup(QLatin1String("ModelSettings"));

    QByteArray coded = QUrl::toPercentEncoding(model);
    QString modelEncoded = QString::fromLatin1(coded.constData());
    settings.beginGroup(modelEncoded);

    bool forceSmooth = m_ui->actionForce_Smooth->isChecked();
    settings.setValue(QLatin1String("ForceSmooth"), forceSmooth);
    bool forceFaceted = m_ui->actionForce_Faceted->isChecked();
    settings.setValue(QLatin1String("ForceFaceted"), forceFaceted);
    bool nativeIndices = m_ui->actionNative_Indices->isChecked();
    settings.setValue(QLatin1String("NativeIndices"), nativeIndices);
    bool correctNormals = m_ui->actionCorrect_Normals->isChecked();
    settings.setValue(QLatin1String("CorrectNormals"), correctNormals);
    bool correctAcute = m_ui->actionCorrect_Acute->isChecked();
    settings.setValue(QLatin1String("CorrectAcute"), correctAcute);
    bool showWarnings = m_ui->actionShow_Warnings->isChecked();
    settings.setValue(QLatin1String("ShowWarnings"), showWarnings);
	
}

void Controls::loadSettings(const QString &model)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("ModelSettings"));

    QByteArray coded = QUrl::toPercentEncoding(model);
    QString modelEncoded = QString::fromLatin1(coded.constData());
    settings.beginGroup(modelEncoded);

    bool forceSmooth = settings.value(QLatin1String("ForceSmooth"), false).toBool();
    m_ui->actionForce_Smooth->setChecked(forceSmooth);
    bool forceFaceted = settings.value(QLatin1String("ForceFaceted"), false).toBool();
    m_ui->actionForce_Faceted->setChecked(forceFaceted);
    bool nativeIndices = settings.value(QLatin1String("NativeIndices"), false).toBool();
    m_ui->actionNative_Indices->setChecked(nativeIndices);
    bool correctNormals = settings.value(QLatin1String("CorrectNormals"), false).toBool();
    m_ui->actionCorrect_Normals->setChecked(correctNormals);
    bool correctAcute = settings.value(QLatin1String("CorrectAcute"), false).toBool();
    m_ui->actionCorrect_Acute->setChecked(correctAcute);
    bool showWarnings = settings.value(QLatin1String("ShowWarnings"), false).toBool();
    m_ui->actionShow_Warnings->setChecked(showWarnings);
}

void Controls::addRecentFiles(const QString &fileName)
{
    QMenu *rf = m_ui->menuRecent_Models;
    QStringList files;
    {
        QSettings settings;
        settings.beginGroup(QLatin1String("General"));
        int numFiles = settings.beginReadArray(QLatin1String("RecentFiles"));
        for (int i = 0; i < numFiles; ++i)
        {
            settings.setArrayIndex(i);
            QByteArray coded = settings.value(QLatin1String("file")).toByteArray();
            files.append(QUrl::fromPercentEncoding(coded));
        }
    }
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
    {
        QSettings settings;
        settings.beginGroup(QLatin1String("General"));
        settings.beginWriteArray(QLatin1String("RecentFiles"), files.count());
        for (int i = 0; i < files.count(); ++i)
        {
            settings.setArrayIndex(i);
            QByteArray coded = QUrl::toPercentEncoding(files.at(i));
            settings.setValue(QLatin1String("file"), coded);
        }
        settings.endArray();
        settings.endGroup();
        settings.sync();
    }
}

void Controls::on_actionQuit_triggered()
{
    close();
}

void Controls::on_actionSave_QML_triggered()
{
    QString modelFileName = m_model->fullPath();
    QFileInfo fi(modelFileName);
    QString qmlName = fi.baseName();
    qmlName = qmlName.mid(1).prepend(qmlName[0].toUpper()) + QLatin1String(".qml");
    QString path = fi.absoluteDir().absoluteFilePath(qmlName);
    QFileDialog::Options options = 0;
#ifdef Q_OS_LINUX
    // work around data loss bug with KDE: https://bugs.kde.org/show_bug.cgi?id=210904
    options = QFileDialog::DontUseNativeDialog;
#endif
    QString file = QFileDialog::getSaveFileName(this, tr("Save QML file"), path,
                                                tr("QML files (*.qml)"), 0, options);
    if (!file.isEmpty())
    {
        QFileInfo fi2(file);
        QDir d = fi2.absoluteDir();
        QString relName = d.relativeFilePath(modelFileName);
        QString n2 = fi2.baseName();
        n2 = n2.mid(1).prepend((n2[0].toUpper())) + QLatin1String(".qml");
        file = fi2.absolutePath() + QLatin1Char('/') + n2;
        QmlGenerator gen(file);
        gen.setProperty(QLatin1String("modelFileName"), relName);
        saveSettings(modelFileName);
        QString options = m_model->getOptions();
        if (!options.isEmpty())
            gen.setProperty(QLatin1String("options"), options);
        QVector3D p = m_view->position();
        if (!qFuzzyIsNull(p.x()))
            gen.setProperty(QLatin1String("x_translation"), QString::number(p.x()));
        if (!qFuzzyIsNull(p.y()))
            gen.setProperty(QLatin1String("y_translation"), QString::number(p.y()));
        if (!qFuzzyIsNull(p.z()))
            gen.setProperty(QLatin1String("z_translation"), QString::number(p.z()));
        QVector3D o = m_view->orientation();
        if (!qFuzzyIsNull(o.x()))
            gen.setProperty(QLatin1String("x_rotation"), QString::number(o.x()));
        if (!qFuzzyIsNull(o.y()))
            gen.setProperty(QLatin1String("y_rotation"), QString::number(o.y()));
        if (!qFuzzyIsNull(o.z()))
            gen.setProperty(QLatin1String("z_rotation"), QString::number(o.z()));
        QVector3D s = m_view->scale();
        if (!qFuzzyIsNull(s.x()))
            gen.setProperty(QLatin1String("x_scale"), ((s.x() < 0.0f) ? QString::number(1.0f / qAbs(s.x())) : QString::number(s.x())));
        if (!qFuzzyIsNull(s.y()))
            gen.setProperty(QLatin1String("y_scale"), ((s.y() < 0.0f) ? QString::number(1.0f / qAbs(s.y())) : QString::number(s.y())));
        if (!qFuzzyIsNull(s.z()))
            gen.setProperty(QLatin1String("z_scale"), ((s.z() < 0.0f) ? QString::number(1.0f / qAbs(s.z())) : QString::number(s.z())));
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

void Controls::setManualControl()
{
    m_ui->actionSpin->setChecked(false);
}

void Controls::on_actionSpin_triggered()
{
    m_view->enableAnimation(m_ui->actionSpin->isChecked());
}

void Controls::on_actionShow_Floor_triggered()
{
    m_view->setFloorEnabled(m_ui->actionShow_Floor->isChecked());
}

void Controls::on_action1_x_triggered(bool checked)
{
    if (checked)
    {
        m_view->setZoomScale(1);
    }
}

void Controls::on_action10_x_triggered(bool checked)
{
    if (checked)
    {
        m_view->setZoomScale(10);
    }
}

void Controls::on_action100_x_triggered(bool checked)
{
    if (checked)
    {
        m_view->setZoomScale(100);
    }
}

void Controls::on_xRotSpin_valueChanged(QString value)
{
    Q_UNUSED(value);
    QVector3D o = m_view->orientation();
    o.setX(m_ui->xRotSpin->value());
    m_view->setOrientation(o);
}

void Controls::on_yRotSpin_valueChanged(QString )
{
    QVector3D o = m_view->orientation();
    o.setY(m_ui->yRotSpin->value());
    m_view->setOrientation(o);
}

void Controls::on_zRotSpin_valueChanged(QString )
{
    QVector3D o = m_view->orientation();
    o.setZ(m_ui->zRotSpin->value());
    m_view->setOrientation(o);
}

void Controls::on_xTranSpin_valueChanged(QString )
{
    QVector3D p = m_view->position();
    p.setX(m_ui->xTranSpin->value());
    m_view->setPosition(p);
}

void Controls::on_yTranSpin_valueChanged(QString )
{
    QVector3D p = m_view->position();
    p.setY(m_ui->yTranSpin->value());
    m_view->setPosition(p);
}

void Controls::on_zTranSpin_valueChanged(QString )
{
    QVector3D p = m_view->position();
    p.setZ(m_ui->zTranSpin->value());
    m_view->setPosition(p);
}

void Controls::on_xScaleSpin_valueChanged(QString )
{
    QVector3D s = m_view->scale();
    qreal value = m_ui->xScaleSpin->value();
    if (s.x() != value)
    {
        if (m_scaleLinked)
        {
            s = QVector3D(value, value, value);
            m_view->setScale(s);
            m_ui->yScaleSpin->setValue(value);
            m_ui->zScaleSpin->setValue(value);
        }
        else
        {
            s.setX(value);
            m_view->setScale(s);
        }
    }
}

void Controls::on_yScaleSpin_valueChanged(QString )
{
    QVector3D s = m_view->scale();
    qreal value = m_ui->yScaleSpin->value();
    if (s.y() != value)
    {
        if (m_scaleLinked)
        {
            s = QVector3D(value, value, value);
            m_view->setScale(s);
            m_ui->xScaleSpin->setValue(value);
            m_ui->zScaleSpin->setValue(value);
        }
        else
        {
            s.setY(value);
            m_view->setScale(s);
        }
    }
}

void Controls::on_zScaleSpin_valueChanged(QString )
{
    QVector3D s = m_view->scale();
    qreal value = m_ui->zScaleSpin->value();
    if (s.z() != value)
    {
        if (m_scaleLinked)
        {
            s = QVector3D(value, value, value);
            m_view->setScale(s);
            m_ui->xScaleSpin->setValue(value);
            m_ui->yScaleSpin->setValue(value);
        }
        else
        {
            s.setZ(value);
            m_view->setScale(s);
        }
    }
}

void Controls::on_lockButton_clicked()
{
    if (m_scaleLinked)
    {
        m_ui->lockButton->setIcon(QIcon(QLatin1String(":/images/linked-broken-32x32.png")));
        m_ui->lockButton->setToolTip(tr("Click to link X, Y & Z scale values"));
        m_scaleLinked = false;
    }
    else
    {
        m_ui->lockButton->setIcon(QIcon(QLatin1String(":/images/linked-32x32.png")));
        m_ui->lockButton->setToolTip(tr("Click to unlink X, Y & Z scale values"));
        m_scaleLinked = true;
        m_ui->yScaleSpin->setValue(m_ui->xScaleSpin->value());
        m_ui->zScaleSpin->setValue(m_ui->xScaleSpin->value());
    }
}

void Controls::on_actionShow_Picking_triggered()
{
    m_view->setOption(QGLView::ShowPicking, m_ui->actionShow_Picking->isChecked());
}

void Controls::on_actionCenter_View_triggered()
{
    m_view->setView(Viewer::Centered);
}
