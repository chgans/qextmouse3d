/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgltexturemodel.h"
#include "qglcolormodel.h"
#include "qglnamespace.h"
#include "qglscenenode.h"
#include "qlogicalvertex_p.h"

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
    void newSection(QGL::Smoothing = QGL::Smooth);

    // scene management
    QGLSceneNode *currentNode();
    QGLSceneNode *newNode();
    QGLSceneNode *pushNode();
    QGLSceneNode *popNode();

    void finalize();

    // geometry building by begin() / end()
    enum Operation
    {
        NO_OP,
        TRIANGLE,
        TRIANGLE_STRIP,
        QUAD,
        QUAD_STRIP,
        TRIANGLE_FAN,
        TRIANGULATED_FACE,
        EXTRUSION
    };
    enum OperationFlag
    {
        NO_FLAG               = 0X00,
        FACE_SENSE_REVERSED   = 0x01,
        NO_CLOSE_PATH         = 0x02
    };
    Q_DECLARE_FLAGS(OperationFlags, OperationFlag);
    void begin(Operation, const QVector3D &control = QVector3D());
    void end();
    QGL::VectorArray endResult();
    void setControl(const QVector3D &control);
    QVector3D control() const;
    void setFlags(OperationFlags flag);
    OperationFlags flags() const;
    void addVertex(const QVector3D &);
    void addNormal(const QVector3D &);
    void addColor(const QColor4b &);
    void addTexCoord(const QVector2D &);
    void addVertexArray(const QGL::VectorArray &);
    void addNormalArray(const QGL::VectorArray &);
    void addColorArray(const QGL::ColorArray &);
    void addTexCoordArray(const QGL::TexCoordArray &);
    void addTextureModel(const QGLTextureModel &);
    void addColorModel(const QGLColorModel &);

    // low level geometry building
    void addTriangle(const QVector3D &a, const QVector3D &b,
                     const QVector3D &c, const QVector3D &n = QVector3D(),
                     const QGLTextureModel *textureModel = 0,
                     const QGLColorModel *colorModel = 0,
                     bool inverted = false);
    void addQuad(const QVector3D &a, const QVector3D &b,
                 const QVector3D &c, const QVector3D &d,
                 const QGLTextureModel *textureModel = 0,
                 const QGLColorModel *colorModel = 0);
    void addTriangleFan(const QVector3D &center,
                        const QGL::VectorArray &edges,
                        const QGLTextureModel *textureModel = 0,
                        const QGLColorModel *colorModel = 0);
    void addTriangulatedFace(const QVector3D &center,
                             const QGL::VectorArray &edges,
                             const QGLTextureModel *textureModel = 0,
                             const QGLColorModel *colorModel = 0,
                             bool closePath = true);
    QVector<QVector3D> extrude(const QGL::VectorArray &edges,
                               const QVector3D &dir = QVector3D(),
                               const QGLTextureModel *textureModel = 0,
                               const QGLColorModel *colorModel = 0,
                               bool reverse = false,
                               bool closePath = true);

protected:
    // internal and test functions
    QGLSection *currentSection() const;
    QList<QGLSection*> sections() const;

private:
    Q_DISABLE_COPY(QGLDisplayList);
    void addSection(QGLSection *section);

    friend class QGLSection;

    QGLVertexArray toVertexArray() const;
    void setDirty(bool dirty);

#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug, const QGLSection &);
#endif
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLDisplayList::OperationFlags)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLDISPLAYLIST_H
