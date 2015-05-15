#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qmouse3devent.h"
#include "qmouse3deventprovider.h"
#include "qglnamespace.h"

#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_provider = new QExtMouse3DEventProvider(this);
    m_provider->setWidget(this);
    ui->comboBox->addItems(m_provider->deviceNames());
    connect(m_provider, SIGNAL(availableChanged()),
            this, SLOT(availableChanged()));
    connect(m_provider, SIGNAL(filtersChanged()),
            this, SLOT(filtersChanged()));
    connect(m_provider, SIGNAL(sensitivityChanged()),
            this, SLOT(sensitivityChanged()));
    connect(ui->filterGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateFilters()));
    connect(ui->senseSlider, SIGNAL(valueChanged(int)),
            this, SLOT(updateSensitivity()));

    ui->mtrans->setChecked(m_provider->filters().testFlag(QExtMouse3DEventProvider::Translations));
    ui->mrot->setChecked(m_provider->filters().testFlag(QExtMouse3DEventProvider::Rotations));
    ui->mdom->setChecked(m_provider->filters().testFlag(QExtMouse3DEventProvider::DominantAxis));
    ui->msens->setChecked(m_provider->filters().testFlag(QExtMouse3DEventProvider::Sensitivity));

    ui->ktrans->setChecked(m_provider->keyFilters().testFlag(QExtMouse3DEventProvider::Translations));
    ui->krot->setChecked(m_provider->keyFilters().testFlag(QExtMouse3DEventProvider::Rotations));
    ui->kdom->setChecked(m_provider->keyFilters().testFlag(QExtMouse3DEventProvider::DominantAxis));
    ui->ksens->setChecked(m_provider->keyFilters().testFlag(QExtMouse3DEventProvider::Sensitivity));

    qreal sens = m_provider->sensitivity();
    if (sens >= 1.0)
        ui->senseSlider->setValue(63+int(sens));
    else
        ui->senseSlider->setValue(65-int((1.0/sens)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::availableChanged()
{
    QString line = QString("Available changed: %1").arg(m_provider->deviceNames().join(", "));
    ui->textEdit->append(line);
}

void MainWindow::filtersChanged()
{
    ui->textEdit->append(QString("Filters changed: Mouse=0x%1")
                         .arg(uint(m_provider->filters()), 2, 16, QChar('0')));
    ui->textEdit->append(QString("Filters changed: Key=0x%1")
                         .arg(uint(m_provider->filters()), 2, 16, QChar('0')));

}

void MainWindow::sensitivityChanged()
{
    ui->textEdit->append(QString("Sensitivity changed: %1").arg(m_provider->sensitivity()));
}

void MainWindow::updateFilters()
{
    QExtMouse3DEventProvider::Filters filters;

    filters = QExtMouse3DEventProvider::NoFilters;
    if (ui->mtrans->isChecked())
        filters |= QExtMouse3DEventProvider::Translations;
    if (ui->mrot->isChecked())
        filters |= QExtMouse3DEventProvider::Rotations;
    if (ui->mdom->isChecked())
        filters |= QExtMouse3DEventProvider::DominantAxis;
    if (ui->msens->isChecked())
        filters |= QExtMouse3DEventProvider::Sensitivity;
    m_provider->setFilters(filters);

    filters = QExtMouse3DEventProvider::NoFilters;
    if (ui->ktrans->isChecked())
        filters |= QExtMouse3DEventProvider::Translations;
    if (ui->krot->isChecked())
        filters |= QExtMouse3DEventProvider::Rotations;
    if (ui->kdom->isChecked())
        filters |= QExtMouse3DEventProvider::DominantAxis;
    if (ui->ksens->isChecked())
        filters |= QExtMouse3DEventProvider::Sensitivity;
    m_provider->setKeyFilters(filters);
}

void MainWindow::updateSensitivity()
{
    int v = ui->senseSlider->value();
    qreal sense;
    if (v>=64)
        sense = v-63;
    else
        sense = 1.0/(65-v);
    m_provider->setSensitivity(sense);
}

bool MainWindow::event(QEvent *e)
 {
     if (e->type() == QExtMouse3DEvent::type) {
         QExtMouse3DEvent *event = static_cast<QExtMouse3DEvent *>(e);
         ui->textEdit->append(QString("Mouse event: TX=%1, TY=%2, TZ=%3, RX=%4, RY=%5, RZ=%6")
                              .arg(event->translateX()).arg(event->translateY()).arg(event->translateZ())
                              .arg(event->rotateX()).arg(event->rotateY()).arg(event->rotateZ()));
         return true;
     }
     return QWidget::event(e);
 }

 void MainWindow::keyPressEvent(QKeyEvent *e)
 {
     if (e->key() == QGL::Key_TopView) {
         ui->textEdit->append("Key event");
     }
     QWidget::keyPressEvent(e);
 }
