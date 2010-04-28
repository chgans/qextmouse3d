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

#ifndef CONTROLS_H
#define CONTROLS_H

#include <QtGui/QMainWindow>

namespace Ui {
    class Controls;
}

class Viewer;

class Controls : public QMainWindow {
    Q_OBJECT
public:
    Controls(QWidget *parent = 0);
    ~Controls();

public slots:
    void signalColor(const QColor &);
    void loadModelDefaults(const QString &);
    void saveModelDefaults(const QString &);

signals:
    void updateSelectColor(const QColor &);
    void openFile(const QString &);

protected:
    void changeEvent(QEvent *e);

private:
    QString populateModelMenu();

    Ui::Controls *ui;
    Viewer *mView;
    QColor mSelectColor;

private slots:
    void on_actionSave_QML_triggered();
    void on_actionQuit_triggered();
    void on_actionComponent_triggered();
    void on_actionOpen_triggered();
    void on_spinCheckBox_stateChanged(int );
    void on_selectColorButton_clicked();
    void optionMenuToggled(bool);
    void saveSettings(const QString &);
    void loadSettings(const QString &);
};

#endif // CONTROLS_H
