#include "qglinfowindow.h"
#include "qglinfo.h"
#include "ui_qglinfowindow.h"
#include "aboutdialog.h"
#include "fpswidget.h"

#include <QtOpenGL/qgl.h>
#include <QtGui/qfiledialog.h>
#include <QtGui/qfont.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/QTextStream>

QGLInfoWindow::QGLInfoWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QGLInfoWindow)
{
    ui->setupUi(this);
    setStatusBar(0);
    if (QApplication::arguments().contains(QLatin1String("-maximize")) ||
        QApplication::arguments().contains(QLatin1String("-fullscreen")))
    {
        ui->verticalLayout->setMargin(1);
        ui->verticalLayout->setContentsMargins(1, 1, 1, 1);
        ui->verticalLayout_2->setMargin(1);
        ui->verticalLayout_2->setContentsMargins(1, 1, 1, 1);
        ui->menubar->setVisible(false);
        connect(ui->quitPushButton, SIGNAL(clicked()),
                this, SLOT(on_actionQuit_triggered()));
        connect(ui->runFPSTestPushButton, SIGNAL(clicked()),
                this, SLOT(on_actionRun_FPS_Test_triggered()));
        connect(ui->saveAsPushButton, SIGNAL(clicked()),
                this, SLOT(on_action_Save_As_triggered()));
        QFont f = font();
        f.setPointSize(f.pointSize() * 1.4);
        setFont(f);
    }
    else
    {
        ui->buttonsWidget->setVisible(false);
    }
    info = new QGLInfo();
    connect(info, SIGNAL(reportHtml(QString)),
            ui->textBrowser, SLOT(setHtml(QString)));
    ui->fpsWidget->setVisible(false);
}

QGLInfoWindow::~QGLInfoWindow()
{
    delete ui;
    delete info;
}

void QGLInfoWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void QGLInfoWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

void QGLInfoWindow::on_action_Save_As_triggered()
{
    QString defName = QDir::home().absoluteFilePath(QLatin1String("qglinfo.txt"));

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                defName,
                                tr("Text (*.txt)"));
    if (!fileName.isEmpty())
    {
        QFile saveFile(fileName);
        saveFile.open(QFile::WriteOnly);
        {
            QTextStream s(&saveFile);
            s << info->report();
        }
        saveFile.close();
    }
}

void QGLInfoWindow::on_actionAbout_triggered()
{
    AboutDialog *about = new AboutDialog(this);
    about->exec();
}

void QGLInfoWindow::on_actionRun_FPS_Test_triggered()
{
    fps = new FPSWidget(0);
    fps->setGeometry(rect());
    fps->move(this->pos());
    connect(fps, SIGNAL(fps(int)),
            ui->fpsLcdNumber, SLOT(display(int)));
    ui->fpsWidget->setVisible(true);
    fps->show();
}

