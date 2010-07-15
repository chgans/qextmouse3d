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

#ifndef QGLBuilder_H
#define QGLBuilder_H

#include <QtCore/qvector.h>
#include <QtCore/qlist.h>
#include <QtGui/qvector3d.h>
#include <QtOpenGL/qgl.h>

#include "qglnamespace.h"
#include "qglscenenode.h"
#include "qlogicalvertex.h"
#include "qglattributevalue.h"
#include "qgeometrydata.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLSection;
class QGLMaterialCollection;
class QGLBuilderPrivate;
class QGLPainter;

class Q_QT3D_EXPORT QGLBuilder
{
public:
    explicit QGLBuilder(QGLMaterialCollection *materials = 0);
    virtual ~QGLBuilder();

    // section management
    void newSection(QGL::Smoothing sm = QGL::Smooth);

    // scene management
    QGLSceneNode *sceneNode();
    QGLSceneNode *currentNode();
    QGLSceneNode *newNode();
    QGLSceneNode *pushNode();
    QGLSceneNode *popNode();
    QGLMaterialCollection *palette();
    QGLSceneNode *finalizedSceneNode();

    // geometry building by primitive
    void addTriangles(const QGeometryData &triangle);
    void addQuads(const QGeometryData &quad);
    void addTriangleFan(const QGeometryData &fan);
    void addTriangleStrip(const QGeometryData &strip);
    void addTriangulatedFace(const QGeometryData &face);
    void addQuadStrip(const QGeometryData &strip);
    void addQuadsInterleaved(const QGeometryData &top,
                        const QGeometryData &bottom);

protected:
    // internal and test functions
    QGLSection *currentSection() const;
    QList<QGLSection*> sections() const;
    void setDefaultThreshold(int);

private:
    Q_DISABLE_COPY(QGLBuilder);
    void addSection(QGLSection *section);
    void setDirty(bool dirty);

    friend class QGLSection;

    QGLBuilderPrivate *dptr;
};

Q_QT3D_EXPORT QGLBuilder& operator<<(QGLBuilder& builder, const QGL::Smoothing& smoothing);
Q_QT3D_EXPORT QGLBuilder& operator<<(QGLBuilder& builder, const QGeometryData& triangles);

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLBuilder_H
