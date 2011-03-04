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

#ifndef CONTROLS_H
#define CONTROLS_H

#include <QtGui/QMainWindow>

namespace Ui {
    class Controls;
}

class Viewer;
class Model;
class QLabel;

class Controls : public QMainWindow {
    Q_OBJECT
public:
    Controls(QWidget *parent = 0);
    ~Controls();

public slots:
    void loadModelDefaults(const QString &);
    void saveModelDefaults(const QString &);

signals:
    void openFile(const QString &);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);

private slots:
    void on_actionShow_Picking_triggered();
    void on_lockButton_clicked();
    void on_zScaleSpin_valueChanged(QString );
    void on_yScaleSpin_valueChanged(QString );
    void on_xScaleSpin_valueChanged(QString );
    void on_zTranSpin_valueChanged(QString );
    void on_yTranSpin_valueChanged(QString );
    void on_xTranSpin_valueChanged(QString );
    void on_zRotSpin_valueChanged(QString );
    void on_yRotSpin_valueChanged(QString );
    void on_xRotSpin_valueChanged(QString );
    void on_action100_x_triggered(bool checked);
    void on_action10_x_triggered(bool checked);
    void on_action1_x_triggered(bool checked);
    void setManualControl();
    void on_actionShow_Floor_triggered();
    void on_actionSpin_triggered();
    void on_actionComponent_triggered();
    void on_actionSave_QML_triggered();
    void on_actionQuit_triggered();
    void on_actionOpen_triggered();
    void optionMenuToggled(bool);
    void saveSettings(const QString &);
    void loadSettings(const QString &);
    void triangleCountUpdated(int);
    void fileLoadTimeNotified(int);
    void addRecentFiles(const QString &);
    void load();
    void viewTypeChanged();
    void front_view_triggered();
    void back_view_triggered();
    void top_view_triggered();
    void bottom_view_triggered();
    void left_view_triggered();
    void right_view_triggered();
    void frontquarter_view_triggered();
    void rearquarter_view_triggered();

    void on_actionCenter_View_triggered();

private:
    QString populateModelMenu();

    Ui::Controls *m_ui;
    Viewer *m_view;
    Model *m_model;
    QLabel *m_triangleCount;
    bool m_scaleLinked;
};

#endif // CONTROLS_H
