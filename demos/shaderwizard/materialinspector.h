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
