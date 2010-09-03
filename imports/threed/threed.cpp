/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarative.h>

#include "redcyaneffect.h"
#include "item3d.h"
#include "viewport.h"
#include "effect.h"
#include "qgraphicsrotation3d.h"
#include "qgraphicstranslation3d.h"
#include "qgraphicsscale3d.h"
#include "qgraphicsfacecamera.h"
#include "shaderprogram.h"
#include "qt3dnamespace.h"
#include "qglscenenode.h"

QT_BEGIN_NAMESPACE

QML_DECLARE_TYPE(QGraphicsRotation3D)
QML_DECLARE_TYPE(QGraphicsTranslation3D)
QML_DECLARE_TYPE(QGraphicsScale3D)
QML_DECLARE_TYPE(QGraphicsFaceCamera)

class QThreedQmlModule : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Qt.labs.threed"));
        qmlRegisterType<QGLSceneNode>(uri,1,0,"SceneNode");
        qmlRegisterType<RedCyanEffect>(uri,1,0,"RedCyan");
        qmlRegisterType<Effect>(uri,1,0,"Effect");
        qmlRegisterType<Mesh>(uri,1,0,"Mesh");
        qmlRegisterType<Item3d>(uri,1,0,"Item3d");
        qmlRegisterType<Viewport>(uri,1,0,"Viewport");
        qmlRegisterType<QGLLightModel>(uri,1,0,"LightModel");
        qmlRegisterType<QGLLightParameters>(uri,1,0,"Light");
        qmlRegisterType<QGLCamera>(uri,1,0,"Camera");
        qmlRegisterType<QGraphicsRotation3D>(uri,1,0,"Rotation3D");
        qmlRegisterType<QGraphicsTranslation3D>(uri,1,0,"Translation3D");
        qmlRegisterType<QGraphicsScale3D>(uri,1,0,"Scale3D");
        qmlRegisterType<QGraphicsFaceCamera>(uri,1,0,"FaceCamera");
        qmlRegisterType<QGLMaterial>(uri,1,0,"Material");
        qmlRegisterType<ShaderProgram>(uri,1,0,"ShaderProgram");
    }
    void initializeEngine(QDeclarativeEngine *engine, const char *uri)
    {
        Q_UNUSED(uri);
        QDeclarativeContext *context = engine->rootContext();
        context->setContextProperty("Qt3d", new Qt3dNamespace);
    }
};

QT_END_NAMESPACE

#include "threed.moc"

Q_EXPORT_PLUGIN2(threedqmlmodule, QT_PREPEND_NAMESPACE(QThreedQmlModule));
