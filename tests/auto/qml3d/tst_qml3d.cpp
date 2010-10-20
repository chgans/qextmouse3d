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

#include <QApplication>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtOpenGL/qgl.h>
#include <QtCore/qurl.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qdir.h>
#include <QtCore/qdebug.h>
#include <QtCore/qeventloop.h>
#include <stdio.h>

class QuitObject : public QObject
{
    Q_OBJECT
public:
    QuitObject(QObject *parent = 0) : QObject(parent), hasQuit(false) {}

    bool hasQuit;

private Q_SLOTS:
    void quit() { hasQuit = true; }
};

static int passed = 0;
static int failed = 0;
static int skipped = 0;

class TestReport : public QObject
{
    Q_OBJECT
public:
    TestReport(QObject *parent = 0) : QObject(parent) {}

public Q_SLOTS:
    void report(int pass, int fail, int skip);
};

QML_DECLARE_TYPE(TestReport)

void TestReport::report(int pass, int fail, int skip)
{
    passed += pass;
    failed += fail;
    skipped += skip;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Determine where to look for the test data.  On a device it will
    // typically be necessary to set QML3D_TEST_SOURCE_DIR.
    QString testPath = QString::fromLocal8Bit(qgetenv("QML3D_TEST_SOURCE_DIR"));
#ifdef QML3D_TEST_SOURCE_DIR
    if (testPath.isEmpty())
        testPath = QString::fromLocal8Bit(QML3D_TEST_SOURCE_DIR);
#endif
    if (testPath.isEmpty())
        testPath = QLatin1String(".");

    // Find the subdirectories that look like they may contain test cases.
    // The "Qt3D" directory contains the common QML and JS test logic.
    QDir dir(testPath);
    QStringList entries = dir.entryList(QDir::Dirs);
    entries.removeAll(QLatin1String("."));
    entries.removeAll(QLatin1String(".."));
    entries.removeAll(QLatin1String("Qt3D"));

    fprintf(stderr, "********* Start testing of tst_qml3d *********\n");

    qmlRegisterType<TestReport>("Qt3D.Test", 1, 0, "TestReport");

    // Scan through all of the "*.qml" files in the subdirectories
    // and run each of them in turn with a QDeclarativeView.
    QStringList filters;
    filters += QLatin1String("tst_*.qml");
    foreach (QString name, entries) {
        QDir subdir(testPath + QDir::separator() + name);
        QStringList files = subdir.entryList(filters, QDir::Files);
        foreach (QString file, files) {
            QString source = subdir.path() + QDir::separator() + file;
            QFileInfo fi(source);
            if (fi.exists()) {
                QDeclarativeView view;
                QuitObject quitobj;
                QEventLoop eventLoop;
                QObject::connect(view.engine(), SIGNAL(quit()),
                                 &quitobj, SLOT(quit()));
                QObject::connect(view.engine(), SIGNAL(quit()),
                                 &eventLoop, SLOT(quit()));
                view.setViewport(new QGLWidget());
                view.engine()->addImportPath(testPath);
                view.setSource(QUrl::fromLocalFile(fi.absoluteFilePath()));
                if (view.status() == QDeclarativeView::Error) {
                    // Error compiling the test - flag failure and continue.
                    ++failed;
                    continue;
                }
                if (!quitobj.hasQuit) {
                    // If the test already quit, then it was performed
                    // synchronously during setSource().  Otherwise it is
                    // an asynchronous test and we need to show the window
                    // and wait for the quit indication.
                    view.show();
                    eventLoop.exec();
                }
            }
        }
    }

    fprintf(stderr, "Totals: %d passed, %d failed, %d skipped\n",
            passed, failed, skipped);
    fprintf(stderr, "********* Finished testing of tst_qml3d *********\n");
    return failed != 0;
}

#include "tst_qml3d.moc"
