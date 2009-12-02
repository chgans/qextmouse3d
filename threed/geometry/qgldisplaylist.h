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

#include <QtCore/qobject.h>
#include <QtCore/qvector.h>
#include <QtCore/qlist.h>
#include <QtGui/qvector3d.h>
#include <QtOpenGL/qgl.h>

#include "qgltexturespecifier.h"
#include "qglgeometry.h"
#include "qglnamespace.h"
#include "qglsection.h"
#include "qlogicalvertex.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLSection;
class QGLMaterialCollection;
class QGLDisplayListPrivate;
class QGLPainter;

class Q_QT3D_EXPORT QGLDisplayList : public QGLGeometry
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGLDisplayList);
public:
    QGLDisplayList(QObject *parent = 0, QGLMaterialCollection *materials = 0);
    virtual ~QGLDisplayList();

    // drawing
    void draw(QGLPainter *painter);

    // section management
    QGLSection *newSection(QGL::Smoothing = QGL::Smooth);
    QGLSection *currentSection();
    QVector<QGLSection*> &sections();

    // geometry building
    void reserve(int n);
    void addTriangle(const QVector3D &a, const QVector3D &b,
                     const QVector3D &c, const QVector3D &n = QVector3D(),
                     const QGLTextureSpecifier &textureModel = QGLTextureSpecifier(),
                     bool inverted = false);
    void addQuad(const QVector3D &a, const QVector3D &b,
                 const QVector3D &c, const QVector3D &d,
                 const QGLTextureSpecifier &textureModel = QGLTextureSpecifier());
    void addTriangleFan(const QVector3D &center,
                        const QGL::VectorArray &edges,
                        const QGLTextureSpecifier &textureModel = QGLTextureSpecifier());
    void addTriangulatedFace(const QVector3D &center,
                             const QGL::VectorArray &edges,
                             const QGLTextureSpecifier &textureModel = QGLTextureSpecifier());
    QVector<QVector3D> extrude(const QGL::VectorArray &edges,
                               const QVector3D &dir = QVector3D(),
                               const QGLTextureSpecifier &textureModel = QGLTextureSpecifier());

    // data accessors
    inline QGLMaterialCollection *materials() const;
protected:
    virtual void finalize();
    virtual void loadArrays(QGLPainter *painter);

    virtual void addSection(QGLSection *section);
private:
    Q_DISABLE_COPY(QGLDisplayList);

    friend class QGLSection;

    QGLVertexArray toVertexArray() const;
    void setDirty(bool dirty);
    void draw(QGLPainter *, int, int);

#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug, const QGLSection &);
#endif
    QGLMaterialCollection *m_materials;
    QVector<QGLSection *> m_sections;
    QGLSection *m_currentSection;
};


inline QGLSection *QGLDisplayList::currentSection()
{
    return m_currentSection;
}

inline QVector<QGLSection*> &QGLDisplayList::sections()
{
    return m_sections;
}

inline QGLMaterialCollection *QGLDisplayList::materials() const
{
    return m_materials;
}

inline void QGLDisplayList::draw(QGLPainter *painter, int start, int count)
{
    QGLGeometry::draw(painter, start, count);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLDISPLAYLIST_H
