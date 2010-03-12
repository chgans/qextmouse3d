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
    QString format;
    foreach (format, formats)
        supportedFormats << "*." + format.toLower();

    static QDir path  = QDir::homePath();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Texture Image"),
                            path.path(), tr("Images") + " ("+ supportedFormats.join(" ") + ")");
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
