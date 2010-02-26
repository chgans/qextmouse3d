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

#ifndef QGLDISPLAYLIST_H
#define QGLDISPLAYLIST_H

#include <QtCore/qvector.h>
#include <QtCore/qlist.h>
#include <QtGui/qvector3d.h>
#include <QtOpenGL/qgl.h>

#include "qglnamespace.h"
#include "qglscenenode.h"
#include "qlogicalvertex.h"
#include "qglattributevalue.h"
#include "qglvertexdescription.h"
#include "qglprimitive.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLSection;
class QGLMaterialCollection;
class QGLDisplayListPrivate;
class QGLPainter;

class Q_QT3D_EXPORT QGLDisplayList : public QGLSceneNode
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGLDisplayList);
public:
    QGLDisplayList(QObject *parent = 0, QGLMaterialCollection *materials = 0);
    virtual ~QGLDisplayList();

    void draw(QGLPainter *painter)
    {
        finalize();
        QGLSceneNode::draw(painter);
    }

    // section management
    void newSection(QGL::Smoothing sm = QGL::Smooth, QGL::Strategy st = QGL::HashLookup);

    // scene management
    QGLSceneNode *currentNode();
    QGLSceneNode *newNode();
    QGLSceneNode *pushNode();
    QGLSceneNode *popNode();

    void finalize();

    // geometry building by begin() / end()
    void begin(QGL::Operation);
    void end();

    void setFlags(QGL::OperationFlags flag);
    QGL::OperationFlags flags() const;

    void addVertex(const QVector3D &);
    void addVertex(qreal x, qreal y) { addVertex(QVector3D(x, y, 0.0)); }
    void addVertex(qreal x, qreal y, qreal z) { addVertex(QVector3D(x, y, z)); }

    void addNormal(const QVector3D &);
    void addColor(const QColor4B &);

    void addTexCoord(const QVector2D &, QGL::VertexAttribute attr = QGL::TextureCoord0);
    void addTexCoord(qreal s, qreal t, QGL::VertexAttribute attr = QGL::TextureCoord0)
    {
        addTexCoord(QVector2D(s, t), attr);
    }

    void addAttribute(const QVector3D &, QGL::VertexAttribute = QGL::CustomVertex0);

    void addVertexArray(const QVector3DArray &);
    void addNormalArray(const QVector3DArray &);
    void addColorArray(const QArray<QColor4B> &);
    void addTexCoordArray(const QVector2DArray &,
                          QGL::VertexAttribute attr = QGL::TextureCoord0);
    void addAttributeArray(const QCustomDataArray &,
                           QGL::VertexAttribute attr = QGL::CustomVertex0);

    // geometry building by primitive
    QGLPrimitive *currentPrimitive();
    void addTriangle(const QGLPrimitive &triangle);
    void addQuad(const QGLPrimitive &quad);
    void addTriangleFan(const QGLPrimitive &fan);
    void addTriangleStrip(const QGLPrimitive &strip);
    void addTriangulatedFace(const QGLPrimitive &face);
    void addQuadStrip(const QGLPrimitive &strip);
    void addQuadsZipped(const QGLPrimitive &top,
                        const QGLPrimitive &bottom);

protected:
    // internal and test functions
    QGLSection *currentSection() const;
    QList<QGLSection*> sections() const;

private slots:
    void deleteNode(QObject *);

private:
    Q_DISABLE_COPY(QGLDisplayList);
    void addSection(QGLSection *section);
    void addTriangle(int a, int b, int c, QGLPrimitive &p);
    void adjustSectionNodes(QGLSection *sec, int offset, QGeometryData *geom);
    int adjustNodeTree(QGLSceneNode *top, int offset, QGeometryData *geom);

    friend class QGLSection;

    QGLVertexArray toVertexArray() const;
    void setDirty(bool dirty);

#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug, const QGLSection &);
#endif
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLDISPLAYLIST_H
