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

#ifndef MATERIALINSPECTOR_H
#define MATERIALINSPECTOR_H

#include <QtGui/QWidget>
#include "qglmaterial.h"
#include <QPointer>
#include <QDialog>

class QPushButton;
class QColorDialog;
class QLabel;

namespace Ui {
    class MaterialInspector;
}

class MaterialInspector : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(MaterialInspector);

    enum SignalTarget
    {
        NoTarget = 0,
        AmbientMaterialTarget,
        DiffuseMaterialTarget,
        SpecularMaterialTarget,
    };
public:
    explicit MaterialInspector(QWidget *parent = 0);
    virtual ~MaterialInspector();
public slots:
    void setMaterial(QGLMaterial* newMaterial);
    void setAmbientColor(QColor color);
    void setDiffuseColor(QColor color);
    void setSpecularColor(QColor color);
    void setShininess(int shininess);
    void setTexture(const QImage&);

     void colorDialog();
     void setTarget(SignalTarget target);
     void setColor(QColor color);

    void selectTexture();

    void ambientColorDialog();
    void diffuseColorDialog();
    void specularColorDialog();

protected slots:
    void restoreColor();

signals:
    void ambientColorChanged(QColor color);
    void diffuseColorChanged(QColor color);
    void specularColorChanged(QColor color);
    void shininessChanged(int shininess);
    void textureChanged(const QImage& image);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::MaterialInspector *m_ui;
    QPointer<QGLMaterial> currentMaterial;
    QColorDialog *dialog;
    SignalTarget target;
    QColor backupColor;
    QLabel *textureLabel;


private slots:
    void on_textureButton_clicked(bool checked);
};

#endif // MATERIALINSPECTOR_H
