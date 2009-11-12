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
#include "cubeview.h"

#include <QDir>
#include <QColorDialog>
#include <QFileDialog>

#include <QtCore/qdebug.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mUi(new Ui::MainWindow)
    , mView(0)
    , mSelectColor(QColor(Qt::blue))
{
    mUi->setupUi(this);
    mView = new CubeView(mUi->frame);
    QHBoxLayout *lay = new QHBoxLayout();
    lay->addWidget(mView);
    mUi->frame->setLayout(lay);
    connect(mUi->xTiltComboBox, SIGNAL(currentIndexChanged(int)),
            mView, SLOT(xTiltChanged(int)));
    connect(mUi->yTiltComboBox, SIGNAL(currentIndexChanged(int)),
            mView, SLOT(yTiltChanged(int)));
    connect(mUi->zTiltComboBox, SIGNAL(currentIndexChanged(int)),
            mView, SLOT(zTiltChanged(int)));
    connect(mUi->zoomSlider, SIGNAL(sliderReleased()),
            mView, SLOT(zoomChanged()));
    connect(mUi->yaxSlider, SIGNAL(sliderReleased()),
            mView, SLOT(yaxChanged()));
    mView->setColorMenu(mUi->menuColors);
    mView->setComponentMenu(mUi->menuComponents);
    connect(mUi->actionQuit, SIGNAL(triggered()),
            this, SLOT(close()));
    connect(mUi->actionSelect_None, SIGNAL(triggered()),
            mView, SLOT(selectComponent()));
    connect(mView, SIGNAL(colorUpdate(QColor)),
            this, SLOT(signalColor(QColor)));
    connect(this, SIGNAL(updateSelectColor(QColor)),
            mView, SLOT(selectColorChanged(QColor)));
    connect(this, SIGNAL(openFile(QString)),
            mView, SLOT(openModelFile(QString)));
    QPixmap px(32, 32);
    px.fill(QColor(mSelectColor));
    mUi->selectColorButton->setIcon(QIcon(px));
    populateModelMenu();
}

MainWindow::~MainWindow()
{
    delete mUi;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        mUi->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::populateModelMenu()
{
    QMenu *menu = mUi->menuModels;
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
            QAction *act = new QAction(name, this);
            menu->addAction(act);
            QObject::connect(act, SIGNAL(triggered()),
                             mView, SLOT(load()));
        }
    }
}

void MainWindow::signalColor(const QColor &color)
{
    QPixmap px(32, 32);
    px.fill(color);
    mUi->selectColorButton->setIcon(QIcon(px));
}

void MainWindow::on_selectColorButton_clicked()
{
    QColor color = QColorDialog::getColor(mSelectColor, this);
    if (color.isValid() && color != mSelectColor)
    {
        mSelectColor = color;
        emit updateSelectColor(color);
        QPixmap px(32, 32);
        px.fill(color);
        mUi->selectColorButton->setIcon(QIcon(px));
    }
}

void MainWindow::on_spinCheckBox_stateChanged(int state)
{
    mView->enableAnimation(state == Qt::Checked);
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 QDir::homePath(),
                                                 tr("Models (*.3ds *.obj)"));
    if (fileName.isEmpty())
        return;
    setWindowTitle(fileName);
    mUi->yaxSlider->setValue(0);
    mUi->zoomSlider->setValue(15);
    mUi->xTiltComboBox->setCurrentIndex(6);
    mUi->yTiltComboBox->setCurrentIndex(0);
    mUi->zTiltComboBox->setCurrentIndex(0);
    emit openFile(fileName);
}
