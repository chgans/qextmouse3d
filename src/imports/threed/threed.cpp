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

#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarative.h>

#include "item3d.h"
#include "viewport.h"
#include "effect.h"
#include "scale3d.h"
#include "qgraphicsrotation3d.h"
#include "qgraphicstranslation3d.h"
#include "qgraphicsscale3d.h"
#include "qgraphicsbillboardtransform.h"
#include "shaderprogram.h"
#include "qt3dnamespace.h"
#include "qglscenenode.h"
#include "floatingitem.h"
#include "stereoview.h"

QT_BEGIN_NAMESPACE

QML_DECLARE_TYPE(QGraphicsTransform3D)
QML_DECLARE_TYPE(QGraphicsRotation3D)
QML_DECLARE_TYPE(QGraphicsTranslation3D)
QML_DECLARE_TYPE(QGraphicsScale3D)
QML_DECLARE_TYPE(QGraphicsBillboardTransform)

class QThreedQmlModule : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Qt3D"));
        qmlRegisterType<QGLSceneNode>(uri,1,0,"SceneNode");
        qmlRegisterType<Effect>(uri,1,0,"Effect");
        qmlRegisterType<Mesh>(uri,1,0,"Mesh");
        qmlRegisterType<Item3D>(uri,1,0,"Item3D");
        qmlRegisterType<Viewport>(uri,1,0,"Viewport");
        qmlRegisterType<QGLLightModel>(uri,1,0,"LightModel");
        qmlRegisterType<QGLLightParameters>(uri,1,0,"Light");
        qmlRegisterType<QGLCamera>(uri,1,0,"Camera");
        qmlRegisterType<QGraphicsRotation3D>(uri,1,0,"Rotation3D");
        qmlRegisterType<QGraphicsTranslation3D>(uri,1,0,"Translation3D");
        qmlRegisterType<Scale3D>(uri,1,0,"Scale3D");
        qmlRegisterType<QGraphicsBillboardTransform>(uri,1,0,"BillboardTransform");
        qmlRegisterType<QGLMaterial>(uri,1,0,"Material");
#if !defined(QT_OPENGL_ES_1)
        qmlRegisterType<ShaderProgram>(uri,1,0,"ShaderProgram");
#endif

        qmlRegisterType<FloatingItem>(uri,1,0,"FloatingItem");
        qmlRegisterType<StereoView>(uri,1,0,"StereoView");

        // Needed to make QDeclarativeListProperty<QGraphicsTransform3D> work.
        qmlRegisterType<QGraphicsTransform3D>();
        qmlRegisterType<QGraphicsScale3D>();
    }
    void initializeEngine(QDeclarativeEngine *engine, const char *uri)
    {
        Q_UNUSED(uri);
        QDeclarativeContext *context = engine->rootContext();
        context->setContextProperty(QLatin1String("Qt3d"), new Qt3dNamespace);
    }
};

QT_END_NAMESPACE

#include "threed.moc"

Q_EXPORT_PLUGIN2(threedqmlmodule, QT_PREPEND_NAMESPACE(QThreedQmlModule));
