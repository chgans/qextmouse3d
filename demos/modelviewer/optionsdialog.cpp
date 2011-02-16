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

#include "optionsdialog.h"
#include "ui_optionsdialog.h"

#include <QSettings>
#include <QMessageBox>
#include <QStringListModel>

OptionsDialog::OptionsDialog(const QString &name, const QStringList &components, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptionsDialog)
    , m_name(name)
    , m_modified(false)
{
    ui->setupUi(this);
    connect(ui->forceFacetedCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(toggleCheckBox(bool)));
    connect(ui->forceSmoothCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(toggleCheckBox(bool)));
    connect(ui->correctActuteCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(toggleCheckBox(bool)));
    connect(ui->correctNormalsCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(toggleCheckBox(bool)));
    connect(ui->nativeIndicesCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(toggleCheckBox(bool)));
    connect(ui->showWarningsCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(toggleCheckBox(bool)));
    QStringList items = components;
    items.prepend(name);
    QStringListModel *model = new QStringListModel(items, this);
    ui->componentComboBox->setModel(model);
    ui->componentComboBox->setCurrentIndex(0); // should trigger currentIndexChanged
}

OptionsDialog::~OptionsDialog()
{
    if (result() == QDialog::Accepted)
    {
        saveSettings();
    }
    delete ui;
}

void OptionsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void OptionsDialog::saveSettings()
{
    QSettings settings;
    QString group = (m_component == m_name) ? m_name : m_name + QLatin1Char('/') + m_component;
    settings.beginGroup(group);
    bool forceSmooth = ui->forceSmoothCheckBox->isChecked();
    settings.setValue(QLatin1String("ForceSmooth"), forceSmooth);
    bool forceFaceted = ui->forceFacetedCheckBox->isChecked();
    settings.setValue(QLatin1String("ForceFaceted"), forceFaceted);
    bool nativeIndices = ui->nativeIndicesCheckBox->isChecked();
    settings.setValue(QLatin1String("NativeIndices"), nativeIndices);
    bool correctNormals = ui->correctNormalsCheckBox->isChecked();
    settings.setValue(QLatin1String("CorrectNormals"), correctNormals);
    bool correctAcute = ui->correctActuteCheckBox->isChecked();
    settings.setValue(QLatin1String("CorrectAcute"), correctAcute);
    bool showWarnings = ui->showWarningsCheckBox->isChecked();
    settings.setValue(QLatin1String("ShowWarnings"), showWarnings);	
}

void OptionsDialog::loadSettings()
{
    QSettings settings;
    QString group = (m_component == m_name) ? m_name : m_name + QLatin1Char('/') + m_component;
    settings.beginGroup(group);
    bool forceSmooth = settings.value(QLatin1String("ForceSmooth"), false).toBool();
    ui->forceSmoothCheckBox->setChecked(forceSmooth);
    bool forceFaceted = settings.value(QLatin1String("ForceFaceted"), false).toBool();
    ui->forceFacetedCheckBox->setChecked(forceFaceted);
    bool nativeIndices = settings.value(QLatin1String("NativeIndices"), false).toBool();
    ui->nativeIndicesCheckBox->setChecked(nativeIndices);
    bool correctNormals = settings.value(QLatin1String("CorrectNormals"), false).toBool();
    ui->correctNormalsCheckBox->setChecked(correctNormals);
    bool correctAcute = settings.value(QLatin1String("CorrectAcute"), false).toBool();
    ui->correctActuteCheckBox->setChecked(correctAcute);
    bool showWarnings = settings.value(QLatin1String("ShowWarnings"), false).toBool();
    ui->showWarningsCheckBox->setChecked(showWarnings);
    m_modified = false;
}

void OptionsDialog::on_componentComboBox_currentIndexChanged(QString newComponent)
{
    if (m_modified)
    {
        int result = QMessageBox::warning(this, tr("Unsaved changes"),
                             tr("Save the changes made to settings for %1 mesh?").arg(m_component),
                             QMessageBox::Save, QMessageBox::Discard);
        if (result == QMessageBox::Save)
            saveSettings();
    }
    m_component = newComponent;
    loadSettings();
}

void OptionsDialog::toggleCheckBox(bool checked)
{
    QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
    Q_ASSERT(cb);
    if (checked && (cb == ui->forceFacetedCheckBox) && ui->forceSmoothCheckBox->isChecked())
        ui->forceSmoothCheckBox->setChecked(false);
    if (checked && (cb == ui->forceSmoothCheckBox) && ui->forceFacetedCheckBox->isChecked())
        ui->forceFacetedCheckBox->setChecked(false);
    m_modified = true;
}
