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
#include <QtDeclarative/qdeclarativeview.h>
#ifdef QT_BUILD_INTERNAL
#include <QtGui/private/qapplication_p.h>
#endif
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeerror.h>
//#include <QtDeclarative/qdeclarativecomponentview.h>
#include "qglview.h"
#include "qml3dview.h"
//#include <QtDeclarative/qdeclarativemetatype.h>
#include <QtGui/qvector3d.h>
#include <QtCore/qfile.h>
#include <QtCore/qvariantanimation.h>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // If "-graphicssystem OpenGL" was supplied, then enable "mixed mode".
    bool mixed = false;
    bool glViewport = false;
#ifdef QT_BUILD_INTERNAL
    if (QApplicationPrivate::graphics_system_name.compare
            (QLatin1String("opengl"), Qt::CaseInsensitive) == 0)
        mixed = true;
#endif
    if (QApplication::arguments().contains(QLatin1String("-opengl")))
        glViewport = true;

    QString source;
    for (int index = 1; index < argc; ++index) {
        QString arg = QString::fromLocal8Bit(argv[index]);
        if (arg.startsWith(QLatin1Char('-')))
            continue;   // TODO
        else
            source = arg;
    }
    if (source.isEmpty()) {
        qWarning() << "Usage: qml3d [-opengl] file.qml";
        return 1;
    }
    QUrl url(source);
    QFileInfo fi(source);
    if (fi.exists())
        url = QUrl::fromLocalFile(fi.absoluteFilePath());

    if (mixed || glViewport) {
        QDeclarativeView view;
        if (glViewport)
            view.setViewport(new QGLWidget());
        view.setSource(url);        
        view.show();

        return app.exec();
    } else {
        Qml3dView view;
        view.setSource(url);
        view.show();
        
        return app.exec();
    }
}
