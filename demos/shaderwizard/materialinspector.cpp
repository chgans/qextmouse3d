/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "materialinspector.h"
#include "ui_materialinspector.h"

#include <QColorDialog>
#include <QImageReader>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

MaterialInspector::MaterialInspector(QWidget *parent) :
    QWidget(parent)
    , m_ui(new Ui::MaterialInspector)
    , dialog(0)
    , target(NoTarget)
    , backupColor()
    , textureLabel(new QLabel)
{
    m_ui->setupUi(this);
    connect(m_ui->ambientButton, SIGNAL(clicked()), this, SLOT(ambientColorDialog()));
    connect(m_ui->diffuseButton, SIGNAL(clicked()), this, SLOT(diffuseColorDialog()));
    connect(m_ui->specularButton, SIGNAL(clicked()), this, SLOT(specularColorDialog()));

    connect(m_ui->textureButton, SIGNAL(clicked()), this, SLOT(selectTexture()));
    connect(m_ui->shininessSlider, SIGNAL(valueChanged(int)), m_ui->shininessSpinBox, SLOT(setValue(int)));
    connect(m_ui->shininessSpinBox, SIGNAL(valueChanged(int)), m_ui->shininessSlider, SLOT(setValue(int)));
    connect(m_ui->shininessSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(shininessChanged(int)));

    textureLabel->setScaledContents(true);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(textureLabel);
    m_ui->textureButton->setLayout(layout);

    dialog = new QColorDialog();
    dialog->setOption(QColorDialog::ShowAlphaChannel, true);
    connect( dialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(setColor(QColor)));
    connect( dialog, SIGNAL(rejected()), this, SLOT(restoreColor()));
}

MaterialInspector::~MaterialInspector()
{
    delete m_ui;
    delete dialog;
    dialog = 0;
}

void MaterialInspector::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MaterialInspector::setMaterial(QGLMaterial* newMaterial)
{
    currentMaterial = newMaterial;

    m_ui->ambientButton->setPalette( QPalette( QColor(newMaterial->ambientColor() )));
    m_ui->diffuseButton->setPalette( QPalette( QColor(newMaterial->diffuseColor() )));
    m_ui->specularButton->setPalette( QPalette( QColor(newMaterial->specularColor() )));
}

void MaterialInspector::setAmbientColor(QColor color)
{
    m_ui->ambientButton->setPalette( QPalette(color) );
    if(currentMaterial)
    {
        currentMaterial->setAmbientColor(color);
    }
    emit ambientColorChanged(color);
}

void MaterialInspector::setDiffuseColor(QColor color)
{
    m_ui->diffuseButton->setPalette(QPalette(color));
    if(currentMaterial)
    {
        currentMaterial->setDiffuseColor(color);
    }
    emit diffuseColorChanged(color);
}

void MaterialInspector::setSpecularColor(QColor color)
{
    m_ui->specularButton->setPalette(QPalette(color));
    if(currentMaterial)
    {
        currentMaterial->setSpecularColor(color);
    }
    emit specularColorChanged(color);
}

void MaterialInspector::setShininess(int shininess)
{
    m_ui->shininessSpinBox->setValue(shininess);
    if(currentMaterial)
        currentMaterial->setShininess(shininess);
    emit shininessChanged(shininess);
}

void MaterialInspector::setTexture(const QImage& image)
{
    textureLabel->setPixmap(QPixmap::fromImage(image));
    emit textureChanged(image);
}

void MaterialInspector::on_textureButton_clicked(bool )
{
}

void MaterialInspector::ambientColorDialog()
{
    target = AmbientMaterialTarget;
    backupColor = m_ui->ambientButton->palette().background().color();
    colorDialog();
}

void MaterialInspector::diffuseColorDialog()
{
    target = DiffuseMaterialTarget;
    backupColor = m_ui->diffuseButton->palette().background().color();
    colorDialog();
}

void MaterialInspector::specularColorDialog()
{
    target = SpecularMaterialTarget;
    backupColor = m_ui->specularButton->palette().background().color();
    colorDialog();
}


void MaterialInspector::colorDialog()
{
    dialog->setCurrentColor( backupColor );
    dialog->show();
    dialog->raise();
}

void MaterialInspector::setTarget(SignalTarget target)
{
    this->target = target;
}

void MaterialInspector::setColor(QColor color)
{
    switch(target)
    {
    case( NoTarget) :
        return;
    case( AmbientMaterialTarget ) :
        setAmbientColor(color);
        break;
    case( DiffuseMaterialTarget ) :
        setDiffuseColor(color);
        break;
    case( SpecularMaterialTarget ) :
        setSpecularColor(color);
        m_ui->specularButton->setPalette(QPalette(color));
        break;
    default:
        qWarning() << "Unrecognized target in setColor() : " << target;
    }
}

void MaterialInspector::selectTexture()
{
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    QStringList supportedFormats;
    QByteArray format;
    foreach (format, formats)
        supportedFormats << QLatin1String("*.") + QString::fromLatin1(format.constData()).toLower();

    static QDir path  = QDir::homePath();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Texture Image"),
                            path.path(), tr("Images") + QLatin1String(" (")+ supportedFormats.join(QLatin1String(" ")) + QLatin1Char(')'));
    if (fileName.isEmpty())
        return;

    path = QDir(fileName);
    setTexture(QImage(fileName));

}

void MaterialInspector::restoreColor()
{
    switch(target)
    {
    case( NoTarget) :
        return;
    case( AmbientMaterialTarget ) :
        m_ui->ambientButton->setPalette( backupColor );
        break;
    case( DiffuseMaterialTarget ) :
        m_ui->diffuseButton->setPalette( backupColor );
        break;
    case( SpecularMaterialTarget ) :
        m_ui->specularButton->setPalette( backupColor );
        break;
    default:
        qWarning() << "Unrecognized target in restoreColor() : " << target;
    }
}
