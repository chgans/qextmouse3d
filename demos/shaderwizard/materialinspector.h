#ifndef MATERIALINSPECTOR_H
#define MATERIALINSPECTOR_H

#include <QtGui/QWidget>
#include "qglmaterialparameters.h"
#include <QPointer>
class QPushButton;
class QColorDialog;
#include <QDialog>

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
    void setMaterial(QGLMaterialParameters* newMaterial);
    void setAmbientColor(QColor color);
    void setDiffuseColor(QColor color);
    void setSpecularColor(QColor color);
    void setShininess(int shininess);

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

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::MaterialInspector *m_ui;
    QPointer<QGLMaterialParameters> currentMaterial;
    QColorDialog *dialog;
    SignalTarget target;
    QColor backupColor;

private slots:
    void on_textureButton_clicked(bool checked);
};

#endif // MATERIALINSPECTOR_H
