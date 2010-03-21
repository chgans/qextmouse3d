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
#include <QtGui/private/qapplication_p.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeerror.h>
//#include <QtDeclarative/qdeclarativecomponentview.h>
#include "qglview.h"
#include "qml3dview.h"
#include "item3d.h"
//#include <QtDeclarative/qdeclarativemetatype.h>
#include <QtGui/qvector3d.h>
#include <QtCore/qfile.h>
#include <QtCore/qvariantanimation.h>
#include <QFileInfo>
#include "viewport.h"
#include "effect.h"
#include "rotation3d.h"
#include "shaderprogram.h"
#include "cube.h"

static QVariant interpolateVector3D
    (const QVector3D &f, const QVector3D &t, qreal progress)
{
    return qVariantFromValue(f + (t - f) * progress);
}
	
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //We now have to register types at runtime rather than globally in the .cpp file.
    
    
    qmlRegisterType<Effect>("Qt",4,6,"Effect");
    qmlRegisterType<Mesh>("Qt",4,6,"Mesh");
    qmlRegisterType<Item3d>("Qt",4,6,"Item3d");
    qmlRegisterType<Viewport>("Qt",4,6,"Viewport");
    qmlRegisterType<QGLLightModel>("Qt",4,6,"LightModel");
    qmlRegisterType<QGLLightParameters>("Qt",4,6,"Light");
    qmlRegisterType<QGLCamera>("Qt",4,6,"Camera");
    qmlRegisterType<Rotation3D>("Qt",4,6,"Rotation3D");
    qmlRegisterType<QGLMaterial>("Qt",4,6,"Material");
    qmlRegisterType<ShaderProgram>("Qt",4,6,"ShaderProgram");
    qmlRegisterType<Cube>("Qt",4,6,"Cube");


/*  //Uncomment this section if you are not using the 4.7 branch (eg. you are using master, oslo staging 2, etc).
    QML_REGISTER_TYPE(Qt,4,6,Effect,Effect);
    QML_REGISTER_TYPE(Qt,4,6,Mesh,Mesh);
    QML_REGISTER_TYPE(Qt,4,6,Item3d,Item3d);
    QML_REGISTER_TYPE(Qt,4,6,Viewport,Viewport);
    QML_REGISTER_TYPE(Qt,4,6,LightModel,QGLLightModel);
    QML_REGISTER_TYPE(Qt,4,6,Light,QGLLightParameters);
    QML_REGISTER_TYPE(Qt,4,6,Camera,QGLCamera);
    QML_REGISTER_TYPE(Qt,4,6,Rotation3D,Rotation3D);
    QML_REGISTER_TYPE(Qt,4,6,Material,QGLMaterial);
    QML_REGISTER_TYPE(Qt,4,6,ShaderProgram,ShaderProgram);
    QML_REGISTER_TYPE(Qt,4,6,Cube,Cube);
*/
    // If "-graphicssystem OpenGL" was supplied, then enable "mixed mode".
    bool mixed = false;
    if (QApplicationPrivate::graphics_system_name.compare
            (QLatin1String("opengl"), Qt::CaseInsensitive) == 0)
        mixed = true;

    QString source;
    for (int index = 1; index < argc; ++index) {
        QString arg = argv[index];
        if (arg.startsWith(QChar('-')))
            continue;   // TODO
        else
            source = arg;
    }
    if (source.isEmpty()) {
        qWarning() << "Usage: qml3d file.qml [-graphicssystem opengl]";
        return 1;
    }
    QUrl url(source);
    QFileInfo fi(source);
    if (fi.exists())
        url = QUrl::fromLocalFile(fi.absoluteFilePath());

    // Register an interpolator for linear movement along vector paths.
    qRegisterAnimationInterpolator<QVector3D>(interpolateVector3D);

    if (mixed) {
        QDeclarativeView view;
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
